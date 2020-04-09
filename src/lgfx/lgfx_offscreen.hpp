/*
MIT License

Copyright (c) 2020 lovyan03

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*----------------------------------------------------------------------------/
  Lovyan GFX library - ESP32 hardware SPI graphics library .  
  
    for Arduino and ESP-IDF  
  
Original Source  
 https://github.com/lovyan03/LovyanGFX/  

Licence  
 [MIT](https://github.com/lovyan03/LovyanGFX/blob/master/LICENSE)  

Author  
 [lovyan03](https://twitter.com/lovyan03)  
/----------------------------------------------------------------------------*/
#ifndef LGFX_OFFSCREEN_HPP_
#define LGFX_OFFSCREEN_HPP_

#include <algorithm>

#include "lgfx_base.hpp"

namespace lgfx
{
  class LGFX_Offscreen : public LovyanGFX
  {
  public:

    LGFX_Offscreen(LovyanGFX* parent)
    : LovyanGFX()
    , _parent(parent)
    , _buffer8(nullptr)
    , _xptr (0)
    , _yptr (0)
    , _xs   (0)
    , _xe   (0)
    , _ys   (0)
    , _ye   (0)
    , _index(0)
    {
      _write_conv.setColorDepth((parent == nullptr) ? color_depth_t::rgb565_2Byte : parent->getColorDepth());
      _read_conv = _write_conv;
      _has_transaction = false;
      _transaction_count = 0xFFFF;
    }

    uint8_t** buffer(void) { return _buffer8; }

    LGFX_Offscreen()
    : LGFX_Offscreen(nullptr)
    {}

    virtual ~LGFX_Offscreen() {
      free();
    }

    void deleteSprite(void) { free(); }
    void free(void)
    {
      if (_buffer8 != nullptr) {
        for (size_t i = 0; i <= _height; ++i) {
          if (_buffer8[i] != nullptr) {
            _mem_free(_buffer8[i]);
            _buffer8[i] = nullptr;
          }
        }
        _mem_free(_buffer8);
        _buffer8 = nullptr; 
      }
      _width = 0;
      _height = 0;
    }

    uint8_t** createSprite(int32_t w, int32_t h) { return create(w, h); }
    uint8_t** create(int32_t w = 0, int32_t h = 0)
    {
      if (_parent) {
        if (w == 0) w = _parent->width();
        if (h == 0) h = _parent->height();
      }
      if (w < 1 || h < 1) return nullptr;
      if (_buffer8) free();

      _buffer8 = (uint8_t**)_mem_alloc(sizeof(uint8_t*) * (h + 1));
      if (!_buffer8) return nullptr;

      int32_t len = w * _write_conv.bytes;
      for (int32_t i = 0; i < h; ++i) {
        _buffer8[i] = (uint8_t*)_mem_alloc(len);
        if (_buffer8[i] == nullptr) {
          free();
          return nullptr;
        }
        memset(_buffer8[i], 0, len);
      }
      _buffer8[h] = (uint8_t*)_mem_alloc((_write_conv.bits + 7) >> 3);

      _sw = _width = w;
      _clip_r = _xe = w - 1;
      _xpivot = w >> 1;

      _sh = _height = h;
      _clip_b = _ye = h - 1;
      _ypivot = h >> 1;

      _clip_l = _clip_t = _index = _sx = _sy = _xs = _ys = _xptr = _yptr = 0;

      return _buffer8;
    }


#if defined (ARDUINO)
 #if defined (FS_H)

    inline void createFromBmp(fs::FS &fs, const char *path) { createFromBmpFile(fs, path); }
    void createFromBmpFile(fs::FS &fs, const char *path) {
      FileWrapper file;
      file.setFS(fs);
      createFromBmpFile(&file, path);
    }

 #endif

#elif defined (CONFIG_IDF_TARGET_ESP32)  // ESP-IDF

    void createFromBmpFile(const char *path) {
      FileWrapper file;
      createFromBmpFile(&file, path);
    }

#endif

    void createFromBmp(const uint8_t *bmp_data, uint32_t bmp_len) {
      PointerWrapper data;
      data.set(bmp_data, bmp_len);
      create_from_bmp(&data);
    }

