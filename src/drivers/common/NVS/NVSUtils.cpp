/*\
 *
 * NVS Dumper
 *
 * Copyleft tobozo 2020
 *
 * Inspired from the following implementations:
 *
 *   - https://github.com/Edzelf/ESP32-Show_nvs_keys/
 *   - https://gist.github.com/themadsens/026d38f432727567c3c456fb4396621b
 *
\*/

#include "NVSUtils.h"

NVSUtils::NVSUtils() {
  //Dump( nullptr );
}

int NVSUtils::Erase() {
  esp_err_t result = nvs_flash_erase();
  if( result != ESP_OK ) return -1;
  return 0;
}

char *NVSUtils::IterateTypeToString(struct NVSIterType *nityp, char *buf)
{
  strcpy(buf, nityp->nvs_type == NVST_SIGNED ? "int" :
              nityp->nvs_type == NVST_UNSIGNED ? "uint" :
              nityp->nvs_type == NVST_STRING ? "string" :
              nityp->nvs_type == NVST_BLOB ? "blob" : "<unknown>"
  );
  if (nityp->nvs_type == NVST_SIGNED || nityp->nvs_type == NVST_UNSIGNED) {
    sprintf(buf + strlen(buf), "%d", 8 * nityp->bytewid);
  }
  return buf;
}

uint64_t NVSUtils::IterateGetData(const char *key)
{
  return *((uint64_t *) (key + 16));
}

char *NVSUtils::IterateGetNamespace(struct NVSIterator *it, const char *key)
{
  size_t off = offsetof(struct NVSEntry, Key);
  struct NVSEntry *ent = (struct NVSEntry *) (key - off);
  return NULL == it->nsindex ? (char*)"<NONE>" : (it->nsindex->buf + it->nsindex->names[ent->Ns]);
}

// Find the namespace ID for the namespace passed as parameter.                                    *
uint8_t NVSUtils::FindNsID(const esp_partition_t* nvs, const char* ns, struct NSIndex **indexP)
{
  esp_err_t      result = ESP_OK; // Result of reading partition
  uint32_t       offset = 0;      // Offset in nvs partition
  uint8_t        bm;              // Bitmap for an entry
  uint8_t        res = 0xFF;      // Function result
  struct NVSPage *page = (struct NVSPage*)malloc(sizeof(struct NVSPage));
  struct NSIndex *index = NULL;

  if ( NULL == ns ) {
    index = (struct NSIndex *) malloc(sizeof(struct NSIndex));
    strcpy(index->buf, "<OVF>");
    index->next = 6;
    *indexP = index;
  }

  while ( offset < nvs->size ) {
    // Read 1 page in nvs partition
    result = esp_partition_read ( nvs, offset,  page, sizeof(struct NVSPage) );
    log_v("NSREAD: %d/%d -> %d", offset, nvs->size, result);
    if ( result != ESP_OK ) {
      log_e( "Error reading NVS!" );
      break;
    }
    int i = 0;
    while ( i < 126 ) {
      // Get bitmap for this entry
      bm = ( page->Bitmap[i/4] >> ( ( i % 4 ) * 2 ) ) & 0x03;
      if ( ( bm == 2 ) && ( page->Entry[i].Ns == 0 ) && ( NULL == ns || strcmp ( ns, page->Entry[i].Key ) == 0 ) )  {
        // Return the ID
        res = page->Entry[i].Data & 0xFF;
        if ( NULL == ns ) {
          int len = strlen(page->Entry[i].Key) + 1;
          log_v("NS(%d): %s", res, page->Entry[i].Key);
          if (index->next + len > 2048) {
            index->names[res] = 0;
          } else {
            index->names[res] = index->next;
            strcpy(index->buf + index->next, page->Entry[i].Key);
            index->next += len;
          }
          i += page->Entry[i].Span;// Next entry
        } else {
          log_v("Found NS(%d): %s", res, page->Entry[i].Key);
          offset = nvs->size;  // Stop outer loop as well
          break;
        }
      } else {
        if ( bm == 2 ) {
          i += page->Entry[i].Span;                          // Next entry
          log_v("KEY(%d): %s", page->Entry[i].Ns, page->Entry[i].Ns == 0xFF ? "<ANY>" : page->Entry[i].Key);
        } else {
          i++;
        }
      }
    }
    offset += sizeof(struct NVSPage); // Prepare to read next page in nvs
  }
  free(page);
  return res;
}

