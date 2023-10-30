/*\
 *
 * QoiEncoder
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
#pragma once
#define __FAT_PNG_ENCODER_H

#include "../ScreenShot.hpp"

struct qoienc_t
{
  void* device; // display or sprite
  int32_t x, y;
  uint32_t w, h;
};

class QOI_Encoder
{

  public:

    QOI_Encoder( void* src, bool is_sprite ) : _src(src), is_sprite(is_sprite) { };
    QOI_Encoder( LGFX *tft, fs::FS *fileSystem  ) : _src(tft), _fileSystem(fileSystem) { };
    QOI_Encoder( LGFX_Sprite *sprite, fs::FS *fileSystem  ) : _src(sprite), _fileSystem(fileSystem) { is_sprite = true; };
    void setFileSystem( fs::FS* fs ) { _fileSystem=fs; }
    size_t encode( Stream* stream, qoienc_t *qoi_info );
    size_t encodeToStream( Stream* stream, const int imageW, const int imageH );
    size_t encodeToStream( Stream* stream, const int imageX, const int imageY, const int imageW, const int imageH );
    size_t encodeToFile( const char* filename, const int imageW, const int imageH );
    size_t encodeToFile( const char* filename, const int imageX, const int imageY, const int imageW, const int imageH );
    static uint8_t* get_row_tft( uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc );
    static uint8_t* get_row_sprite( uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc );
    static int write_bytes(uint8_t* buf, size_t buf_len);
    static Stream* stream;

  private:

    void *_src; // display or sprite
    bool is_sprite = false;
    fs::FS * _fileSystem;

};