    void createFromBmpFile(FileWrapper* file, const char *path) {
      file->need_transaction = false;
      if (file->open(path, "rb")) {
        create_from_bmp(file);
        file->close();
      }
    }

    __attribute__ ((always_inline)) inline void* setColorDepth(uint8_t bpp) { return setColorDepth((color_depth_t)bpp); }
    void* setColorDepth(color_depth_t depth)
    {
      if (depth <= color_depth_t::rgb565_2Byte) depth = color_depth_t::rgb565_2Byte;

      _write_conv.setColorDepth(depth);
      _read_conv = _write_conv;

      if (_buffer8 == nullptr) return nullptr;
      free();
      return createSprite(_width, _height);
    }

    uint32_t readPixelValue(int32_t x, int32_t y)
    {
      if (_read_conv.bytes == 2) {
        return _buffer16[y][x];
      } else {
        return (uint32_t)(_buffer24[y][x]);
      }
    }

    template<typename T>
    __attribute__ ((always_inline)) inline void fillSprite (const T& color) { fillRect(0, 0, _width, _height, color); }
/*
    template<typename T>
    __attribute__ ((always_inline)) inline void pushSprite(                int32_t x, int32_t y, const T& transp) { push_sprite(_parent, x, y, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T>
    __attribute__ ((always_inline)) inline void pushSprite(LovyanGFX* dst, int32_t x, int32_t y, const T& transp) { push_sprite(    dst, x, y, _write_conv.convert(transp) & _write_conv.colormask); }
    __attribute__ ((always_inline)) inline void pushSprite(                int32_t x, int32_t y) { push_sprite(_parent, x, y); }
    __attribute__ ((always_inline)) inline void pushSprite(LovyanGFX* dst, int32_t x, int32_t y) { push_sprite(    dst, x, y); }

    template<typename T> bool pushRotated(                float angle, const T& transp) { return push_rotate_zoom(_parent, _parent->getPivotX(), _parent->getPivotY(), angle, 1.0f, 1.0f, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T> bool pushRotated(LovyanGFX* dst, float angle, const T& transp) { return push_rotate_zoom(dst    , dst    ->getPivotX(), dst    ->getPivotY(), angle, 1.0f, 1.0f, _write_conv.convert(transp) & _write_conv.colormask); }
                         bool pushRotated(                float angle) { return push_rotate_zoom(_parent, _parent->getPivotX(), _parent->getPivotY(), angle, 1.0f, 1.0f); }
                         bool pushRotated(LovyanGFX* dst, float angle) { return push_rotate_zoom(dst    , dst    ->getPivotX(), dst    ->getPivotY(), angle, 1.0f, 1.0f); }

    template<typename T> bool pushRotateZoom(                                              float angle, float zoom_x, float zoom_y, const T& transp) { return push_rotate_zoom(_parent, _parent->getPivotX(), _parent->getPivotY(), angle, zoom_x, zoom_y, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T> bool pushRotateZoom(LovyanGFX* dst                              , float angle, float zoom_x, float zoom_y, const T& transp) { return push_rotate_zoom(    dst, _parent->getPivotX(), _parent->getPivotY(), angle, zoom_x, zoom_y, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T> bool pushRotateZoom(                int32_t dst_x, int32_t dst_y, float angle, float zoom_x, float zoom_y, const T& transp) { return push_rotate_zoom(_parent,                dst_x,                dst_y, angle, zoom_x, zoom_y, _write_conv.convert(transp) & _write_conv.colormask); }
    template<typename T> bool pushRotateZoom(LovyanGFX* dst, int32_t dst_x, int32_t dst_y, float angle, float zoom_x, float zoom_y, const T& transp) { return push_rotate_zoom(    dst,                dst_x,                dst_y, angle, zoom_x, zoom_y, _write_conv.convert(transp) & _write_conv.colormask); }
                         bool pushRotateZoom(                                              float angle, float zoom_x, float zoom_y)                  { return push_rotate_zoom(_parent, _parent->getPivotX(), _parent->getPivotY(), angle, zoom_x, zoom_y); }
                         bool pushRotateZoom(LovyanGFX* dst                              , float angle, float zoom_x, float zoom_y)                  { return push_rotate_zoom(    dst, _parent->getPivotX(), _parent->getPivotY(), angle, zoom_x, zoom_y); }
                         bool pushRotateZoom(                int32_t dst_x, int32_t dst_y, float angle, float zoom_x, float zoom_y)                  { return push_rotate_zoom(_parent,                dst_x,                dst_y, angle, zoom_x, zoom_y); }
                         bool pushRotateZoom(LovyanGFX* dst, int32_t dst_x, int32_t dst_y, float angle, float zoom_x, float zoom_y)                  { return push_rotate_zoom(    dst,                dst_x,                dst_y, angle, zoom_x, zoom_y); }
//*/
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

