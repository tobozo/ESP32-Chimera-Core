/*\
 *
 * QOIEncoder
 *
 * Copyright 2020 tobozo http://github.com/tobozo
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files ("TinyBMPEncoder"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
\*/


#include "QOIEncoder.hpp"

#if __has_include(<lgfx/utility/lgfx_qoi.h>)
  #include <lgfx/utility/lgfx_qoi.h>
#else
  #error "lgfx_qoi.h not found, please update to the latest version of LovyanGFX"
#endif


Stream* QOI_Encoder::stream = nullptr;


uint8_t* QOI_Encoder::get_row_sprite( uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc )
{
  assert( qoienc );
  auto qoi_info = (qoienc_t*)qoienc;
  ((LGFX_Sprite*)(qoi_info->device))->readRectRGB( qoi_info->x, qoi_info->y + y, w, h, lineBuffer );
  return lineBuffer;
}


uint8_t* QOI_Encoder::get_row_tft( uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc )
{
  assert( qoienc );
  auto qoi_info = (qoienc_t*)qoienc;
  ((LGFX*)(qoi_info->device))->readRectRGB( qoi_info->x, qoi_info->y + y, w, h, lineBuffer );
  return lineBuffer;
}



int QOI_Encoder::write_bytes(uint8_t* buf, size_t buf_len)
{
  assert( QOI_Encoder::stream );
  return QOI_Encoder::stream->write( buf, buf_len );
}


size_t QOI_Encoder::encode( Stream* stream, qoienc_t *qoi_info )
{
  assert(stream);
  assert(qoi_info);
  QOI_Encoder::stream = stream;
  RGBColor lineBuffer[qoi_info->w+1];
  return lgfx_qoi_encoder_write_cb( &lineBuffer, 4096, qoi_info->w, qoi_info->h, 3, 0, is_sprite?get_row_sprite:get_row_tft, write_bytes, qoi_info );
}


size_t QOI_Encoder::encodeToStream( Stream* stream, const int w, const int h )
{
  return encodeToStream( stream, 0, 0, w, h );
}


size_t QOI_Encoder::encodeToStream( Stream* stream, const int x, const int y, const int w, const int h )
{
  assert( stream );
  qoienc_t qoi_info;
  qoi_info.device = _src;
  qoi_info.x = x;
  qoi_info.y = y;
  qoi_info.w = w;
  qoi_info.h = h;
  return encode( stream, &qoi_info );
}


size_t QOI_Encoder::encodeToFile( const char* filename, const int w, const int h )
{
  return encodeToFile( filename, 0, 0, w, h );
}


size_t QOI_Encoder::encodeToFile( const char* filename, const int x, const int y, const int w, const int h )
{
  assert( filename );
  assert( _fileSystem );
  qoienc_t qoi_info;
  qoi_info.device = _src;
  qoi_info.x = x;
  qoi_info.y = y;
  qoi_info.w = w;
  qoi_info.h = h;
  fs::File destFile = _fileSystem->open( filename, "w");
  if( !destFile ) {
    log_e( "Can't write capture file %s, make sure the path exists!", filename );
    return 0;
  }
  log_v( "Opened capture file %s for writing...", filename );
  size_t written_bytes = encode( &destFile, &qoi_info );
  destFile.close();
  return written_bytes;
}


