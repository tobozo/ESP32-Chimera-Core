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


#include "FatPNGEncoder.hpp"

#if __has_include(<utility/miniz.h>)
  #include <utility/miniz.h>
#elif __has_include(<lgfx/utility/miniz.h>)
  #include <lgfx/utility/miniz.h>
#else
  #error "miniz.h not found, please update to the latest version of LovyanGFX"
#endif

void PNG_Encoder::init()
{
  if( !psramInit() ) {
    log_n("[INFO] No PSRAM found, PNG Encoding won't work");
  }
}


bool PNG_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH )
{
  return encodeToFile( filename, 0, 0, imageW, imageH );
}


bool PNG_Encoder::encodeToFile( const char* filename, const int imageX, const int imageY, const int imageW, const int imageH )
{

  size_t png_data_size = 0;
  //uint32_t time_start = millis();

  #ifndef LOVYANGFX_VERSION
    uint8_t* PNGDataPtr = NULL;
    #warning "LGFX version must be >= 0.2.5 to use createPng, please upgrade LovyanGFX"
    return false;
  #else
    uint8_t* PNGDataPtr = (uint8_t*)_tft->createPng( &png_data_size, imageX, imageY, imageW, imageH );
  #endif

  if (!PNGDataPtr) {
    log_n("[ERROR] PNG Encoding failed, bad PSRAM ?\n");
  } else {
    fs::File outFile = _fileSystem->open(filename, "wb");  // <-----fs:: added for compatibility with SdFat ------
    if( ! outFile ) {
      log_n( "[ERROR] Can't write capture file %s, make sure the path exists!", filename );
      free(PNGDataPtr);
      return false;
    }
    outFile.write((uint8_t *)PNGDataPtr, png_data_size);
    outFile.close();
    //log_n("[SUCCESS] Wrote %d bytes into %s. Total time %u ms", png_data_size, filename, millis()-time_start);
  }

  free(PNGDataPtr);

  if( png_data_size > 0 )  {
    return _fileSystem->exists( filename );
  } else {
    return false;
  }

}