  protected:
    LovyanGFX* _parent;
    union {
      uint8_t**  _buffer8;
      uint16_t** _buffer16;
      bgr888_t** _buffer24;
    };
    int32_t _bitwidth;
    int32_t _xptr;
    int32_t _yptr;
    int32_t _xs;
    int32_t _xe;
    int32_t _ys;
    int32_t _ye;
    int32_t _index;
    bool _use_spiram = false; // disable PSRAM to PSRAM memcpy flg.

    bool create_from_bmp(DataWrapper* data) {
      bitmap_header_t bmpdata;

      if (!load_bmp_header(data, &bmpdata)
       || ( bmpdata.biCompression != 0
         && bmpdata.biCompression != 1
         && bmpdata.biCompression != 3)) {
        return false;
      }
      uint32_t seekOffset = bmpdata.bfOffBits;
      uint_fast16_t bpp = bmpdata.biBitCount; // 24 bcBitCount 24=RGB24bit
      if (bpp < 16 || 24 < bpp) { return false; }
      setColorDepth(bpp);
      int32_t w = bmpdata.biWidth;
      int32_t h = bmpdata.biHeight;  // bcHeight Image height (pixels)
      if (!createSprite(w, h)) return false;

        //If the value of Height is positive, the image data is from bottom to top
        //If the value of Height is negative, the image data is from top to bottom.
      int32_t flow = (h < 0) ? 1 : -1;
      int32_t y = 0;
      if (h < 0) h = -h;
      else y = h - 1;

      data->seek(seekOffset);

      uint8_t lineBuffer[((w * bpp + 31) >> 5) << 2];  // readline 4Byte align.
      if (bpp == 16) {
        do {
          data->read(lineBuffer, sizeof(lineBuffer));
          auto img = _buffer8[y];
          for (size_t i = 0; i < sizeof(lineBuffer); ++i) {
            img[i] = lineBuffer[i ^ 1]; // convert endian
          }
          y += flow;
        } while (--h);
      } else if (bpp == 24) {
        do {
          data->read(lineBuffer, sizeof(lineBuffer));
          auto img = _buffer8[y];
          for (size_t i = 0; i < sizeof(lineBuffer); i += 3) {
            img[i    ] = lineBuffer[i + 2]; // convert endian
            img[i + 1] = lineBuffer[i + 1];
            img[i + 2] = lineBuffer[i    ];
          }
          y += flow;
        } while (--h);
      }
      return true;
    }
/*
    void push_sprite(LovyanGFX* dst, int32_t x, int32_t y, uint32_t transp = ~0)
    {
      pixelcopy_t p(_buffer8, dst->getColorDepth(), getColorDepth(), dst->hasPalette(), _palette, transp);
      p.no_convert = false;
//    dst->push_image(x, y, _width, _height, &p, !_use_spiram); // DMA disable with use SPIRAM
      dst->push_image(x, y, _width, _height, &p, false); // DMA disable
    }

    bool push_rotate_zoom(LovyanGFX* dst, int32_t x, int32_t y, float angle, float zoom_x, float zoom_y, uint32_t transp = ~0)
    {
      if (_buffer8 == nullptr) return false;
      pixelcopy_t p(_buffer8, dst->getColorDepth(), getColorDepth(), dst->hasPalette(), _palette, transp);
      dst->pushImageRotateZoom(x, y, _width, _height, _xpivot, _ypivot, angle, zoom_x, zoom_y, &p);
      return true;
    }
//*/
    void set_window(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
    {
      if (xs > xe) std::swap(xs, xe);
      if (ys > ye) std::swap(ys, ye);
      if ((xe < 0) || (ye < 0) || (xs >= _width) || (ys >= _height))
      {
        _xptr = _xs = _xe = 0;
        _yptr = _ys = _ye = _height;
      } else {
        _xptr = _xs = (xs < 0) ? 0 : xs;
        _yptr = _ys = (ys < 0) ? 0 : ys;
        _xe = std::min(xe, _width  - 1);
        _ye = std::min(ye, _height - 1);
      }
      _index = xs + ys * _bitwidth;
    }

    void setWindow_impl(int32_t xs, int32_t ys, int32_t xe, int32_t ye) override
    {
      set_window(xs, ys, xe, ye);
    }

    void drawPixel_impl(int32_t x, int32_t y) override
    {
      if (_write_conv.bytes == 2) {
        _buffer16[y][x] = _color.rawL;
      } else {
        _buffer24[y][x] = *(bgr888_t*)&_color;
      }
    }

    void writeFillRect_impl(int32_t x, int32_t y, int32_t w, int32_t h) override
    {
/* // for debug pushBlock_impl
setWindow(x,y,x+w-1,y+h-1);
pushBlock_impl(w*h);
return;
//*/
      uint32_t bytes = _write_conv.bytes;
      h += y;
      if (w == 1) {
        if (bytes == 2) {
          uint16_t c = _color.rawL;
          auto buf = _buffer16;
          do { buf[y][x] = c; } while (++y != h);
        } else {  // if (_write_conv.bytes == 3)
          auto c = _color;
          auto buf = _buffer24;
          do { buf[y][x] = *(bgr888_t*)&c; } while (++y != h);
//        auto img = &_img24[index];
//        do { img->r = c.raw0; img->g = c.raw1; img->b = c.raw2; img += bw; } while (--h);
        }
      } else {
        x *= bytes;
        auto buf = _buffer8;
        uint8_t c = _color.raw0;
        if (c == _color.raw1 && (bytes == 2 || (c == _color.raw2))) {
          w *= bytes;
          do {
            memset(&buf[y][x], c, w);
          } while (++y != h);
        } else {
          auto src = &buf[y][x];
          memset_multi(src, _color.raw, bytes, w);
          w *= bytes;
          while (++y != h) {
            memcpy(&buf[y][x], src, w);
          }
        }
      }
    }

    void pushBlock_impl(int32_t length) override
    {
      if (0 >= length) return;
      uint32_t bytes = _write_conv.bytes;
      if (bytes == 1 || (_color.raw0 == _color.raw1 && (bytes == 2 || (_color.raw0 == _color.raw2)))) {
        uint32_t color = _color.raw;
        int32_t ll;
        do {
          ll = std::min(_xe - _xptr + 1, length);
          memset(&_buffer8[_yptr][_xptr * bytes], color, ll * bytes);
          ptr_advance(ll);
        } while (length -= ll);
      } else
      if (_use_spiram) {
        int32_t buflen = std::min(_xe - _xs + 1, length);
        uint8_t linebuf[buflen * bytes];
        memset_multi(linebuf, _color.raw, bytes, buflen);
        int32_t ll;
        do  {
          ll = std::min(_xe - _xptr + 1, length);
          memcpy(&_buffer8[_yptr][_xptr * bytes], linebuf, ll * bytes);
          ptr_advance(ll);
        } while (length -= ll);
        return;
      } else {
        uint32_t color = _color.raw;
        int32_t ll;
        do {
          ll = std::min(_xe - _xptr + 1, length);
          memset_multi(&_buffer8[_yptr][_xptr * bytes], color, bytes, ll);
          ptr_advance(ll);
        } while (length -= ll);
      }
    }

    void copyRect_impl(int32_t dst_x, int32_t dst_y, int32_t w, int32_t h, int32_t src_x, int32_t src_y) override
    {
/*
      if (_write_conv.bits < 8) {
        pixelcopy_t param(_img, _write_conv.depth, _write_conv.depth);
        param.src_width = _bitwidth;
        int32_t add_y = (src_y < dst_y) ? -1 : 1;
        if (src_y != dst_y) {
          if (src_y < dst_y) {
            src_y += h - 1;
            dst_y += h - 1;
          }
          param.src_y = src_y;
          do {
            param.src_x = src_x;
            auto idx = dst_x + dst_y * _bitwidth;
            param.fp_copy(_img, idx, idx + w, &param);
            dst_y += add_y;
            param.src_y += add_y;
          } while (--h);
        } else {
          size_t len = (_bitwidth * _write_conv.bits) >> 3;
          uint8_t buf[len];
          param.src_data = buf;
          param.src_y32 = 0;
          do {
            memcpy(buf, &_img[src_y * len], len);
            param.src_x = src_x;
            auto idx = dst_x + dst_y * _bitwidth;
            param.fp_copy(_img, idx, idx + w, &param);
            dst_y += add_y;
            src_y += add_y;
          } while (--h);
        }
      } else {
        size_t len = w * _write_conv.bytes;
        int32_t add = _bitwidth * _write_conv.bytes;
        if (src_y < dst_y) add = -add;
        int32_t pos = (src_y < dst_y) ? h - 1 : 0;
        uint8_t* src = &_img[(src_x + (src_y + pos) * _bitwidth) * _write_conv.bytes];
        uint8_t* dst = &_img[(dst_x + (dst_y + pos) * _bitwidth) * _write_conv.bytes];
        if (_use_spiram) {
          uint8_t buf[len];
          do {
            memcpy(buf, src, len);
            memcpy(dst, buf, len);
            dst += add;
            src += add;
          } while (--h);
        } else {
          do {
            memmove(dst, src, len);
            src += add;
            dst += add;
          } while (--h);
        }
      }
//*/
    }

    void readRect_impl(int32_t x, int32_t y, int32_t w, int32_t h, void* dst, pixelcopy_t* param) override
    {
      h += y;
      if (param->no_convert && _read_conv.bytes) {
        auto b = _read_conv.bytes;
        w *= b;
        x *= b;
        auto d = (uint8_t*)dst;
        do {
          memcpy(d, &_buffer8[y][x], w);
          d += w;
        } while (++y != h);
      } else {
        int32_t dstindex = 0;
        do {
          param->src_data = _buffer8[y];
          param->src_x = x;
          dstindex = param->fp_copy(dst, dstindex, dstindex + w, param);
        } while (++y != h);
      }
    }

    void pushImage_impl(int32_t x, int32_t y, int32_t w, int32_t h, pixelcopy_t* param, bool use_dma) override
    {
      auto sx = param->src_x;
      h += y;
      do {
        int32_t pos = x;
        int32_t end = x + w;
        while (end != (pos = param->fp_copy(_buffer8[y], pos, end, param))) {
          if ( end == (pos = param->fp_skip(             pos, end, param))) break;
        }
        param->src_x = sx;
        param->src_y++;
      } while (++y != h);
    }

    void pushColors_impl(int32_t length, pixelcopy_t* param) override
    {
      int32_t linelength;
      do {
        linelength = std::min(_xe - _xptr + 1, length);
        param->fp_copy(_buffer8[_yptr], _xptr, _xptr + linelength, param);
        ptr_advance(linelength);
      } while (length -= linelength);
    }

    void beginTransaction_impl(void) override {}
    void endTransaction_impl(void) override {}
    void waitDMA_impl(void) override {}

    inline int32_t ptr_advance(int32_t length = 1) {
      if ((_xptr += length) > _xe) {
        _xptr = _xs;
        if (++_yptr > _ye) {
          _yptr = _ys;
        }
        return (_index = _xptr + _yptr * _bitwidth);
      } else {
        return (_index += length);
      }
    }



//----------------------------------------------------------------------------
#if defined (ESP32) || (CONFIG_IDF_TARGET_ESP32)
//----------------------------------------------------------------------------

  protected:
//    uint32_t _malloc_cap = MALLOC_CAP_8BIT;
    void* _mem_alloc(uint32_t bytes)
    {
      void* res = heap_caps_malloc(bytes, MALLOC_CAP_DMA);
      return res;
    }
    void _mem_free(void* buf)
    {
      heap_caps_free(buf);
    }

//----------------------------------------------------------------------------
#elif defined (ESP8226)
//----------------------------------------------------------------------------
// not implemented.
//----------------------------------------------------------------------------
#elif defined (STM32F7)
//----------------------------------------------------------------------------
// not implemented.
//----------------------------------------------------------------------------
#elif defined (__AVR__)
//----------------------------------------------------------------------------
// not implemented.
//----------------------------------------------------------------------------

#endif

  };

}

#endif
