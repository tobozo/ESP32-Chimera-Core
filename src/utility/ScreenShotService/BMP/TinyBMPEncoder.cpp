/*
 *
 * TinyBMPEncoder
 *
 * Copyright 2019 tobozo http://github.com/tobozo
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
 */
#include "TinyBMPEncoder.hpp"


bool BMP_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH ) {
  return encodeToFile( filename, 0, 0, imageW, imageH );
}

bool BMP_Encoder::encodeToFile( const char* filename, const int imageX, const int imageY, const int imageW, const int imageH ) {

  rgbBuffer = (RGBColor*)calloc( imageW+1, sizeof( RGBColor ) );
  fs::File outFile = _fileSystem->open( filename, "w" );  // <-----fs:: added for compatibility with SdFat ------

  if( ! outFile ) {
    log_e( "Can't write capture file %s, make sure the path exists!", filename );
    return false;
  }

  // file header
  unsigned char bmFlHdr[14] = {
    'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };
  // 54 = std total "old" Windows BMP file header size = 14 + 40
  int rowSize = ((3*imageW + 3) >> 2) << 2;      // how many bytes in the row (used to create padding)
  //unsigned long fileSize = 2ul * imageH * imageW + 54; // pix data + 54 byte hdr
  int fileSize = 54 + imageH*rowSize;        // headers (54 bytes) + pixel data

  bmFlHdr[ 2] = (unsigned char)(fileSize      ); // all ints stored little-endian
  bmFlHdr[ 3] = (unsigned char)(fileSize >>  8); // i.e., LSB first
  bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
  bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

  //info header
  unsigned char bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0
  };
  // 40 = info header size
  //  1 = num of color planes
  // 16 = bits per pixel
  // all other header info = 0, including RI_RGB (no compr), DPI resolution, except...
  bmInHdr[ 4] = (unsigned char)(       imageW      );
  bmInHdr[ 5] = (unsigned char)(       imageW >>  8);
  bmInHdr[ 6] = (unsigned char)(       imageW >> 16);
  bmInHdr[ 7] = (unsigned char)(       imageW >> 24);
  bmInHdr[ 8] = (unsigned char)(       imageH      );
  bmInHdr[ 9] = (unsigned char)(       imageH >>  8);
  bmInHdr[10] = (unsigned char)(       imageH >> 16);
  bmInHdr[11] = (unsigned char)(       imageH >> 24);

  outFile.write(bmFlHdr, sizeof(bmFlHdr));
  outFile.write(bmInHdr, sizeof(bmInHdr));

  uint8_t* buf = (uint8_t*)rgbBuffer;

  // set zero to padding area
  memset(&buf[rowSize - 4], 0, 4);
  for ( int i = imageH - 1; i >= 0; i-- ) {
    _tft->readRectRGB( imageX, imageY+i, imageW, 1, rgbBuffer ); // capture a whole line
    for ( int j = 0; j < imageW; j++ ) {
      // change color order and convert 18bit to 24bit ( 0xFC to 0xFF )
      auto b = rgbBuffer[j].b;
      rgbBuffer[j].b = rgbBuffer[j].r | rgbBuffer[j].r >> 6;
      rgbBuffer[j].g = rgbBuffer[j].g | rgbBuffer[j].g >> 6;
      rgbBuffer[j].r = b | b >> 6;
    }
    outFile.write((uint8_t*)rgbBuffer, rowSize);
  }
  outFile.close();
  free( rgbBuffer );
  return _fileSystem->exists( filename );
}
