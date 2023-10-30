
#include "esp32-hal-log.h"
#include "AviMjpegEncoder.hpp"

#if defined HAS_JPEGENC

  AviMjpegEncoder* JPGENC_AVI_Proxy_t::aviEncoder = nullptr;


  void* JPGENC_AVI_Proxy_t::open(const char *filename)
  {
    assert( JPGENC_AVI_Proxy_t::aviEncoder );
    JPGENC_AVI_Proxy_t::aviEncoder->writeJpegFrameHeader(0); // write empty header
    return (void *)JPGENC_AVI_Proxy_t::aviEncoder;
  }


  int32_t JPGENC_AVI_Proxy_t::write(JPEGFILE *p, uint8_t *buffer, int32_t length)
  {
    auto aviEncoder = (AviMjpegEncoder*)p->fHandle;
    aviEncoder->writeJpegFrameData(buffer, length);
    return length;
  }


  void JPGENC_AVI_Proxy_t::close(JPEGFILE *p)
  {
    auto aviEncoder = (AviMjpegEncoder*)p->fHandle;
    aviEncoder->writeJpegFrameFinish();
  }


  int32_t JPGENC_AVI_Proxy_t::read(JPEGFILE *p, uint8_t *buffer, int32_t length)
  {
    auto aviEncoder = (AviMjpegEncoder*)p->fHandle;
    return aviEncoder->aviFile.readBytes((char*)buffer, length);
  }


  int32_t JPGENC_AVI_Proxy_t::seek(JPEGFILE *p, int32_t position)
  {
    auto aviEncoder = (AviMjpegEncoder*)p->fHandle;
    return aviEncoder->aviFile.seek(position);
  }

#endif



AviMjpegEncoder::AviMjpegEncoder( AVI_Params_t *params ) : params( params )
{
  assert( params );
  assert( params->fs );
  assert( params->path );
  assert( params->size.w>0 && params->size.h>0 );

  params->ready = false;
  aviFile = params->fs->open( params->path, "w" );

  if( ! aviFile ) {
    log_e("Unable to open AVI file %s for writing", params->path.c_str() );
    return;
  }
  log_d("Opened AVI file %s for writing", params->path.c_str() );

  if( params->use_index_file ) {
    idxFile = params->fs->open(tmpIdxPath, "w");
    if( ! idxFile ) {
      log_e("Unable to open IDX file %s for writing", tmpIdxPath );
      return;
    }
    log_d("Opened IDX file %s for writing", tmpIdxPath );
  }

  writeAviHeader();
  params->ready = true;
  log_d("AviMjpegEncoder is ready to encode");
}



