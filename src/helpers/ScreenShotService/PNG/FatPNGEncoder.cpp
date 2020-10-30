/*\
 *
 * FatPNGEncoder
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

#include "FatPNGEncoder.h"
#include "lgfx/utility/miniz.h"


void PNG_Encoder::init( M5Display *tft, fs::FS &fileSystem  ) {
  _tft = tft;
  _fileSystem = &fileSystem;
  if( !psramInit() ) {
    log_n("[INFO] No PSRAM found, PNG Encoding is disabled");
  }
}


bool PNG_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH ) {
  return encodeToFile( filename, 0, 0, imageW, imageH );
}






typedef uint8_t *(*png_encoder_get_row_func)(uint8_t *pImage, int flip, int w, int h, int y, int bpl, void *target);

static RGBColor *rgbBuffer = NULL;
static uint32_t png_encoder_xoffset = 0;
static uint32_t png_encoder_yoffset = 0;

static uint8_t *png_encoder_get_row( uint8_t *pImage, int flip, int w, int h, int y, int bpl, void *target ) {
  auto tft = (M5Display*)target;
  uint32_t ypos = (flip ? (h - 1 - y) : y);
  tft->readRectRGB( png_encoder_xoffset, png_encoder_yoffset+ypos, w, 1, pImage );
  log_n("[Free:%6d] Reading %d pixels line at [%d:%d]", ESP.getFreeHeap(), w, png_encoder_xoffset, png_encoder_yoffset+ypos );
  return (uint8_t*)pImage;
}


bool PNG_Encoder::encodeToFile( const char* filename, const int imageX, const int imageY, const int imageW, const int imageH ) {

  png_encoder_xoffset = imageX;
  png_encoder_yoffset = imageY;
  png_encoder_get_row_func get_row_func = &png_encoder_get_row;

  if( !psramInit() ) {
    log_n("[WARNING] No PSRAM found");
    //return false;
    // TODO: evaluate free ram before calling malloc
    rgbBuffer = (RGBColor*)malloc( (imageW*3)+1 );
  } else {
    rgbBuffer = (RGBColor*)ps_malloc( (imageW*3)+1 );
  }

  if (!rgbBuffer) {
    log_n( "[ERROR]  Can't malloc() image pixels, aborting" );
    return false;
  }

  size_t png_data_size = 0;
  uint32_t time_start = millis();
  bool success = false;

  //void *PNGDataPtr = tdefl_write_image_to_png_file_in_memory_ex(rgbBuffer, imageW, imageH, 3, &png_data_size, 6, 0);
  void *PNGDataPtr = tdefl_write_image_to_png_file_in_memory_ex_with_cb(rgbBuffer, imageW, imageH, 3, &png_data_size, 6, 0, (tdefl_get_png_row_func)get_row_func, _tft);

  if (!PNGDataPtr) {
    log_n("[ERROR] PNG Encoding failed, bad PSRAM ?\n");
  } else {
    fs::File outFile = _fileSystem->open(filename, "wb");  // <-----fs:: added for compatibility with SdFat ------
    if( ! outFile ) {
      log_n( "[ERROR] Can't write capture file %s, make sure the path exists!", filename );
      free(PNGDataPtr);
      free(rgbBuffer);
      return false;
    }
    outFile.write((uint8_t *)PNGDataPtr, png_data_size);
    outFile.close();
    //log_n("[SUCCESS] Wrote %d bytes into %s. Total time %u ms", png_data_size, filename, millis()-time_start);
  }

  free(PNGDataPtr);
  free(rgbBuffer);

  if( png_data_size > 0 )  {
    return _fileSystem->exists( filename );
  } else {
    return false;
  }

}
