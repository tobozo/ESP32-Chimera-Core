#include "M5ProgressBar.h"

void M5ProgressBar::setRange(float minimum, float maximum) {
  _minimum = minimum;
  _maximum = maximum;
  setValue(_valueReq);
  invalidate();
}

void M5ProgressBar::setValue(float value) {
  if (value < _minimum) {
    _valueReq = _minimum;
  } else if (value > _maximum) {
    _valueReq = _maximum;
  } else {
    _valueReq = value;
  }
}

void M5ProgressBar::update() {
  if (_value == _valueReq) return;
  _value = _valueReq;
  drawPointer();
}

void M5ProgressBar::draw() {
  if(!WidgetGfx) {
    log_e("No valid GFX");
    return;
  }
  WidgetGfx->fillRect(x + 1, y + 1, width - 2, height - 2, color(BACKGROUND));
  _value = _valueReq;
  drawPointer();
  WidgetGfx->drawRoundRect(x, y, width, height, 4, color(FRAME));
  WidgetGfx->drawRoundRect(x + 1, y + 1, width - 2, height - 2, 4, color(FRAME));
}

void M5ProgressBar::drawPointer() {
  if (_value == 0.0) return;
  uint16_t w = (_value / _maximum) * width;
  WidgetGfx->fillRect(x + 1, y + 1, w - 2, height - 2, color(CONTENT));
  char text[8];
  dtostrf(_value, 4, 2, text);
  WidgetGfx->setFont(this->myFont);
  uint16_t tw = WidgetGfx->textWidth(text);
  int16_t th = WidgetGfx->fontHeight();
  uint16_t myX = x + (w - tw) - 2;
  if (myX < x + 6) myX = x + 6;
  WidgetGfx->setTextColor(this->color(FOREGROUND));
  WidgetGfx->drawString(text, myX, 2 + y + ((this->height+2) - th) / 2);
}

