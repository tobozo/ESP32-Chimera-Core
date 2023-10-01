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

class M5ProgressBar : public M5Widget {
 public:
  M5ProgressBar( lgfx::LGFX_Device* _gfx ) : M5Widget(_gfx) {}
  /// Sets the minimum and maximum value of the gauge
  void setRange(float minimum, float maximum);
  /// Sets the value displayed by the gauge
  void setValue(float value);
  /// Gets the value displayed by the gauge
  float value() const { return _valueReq; }

  virtual void update(void);
  virtual void draw(void);
 private:
  void drawPointer();
  float _minimum, _maximum, _value, _valueReq;
};

