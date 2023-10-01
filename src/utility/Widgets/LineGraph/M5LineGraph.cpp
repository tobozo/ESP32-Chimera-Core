#include "M5LineGraph.h"

void M5LineGraph::setLineCount(int nrLines) {
  _lastValues.resize(nrLines * sizeof(unsigned int));
  _dim = nrLines;
}

void M5LineGraph::setRange(float minimum, float maximum) {
  _min = minimum;
  _max = maximum;
}

void M5LineGraph::clear() {
  _pos = 0;
  draw();
}

void M5LineGraph::addValue(float value) {
  float v[1];
  v[0] = value;
  addValues(v);
}

void M5LineGraph::addValues(const float* values) {
  // Clear screen as needed
  if (_pos >= width-2) {
    _pos = 0;
    if (!autoClearDisabled) clear();
  }
  // If not auto-clearing, wipe the next column first
  // Also clear the column when drawing for the first time
  if (autoClearDisabled || !isDrawn())
    WidgetGfx->drawFastVLine(x+_pos+1, y+1, height-2, this->color(BACKGROUND));
  // Draw the line
  for (unsigned char i = 0; i < _dim; i++) {
    uint16_t color = lineColor(i);
    unsigned int *y_old_ptr = (unsigned int*) _lastValues.data + i;
    unsigned int x_new = _pos + 1;
    unsigned int y_new;
    if (values[i] <= _min) y_new = 1;
    else if (values[i] >= _max) y_new = height-2;
    else y_new = 1 + (values[i] - _min) / (_max - _min) * (height-2);
    // Reverse the y-value
    y_new = height - y_new - 1;
    // Use same old and new value if first value of graph
    if (_pos == 0) *y_old_ptr = y_new;
    // Draw a line connecting old with new
    // If position is 0 or value did not change, draw only a dot.
    if (_pos && (*y_old_ptr != y_new)) {
      WidgetGfx->drawLine(x+x_new, y+*y_old_ptr, x+x_new, y+y_new, color);
    } else {
      WidgetGfx->drawPixel(x+x_new, y+y_new, color);
    }
    // Update old position
    *y_old_ptr = y_new;
  }
  // Next x-position
  _pos++;
}

void M5LineGraph::update() {
  draw();
}

void M5LineGraph::draw() {
  if(!WidgetGfx) {
    log_e("No valid GFX");
    return;
  }
  // Draw the frame
  WidgetGfx->drawRect(x, y, width, height, color(FRAME));
  // Fill the background by drawing from left to right
  // Do this starting at the current position to the end
  uint16_t bg = color(BACKGROUND);
  for (int dx = _pos; dx < (width - 2); dx++) {
    WidgetGfx->drawFastVLine(x + dx + 1, y + 1, height - 2, bg);
  }
}
