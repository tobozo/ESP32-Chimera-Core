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
#include "TinyBMPEncoder.h"


void BMP_Encoder::init( M5Display *tft, fs::FS &fileSystem  ) {
  _tft = tft;
  _fileSystem = &fileSystem;
}

bool BMP_Encoder::encodeToFile( const char* filename, const int imageW, const int imageH ) {
  byte colorByteH, colorByteL;

  bmpBuffer = (uint8_t*)calloc( imageW*3, sizeof( uint8_t ) );
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

  unsigned long fileSize = 2ul * imageH * imageW + 54; // pix data + 54 byte hdr

  bmFlHdr[ 2] = (unsigned char)(fileSize      ); // all ints stored little-endian
  bmFlHdr[ 3] = (unsigned char)(fileSize >>  8); // i.e., LSB first
  bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
  bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

  //info header
  unsigned char bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0
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

  for ( int i = imageH - 1; i >= 0; i-- ) {
    _tft->readRectRGB( 0, i, imageW, 1, bmpBuffer ); // capture a whole line
    for ( int j = 0; j < imageW; j++ ) {
      uint16_t rgb = _tft->color565( bmpBuffer[j*3], bmpBuffer[(j*3)+1], bmpBuffer[(j*3)+2] );
      colorByteH = ( rgb & 0xFF00 ) >> 8;
      colorByteL = rgb & 0x00FF;
      //RGB565 to RGB555 conversion... 555 is default for uncompressed BMP
      //this conversion is from ...Arduino forum topic=177361.0
      colorByteL = ( colorByteH << 7 ) | ( ( colorByteL & 0xC0 ) >> 1 ) | ( colorByteL & 0x1f );
      colorByteH = colorByteH >> 1;
      outFile.write( colorByteL );
      outFile.write( colorByteH );
      //vTaskDelay(1);
    }
  }
  outFile.close();
  free( bmpBuffer );
  return _fileSystem->exists( filename );
}
