#include "M5Display.h"

void M5Display::progressBar(int x, int y, int w, int h, uint8_t val, uint16_t color) {
  drawRect(x, y, w, h, color);
  if( val>100) val = 100;
  fillRect(x + 1, y + 1, w * (((float)val) / 100.0), h - 1, color);
}