void AviMjpegEncoder::writeAviHeader()
{
  framesCountFieldPos1 = framesCountFieldPos2 = moviPos = jpegFramePos = jpegFrameSize = idx_items_count = 0;
  lengthFields.clear();
  imgIndex.clear();

  // Note: indentation reflects AVI hierarchy

  // Write AVI header
  aviFile.writeWordStr("RIFF");              // RIFF type
  writeLengthField("RIFF");                  // File length (remaining bytes after this field) (nesting level 0)
    aviFile.writeWordStr("AVI ");            // AVI signature
    aviFile.writeWordStr("LIST");            // LIST chunk: data encoding
    writeLengthField("hdrl");                // Chunk length (nesting level 1)
      aviFile.writeWordStr("hdrl");          // LIST chunk type
      aviFile.writeWordStr("avih");          // avih sub-chunk
      aviFile.writeDword(56);                // Sub-chunk length excluding the first 8 bytes of avih signature and size
      aviFile.writeDword(1000000/params->fps); // Frame delay time in microsec
      aviFile.writeDword(7000);              // dwMaxBytesPerSec (maximum data rate of the file in bytes per second)
      aviFile.writeDword(0);                 // Reserved
      aviFile.writeDword(0x10);              // dwFlags, 0x10 bit: AVIF_HASINDEX (the AVI file has an index chunk at the end of the file - for good performance);
                                             // Windows Media Player can't even play it if index is missing!
      framesCountFieldPos1 = aviFile.position();
      aviFile.writeDword(0);                 // Number of frames
      aviFile.writeDword(0);                 // Initial frame for non-interleaved files; non interleaved files should set this to 0
      aviFile.writeDword(1);                 // Number of streams in the video; here 1 video, no audio
      aviFile.writeDword(0);                 // dwSuggestedBufferSize
      aviFile.writeDword(params->size.w);    // Image width in pixels
      aviFile.writeDword(params->size.h);    // Image height in pixels
      aviFile.writeDword(0);                 // Reserved
      aviFile.writeDword(0);
      aviFile.writeDword(0);
      aviFile.writeDword(0);
      // Write stream information
      aviFile.writeWordStr("LIST");          // LIST chunk: stream headers
      writeLengthField("strl");      // Chunk size (nesting level 2)
        aviFile.writeWordStr("strl");        // LIST chunk type: stream list
        aviFile.writeWordStr("strh");        // Stream header // STRH Primitive Chunk (First child of STRL main Header Chunk)
        aviFile.writeDword(56);              // fixed Length of the strh sub-chunk
        aviFile.writeWordStr("vids");        // fccType - type of data stream - here 'vids' for video stream
        aviFile.writeWordStr("MJPG");        // MJPG for Motion JPEG
        aviFile.writeDword(0);               // dwFlags
        aviFile.writeDword(0);               // wPriority, wLanguage
        aviFile.writeDword(0);               // dwInitialFrames
        aviFile.writeDword(1);               // dwScale
        aviFile.writeDword(params->fps);     // dwRate, Frame rate for video streams (the actual FPS is calculated by dividing this by dwScale)
        aviFile.writeDword(0);               // usually zero
        framesCountFieldPos2 = aviFile.position();
        aviFile.writeDword(0);               // dwLength, playing time of AVI file as defined by scale and rate (set equal to the number of frames)
        aviFile.writeDword(0);               // dwSuggestedBufferSize for reading the stream (typically, this contains a value corresponding to the largest chunk in a stream)
        aviFile.writeDword(-1);              // dwQuality, encoding quality given by an integer between (0 and 10,000.  If set to -1, drivers use the default quality value)
        aviFile.writeDword(0);               // dwSampleSize, 0 means that each frame is in its own chunk
        aviFile.writeWord(0);                // left of rcFrame if stream has a different size than dwWidth*dwHeight(unused)
        aviFile.writeWord(0);                //   ..top
        aviFile.writeWord(0);                //   ..right
        aviFile.writeWord(0) ;               //   ..bottom
        // end of 'strh' chunk, stream format follows
        aviFile.writeWordStr("strf");        // stream format chunk // STRF Primitive Chunk (Second child of STRL main Header Chunk located right after STRH)
        writeLengthField("strf");            // Chunk size (nesting level 3)
          aviFile.writeDword(40);            // biSize, write header size of BITMAPINFO header structure; applications should use this size to
                                             // determine which BITMAPINFO header structure is being used, this size includes this biSize field
          aviFile.writeDword(params->size.w);// biWidth, width in pixels
          aviFile.writeDword(params->size.h);// biWidth, height in pixels (may be negative for uncompressed video to indicate vertical flip)
          aviFile.writeWord(1);              // biPlanes, number of color planes in which the data is stored
          aviFile.writeWord(24);             // biBitCount, number of bits per pixel #
          aviFile.writeWordStr("MJPG");      // biCompression, type of compression used (uncompressed: NO_COMPRESSION=0)
          aviFile.writeDword(params->size.w*params->size.h*3); // biSizeImage (buffer size for decompressed mage) may be 0 for uncompressed data
          aviFile.writeDword(0);             // biXPelsPerMeter, horizontal resolution in pixels per meter
          aviFile.writeDword(0);             // biYPelsPerMeter, vertical resolution in pixels per meter
          aviFile.writeDword(0);             // biClrUsed (color table size; for 8-bit only)
          aviFile.writeDword(0);             // biClrImportant, specifies that the first x colors of the color table (0: all colors are important, or,
                                             // rather, their relative importance has not been computed)
        finalizeLengthField("strf"); //'strf' chunk finished (nesting level 3)

        aviFile.writeWordStr("strn");        // Use 'strn' to provide a zero terminated text string describing the stream

        auto descString = String( descStr );
        auto fieldlen = descString.length(); // count chars, excluding the \0 terminator

        if( fieldlen%2 == 0 ) {              // Name must be 0-terminated and stream name length (the length of the chunk) must be even
          descString += " ";                 // append space if necessary
          fieldlen+=2;
        } else {
          fieldlen++;
        }
        aviFile.writeDword( fieldlen );      // Length of the strn sub-CHUNK, without the trailing \0 (must be even)
        aviFile.write( (const uint8_t*)descString.c_str(), fieldlen );
        if( aviFile.position()%2==1 ) aviFile.write((uint8_t)0x0); // terminating byte
      finalizeLengthField("strl");           // LIST 'strl' finished (nesting level 2)
    finalizeLengthField("hdrl");             // LIST 'hdrl' finished (nesting level 1)
    aviFile.writeWordStr("LIST");            // The second LIST chunk, which contains the actual data
    writeLengthField("movi");                // Chunk length (nesting level 1)
      moviPos = aviFile.position();
      aviFile.writeWordStr("movi");          // LIST chunk type: 'movi'
}


void AviMjpegEncoder::writeLengthField( const char* msg )
{
  auto pos = aviFile.position();
  lengthFields.push_back( pos );
  aviFile.writeDword(0);
  if( msg ) {
    log_v("%s.dwSize is at offset %d", msg, pos );
  }
}


