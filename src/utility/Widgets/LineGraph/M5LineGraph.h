/*\
 *

  MIT License

  M5Widgets utilities
  Copyright (c) 2018 Kongduino https://github.com/Kongduino/

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

 *
\*/
#pragma once

#include "../M5Widget.h"

class M5LineGraph : public M5Widget {
 public:
  M5LineGraph( lgfx::LGFX_Device* _gfx ) : M5Widget(_gfx), _pos(0), autoClearDisabled(false) {}
  void setLineCount(int nrLines);
  void setLineColor(unsigned char lineIdx, uint16_t color) { _lineColors[lineIdx]=color; }
  void setRange(float minimum, float maximum);
  uint16_t lineColor(unsigned char lineIdx) { return _lineColors[lineIdx]; }
  unsigned int position(void) { return _pos; }
  void addValues(const float* values);
  void addValue(float value);
  void setAutoClear(bool autoClear) { autoClearDisabled = !autoClear; }
  void clear(void);
  virtual void update(void);
  virtual void draw(void);
 private:
  unsigned char _dim;
  float _min, _max;
  int _pos;
  bool autoClearDisabled;
  DataBuffer _lastValues;
  uint16_t _lineColors[256];
};


