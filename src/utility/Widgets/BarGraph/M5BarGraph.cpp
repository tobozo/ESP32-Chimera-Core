#include "M5BarGraph.h"

void M5BarGraph::setRange(float minimum, float maximum) {
  _minimum = minimum;
  _maximum = maximum;
  _baseVal = minimum;
  setValue(_valueReq);
  invalidate();
}

void M5BarGraph::setValue(float value) {
  if (value < _minimum) {
    _valueReq = _minimum;
  } else if (value > _maximum) {
    _valueReq = _maximum;
  } else {
    _valueReq = value;
  }
}

void M5BarGraph::setBaseValue(float baseValue) {
  _baseVal = baseValue;
  invalidate();
}

void M5BarGraph::update() {
  // If invalidated; ignore drawing updates
  if (invalidated) return;
  // If value flips around the base, make sure to clear properly
  if ((_valueReq > _baseVal) != (_value > _baseVal)) {
    drawBar(_value, _baseVal, color(FOREGROUND));
    _value = _baseVal;
  }

  // Draw changes in the bar graph; invalidate() causes flickering
  if (_valueReq != _value) {
    // Whether bar is increased or reduced depends on two factors
    if ((_valueReq >= _baseVal) != (_valueReq > _value)) {
      drawBar(_value, _valueReq, color(FOREGROUND));
    } else {
      drawBar(_value, _valueReq, color(CONTENT));
    }
  }
  _value = _valueReq;
}

void M5BarGraph::draw() {
  if(!WidgetGfx) {
    log_e("No valid GFX");
    return;
  }
  // Fill the background with a rectangle and a frame
  WidgetGfx->fillRect(x+1, y+1, width-2, height-2, color(FOREGROUND));
  // Draw the bar contents itself
  _value = _valueReq;
  drawBar(_baseVal, _value, color(CONTENT));
  WidgetGfx->drawRoundRect(x, y, width, height, 4, color(FRAME));
  WidgetGfx->drawRoundRect(x+1, y+1, width-2, height-2, 4, color(FRAME));
}

void M5BarGraph::drawBar(float val_a, float val_b, uint16_t color) {
  // Ensure val_a < val_b by swapping as needed
  if (val_a > val_b) {
    float f = val_b;
    val_b = val_a;
    val_a = f;
  }
  // Fill the area between val_a and val_b with a bar
  // Start by converting the input parameters into 0-1 space
  val_a = (val_a - _minimum) / (_maximum - _minimum);
  val_b = (val_b - _minimum) / (_maximum - _minimum);
  // Calculate the bar area
  int bar_x1, bar_x2, bar_y1, bar_y2;
  // Horizontal or vertical mode?
  if (width > height) {
    int bar_w = width - 2;
    int bar_h = height - 4;
    bar_y1 = 2;
    bar_y2 = bar_h + bar_y1;
    bar_x1 = 1 + val_a * bar_w;
    bar_x2 = 1 + val_b * bar_w;
  } else {
    int bar_w = width - 4;
    int bar_h = height - 2;
    bar_x1 = 2;
    bar_x2 = bar_w + bar_x1;
    bar_y1 = 1 + bar_h - val_b * bar_h;
    bar_y2 = 1 + bar_h - val_a * bar_h;
  }
  WidgetGfx->fillRect(x + bar_x1, y + bar_y1, bar_x2 - bar_x1, bar_y2 - bar_y1, color);
}
