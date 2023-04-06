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


struct quoienc_t
{
  LGFX* display;
  int x, y, w, h;
};


static Stream* _qoi_dest = nullptr;



uint8_t* QOI_Encoder::get_row( uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc )
{
  assert( qoienc );
  auto qoi_info = (quoienc_t*)qoienc;
  qoi_info->display->readRectRGB( qoi_info->x, qoi_info->y + y, w, h, lineBuffer );
  return lineBuffer;
}



int QOI_Encoder::write_bytes(uint8_t* buf, size_t buf_len)
{
  assert( _qoi_dest );
  return _qoi_dest->write( buf, buf_len );
}






void QOI_Encoder::init()
{
  // if( !psramInit() ) {
  //   log_n("[INFO] No PSRAM found, PNG Encoding won't work");
  // }
}


bool QOI_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH )
{
  return encodeToFile( filename, 0, 0, imageW, imageH );
}


bool QOI_Encoder::encodeToFile( const char* filename, const int imageX, const int imageY, const int imageW, const int imageH )
{
  quoienc_t qoi_info;
  qoi_info.display = _tft;
  qoi_info.x = imageX;
  qoi_info.y = imageY;
  qoi_info.w = imageW;
  qoi_info.h = imageH;

  fs::File destFile = _fileSystem->open( filename, "w");
  if( !destFile ) {
    log_e( "Can't write capture file %s, make sure the path exists!", filename );
    return false;
  }
  log_e( "Opened capture file %s for writing...", filename );
  _qoi_dest = (Stream*)&destFile;
  RGBColor lineBuffer[imageW+1];
  //uint8_t* lineBuffer = (uint8_t*)calloc(4096, sizeof(uint8_t));
  size_t written_bytes = lgfx_qoi_encoder_write_cb( &lineBuffer, 4096, imageW, imageH, 3, 0, get_row, write_bytes, &qoi_info );
  //free( lineBuffer );
  destFile.close();
  return written_bytes > 0;
}