NVSIteratorPtr NVSUtils::IterateNew(const char *strNamespace, const char *prefix)
{
  esp_partition_iterator_t  pi;  // Iterator for find
  const esp_partition_t*    nvs; // Pointer to partition struct
  struct NVSIterator *it;
  struct NSIndex *index;
  uint8_t namespace_ID;
  // Get partition iterator for this partition
  pi = esp_partition_find ( ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "nvs" );
  if ( pi ) {
    nvs = esp_partition_get ( pi );        // Get partition struct
    esp_partition_iterator_release ( pi ); // Release the iterator
  } else {
    log_e( "NVS Partition not found!" );
    return NULL;
  }
  if ( strNamespace && *strNamespace ) {
    namespace_ID = FindNsID ( nvs, strNamespace, NULL );
    index = NULL;
    if ( namespace_ID == 0xFF ) {
      log_e( "NVS namespace not found!" );
      return NULL;
    }
  } else {
    namespace_ID = 0xFF;
    FindNsID ( nvs, NULL, &index );
  }

  it = (struct NVSIterator *) malloc(sizeof(struct NVSIterator));
  assert(it);

  it->namespace_ID = namespace_ID;
  strcpy(it->prefix, prefix ? prefix : "");
  it->nvs = nvs;
  it->ix = -1;
  it->offset = 0;
  it->nsindex = index;
  log_v("OFF:%d/%d IX:%d NS:%d", it->offset, it->nvs->size, it->ix, it->namespace_ID);
  return it;
}

void NVSUtils::IterateDone(struct NVSIterator *it)
{
  if ( NULL != it ) {
    if ( it->nsindex ) free(it->nsindex);
    free(it);
  }
}

bool NVSUtils::IterateNext(struct NVSIterator *it, const char **key, NVSIteratorTyped *typ)
{
  esp_err_t result = ESP_OK;

  log_v("OFF:%d/%d IX:%d", it->offset, it->nvs->size, it->ix);
  while ( it->offset < it->nvs->size ) {
    if (it->ix < 0 || it->ix >= 126) {
      // Read 1 page in nvs partition
      result = esp_partition_read ( it->nvs, it->offset, &it->page, sizeof(struct NVSPage) );
      log_v("NSREAD: %d/%d -> %d", it->offset, it->nvs->size, result);
      if ( result != ESP_OK ) {
        log_e( "Error reading NVS!" );
        return  false;
      }
      it->ix = 0;
      it->offset += sizeof(struct NVSPage); // Prepare to read next page in nvs
    }

    while ( it->ix < 126 ) {
      uint8_t bm = ( it->page.Bitmap[it->ix/4] >> ( ( it->ix % 4 ) * 2 ) ) & 0x03;  // Get bitmap for this entry
      if ( bm == 2 ) {
        const char *ky = it->page.Entry[it->ix].Key;
        uint8_t tp = it->page.Entry[it->ix].Type;
        int ns = it->page.Entry[it->ix].Ns;
        it->ix += it->page.Entry[it->ix].Span; // Next entry
        // Show all if ID = 0xFF  otherwise just my namespace
        if(  ( ( it->namespace_ID == 0xFF && ns > 0 && ns < 0xFF ) ||  ns == it->namespace_ID )
          && ( ! it->prefix[0] || 0 == strncmp(it->prefix, ky, strlen(it->prefix)) )
        ) {
          typ->nvs_type = (NVSTypes)((tp >> 4) & 0xf);
          typ->bytewid = tp & 0xf;
          *key = ky;
          return true;
        }
      } else {
        it->ix++;
      }
    }
  }
  return false;
}


int NVSUtils::Dump( const char* searchStr)
{
  //bool all = ( searchStr==nullptr || searchStr[0] == '\0' );
  const char *key;
  NVSIteratorTyped typ;

  //NVSIteratorPtr it = IterateNew( all ? NULL : "nvs-data", all ? NULL : searchStr );
  NVSIteratorPtr it = IterateNew( NULL, NULL );

  if (!it) {
    printf("Can not iterate nvs\n");
    return -1;
  }

  printf("*********************************\n  NVS Dump\n*********************************\n");

  char buf[10];
  while (IterateNext(it, &key, &typ)) {
    char val[200];
    uint64_t data = IterateGetData(key);
    const char *nsStr = IterateGetNamespace(it, key);
    const char *typStr = IterateTypeToString(&typ, buf);

    switch( typ.nvs_type ) {
      case NVST_SIGNED:
      case NVST_UNSIGNED:
      {
        uint64_t ival = 0;
        memcpy(&ival, &data, typ.bytewid);
        sprintf(val, "%llu", ival);
      }
      break;
      case NVST_STRING:
      {
        Preferences prefs;
        prefs.begin(nsStr, true);
        prefs.getString( key, val, 200);
        prefs.end();
      }
      break;
      case NVST_BLOB:
        val[0] = '*';
        val[1] = 0;
      break;
    }
    printf("%-6s(%d) %15s:%-15s = %s\n",
      typStr,
      typ.bytewid,
      nsStr,
      key,
      val
    );
  }
  IterateDone(it);
  return 0;
}

