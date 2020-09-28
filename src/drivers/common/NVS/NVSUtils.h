#ifndef _NVSUTILS_H
#define _NVSUTILS_H

#include <Arduino.h>
#include <Preferences.h>
#include <esp_partition.h>
#include <nvs_flash.h>

typedef enum { NVST_SIGNED=0, NVST_UNSIGNED=1, NVST_STRING=2, NVST_BLOB=4 } NVSTypes;

struct NVSEntry
{
  uint8_t  Ns ;         // Namespace ID
  uint8_t  Type ;       // Type of value
  uint8_t  Span ;       // Number of entries used for this item
  uint8_t  Rvs ;        // Reserved, should be 0xFF
  uint32_t CRC ;        // CRC
  char     Key[16] ;    // Key in Ascii
  uint64_t Data ;       // Data in entry
};

struct NVSPage // For nvs entries
{              // 1 page is 4096 bytes
  uint32_t  State ;
  uint32_t  Seqnr ;

  uint32_t  Unused[5] ;
  uint32_t  CRC ;
  uint8_t   Bitmap[32] ;
  struct NVSEntry Entry[126] ;
};

struct NSIndex
{
  uint16_t names[256];
  uint16_t next;
  char     buf[2048];
};

struct NVSIterator
{
  struct NVSPage           page ;      // Holds current page in partition
  const esp_partition_t*   nvs ;       // Pointer to partition struct
  char                     prefix[16];
  struct NSIndex          *nsindex;    // Holds namespace names

  uint8_t                  namespace_ID;
  uint32_t                 offset;     // Offset in nvs partition
  int32_t                  ix;         // Index in Entry 0..125
};


typedef struct NVSIterator* NVSIteratorPtr;

typedef struct NVSIterType
{
  // From ItemType in nvs_types.hpp
  NVSTypes nvs_type;
  uint8_t bytewid;
} NVSIteratorTyped;


class NVSUtils {

  public:

    NVSUtils();
    int            Dump( const char* searchStr = nullptr );
    int            Erase();

  private:

    char          *IterateGetNamespace( struct NVSIterator *it, const char *key );
    char          *IterateTypeToString( NVSIteratorTyped *nityp, char *buf );
    uint64_t       IterateGetData( const char *key );
    void           IterateDone( struct NVSIterator *it );
    bool           IterateNext( struct NVSIterator *it, const char **key, NVSIteratorTyped *typ );
    NVSIteratorPtr IterateNew( const char *nmspace, const char *prefix );
    uint8_t        FindNsID( const esp_partition_t* nvs, const char* ns, struct NSIndex **indexP );

};


#endif