void AviMjpegEncoder::finalizeLengthField( const char* msg )
{
  if( lengthFields.size() == 0 ) {
    log_e("Error, improper state");
    params->ready = false;
    return;
  }
  auto initial_pos = aviFile.position();
  auto seek_pos    = lengthFields[lengthFields.size()-1];
  lengthFields.pop_back();
  assert( initial_pos > seek_pos );
  uint32_t block_size = (initial_pos - seek_pos) - 4; // 4 = sizeof(avi_dword_t);
  aviFile.seek( seek_pos );
  aviFile.writeDword( block_size );
  aviFile.seek(initial_pos);
}


// add jpeg frame with known length
void AviMjpegEncoder::addJpegFrame(const uint8_t *jpegData, int len)
{
  writeJpegFrameHeader( len );
  writeJpegFrameData( jpegData, len );
  writeJpegFrameFinish();
}


// add AVI header for jpeg frame
void AviMjpegEncoder::writeJpegFrameHeader(int len)
{
  jpegFrameSize = 0;
  aviFile.writeWordStr("00dc");
  if( len == 0 ) {
    writeLengthField("00dc");
    needs_finalize = true;
  } else { // len provided, no need to populate later
    aviFile.writeDword(len);
    needs_finalize = false;
  }
  jpegFramePos = aviFile.position();
}


// add jpeg frame data chunk (repeated calls)
void AviMjpegEncoder::writeJpegFrameData(const uint8_t *jpegData, int len)
{
  jpegFrameSize += len;
  aviFile.write( jpegData, len );
}


// add AVI footer for jpeg frame
void AviMjpegEncoder::writeJpegFrameFinish()
{
  if( jpegFrameSize%2 ) { // jpeg length is odd
    aviFile.write((uint8_t)0x0); // even the length
    jpegFrameSize++;
  }
  if( needs_finalize ) {
    finalizeLengthField("00dc"); // "00dc" chunk finished (nesting level 2)
  }
  if( params->use_index_file ) { // append to idx file
    writeJpegIdxItem( &idxFile, { jpegFramePos, jpegFrameSize } );
    idx_items_count++;
  } else { // store in vector
    imgIndex.push_back( { jpegFramePos, jpegFrameSize } );
  }
  log_v("[%s] Added frame off=%d len=%d", params->use_index_file?"idx":"mem", jpegFramePos, jpegFrameSize );
}


void AviMjpegEncoder::writeJpegIdxItem( fs::AviFile* dst, avi_idx_item_t item )
{
  assert( dst );
  dst->writeWordStr("00dc");
  dst->writeDword(0x10);     // flags: select AVIIF_KEYFRAME
  dst->writeDword(item.pos); // offset to the chunk, offset can be relative to file start or 'movi'
  dst->writeDword(item.len); // length of the chunk
}



void AviMjpegEncoder::finalize()
{
  log_d("Finalizing");

  uint32_t framesCount = params->use_index_file ? idx_items_count : imgIndex.size();
  uint32_t idxLength = framesCount*16;
  uint8_t frameInfo[16];
  avi_idx_item_t *item;

  finalizeLengthField("movi"); // LIST 'movi' finished (nesting level 1)
  // Write index
  aviFile.writeWordStr("idx1"); // idx1 chunk
  aviFile.writeDword(idxLength); // Chunk length

  if( params->use_index_file ) {
    idxFile.close();
    idxFile = params->fs->open(tmpIdxPath); // reopen as readonly
    if( !idxFile ) {
      log_e("Unable to reopen %s index file for reading, aborting", tmpIdxPath);
      params->ready = false;
      return;
    }
  }

  for( int i=0;i<framesCount;i++ ) { // Write index data
    if( params->use_index_file ) {
      if( idxFile.readBytes( (char*)frameInfo, 16 ) != 16 ) {
        log_e("Unexpected EOF in index file at offset %d (idxLength=%d), aborting", i, idxLength );
        params->ready = false;
        return;
      }
      item = (avi_idx_item_t *)&frameInfo[8];
    } else {
      item = &imgIndex[i];
    }
    writeJpegIdxItem( &aviFile, { item->pos, item->len } );
  }

  if( params->use_index_file ) {
    idxFile.close();
    params->fs->remove(tmpIdxPath);
  }

  auto pos = aviFile.position();
  aviFile.seek(framesCountFieldPos1);
  aviFile.writeDword(framesCount);
  aviFile.seek(framesCountFieldPos2);
  aviFile.writeDword(framesCount);
  aviFile.seek(pos);
  finalizeLengthField("RIFF"); // 'RIFF' finished (nesting level 0)

  aviFile.close();

  imgIndex.clear();
  lengthFields.clear();
}
