#include "M5Gauge.h"

void M5Gauge::setRange(float minimum, float maximum) {
  _minimum = minimum;
  _maximum = maximum;
  setValue(_valueReq);
  invalidate();
}

void M5Gauge::setValue(float value) {
  if (value < _minimum) {
    _valueReq = _minimum;
  } else if (value > _maximum) {
    _valueReq = _maximum;
  } else {
    _valueReq = value;
  }
}

void M5Gauge::update() {
  if (_value == _valueReq) {
    return;
  }
  // Draw changes in the needle pointer (avoids having to draw the entire background)
  drawPointer(color(FOREGROUND));
  _value = _valueReq;
  drawPointer(color(CONTENT));
}

void M5Gauge::draw() {
  if(!WidgetGfx) {
    log_e("No valid GFX");
    return;
  }
  // Draw the gauge background circle
  int radius = width * 0.5;
  WidgetGfx->fillCircle(x + width / 2, y + width / 2, radius, color(FRAME));
  WidgetGfx->fillCircle(x + width / 2, y + width / 2, radius - 1, color(FOREGROUND));
  // Draw the pointer and background of value label field
  _value = _valueReq;
  drawPointer(color(CONTENT));
  // Draw the tick lines
  int tickCount = 11;
  for (int i = 0; i < tickCount; i++) {
    drawTickLine(radius - 2, radius - 8, (float) i /  (float) (tickCount - 1), color(CONTENT));
  }
  WidgetGfx->drawCircle(x + width / 2, y + width / 2, radius - 1, color(CONTENT));
}

void M5Gauge::drawLineAngle(int x, int y, int r1, int r2, float angle, uint16_t color) {
  float a_sin = sin(angle);
  float a_cos = cos(angle);
  WidgetGfx->drawLine(x + r1 * a_cos, y + r1 * a_sin, x + r2 * a_cos, y + r2 * a_sin, color);
}


void M5Gauge::drawPointer(uint16_t color, bool fillBg) {
  // Draw the line pointer
  float ang = (float) (_value - _minimum) / (float) (_maximum - _minimum);
  drawTickLine(0, 0.5 * width - 9, ang, color);
  // Draw the text
  int txt_h = 0.162 * width + (height - width);
  int txt_w = 0.73 * width + 2;
  int txt_x = x + (width - txt_w) / 2 + 1;
  int txt_y = y + height - txt_h + 1;
  if (fillBg) {
    WidgetGfx->fillRect(txt_x, txt_y, txt_w, txt_h, this->color(FOREGROUND));
    WidgetGfx->drawRect(txt_x, txt_y, txt_w, txt_h, this->color(FRAME));
    WidgetGfx->drawRect(txt_x + 1, txt_y, txt_w - 2, txt_h - 1, this->color(CONTENT));
  }
  char text[8];
  dtostrf(_value, 4, 2, text);
  WidgetGfx->setTextColor(this->color(FOREGROUND), this->color(BACKGROUND));
  drawStringMiddle(txt_x + 2, txt_y + 1, txt_w - 4, txt_h - 2, text, this->myFont, this->color(BACKGROUND), this->color(FOREGROUND));
}

void M5Gauge::drawTickLine(int r1, int r2, float ang, uint16_t color) {
  drawLineAngle(x + 0.5 * width, y + 0.5 * width, r1, r2, PI * (1.5 * ang - 1.25), color);
}
