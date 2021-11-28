#include "M5Display.h"

void M5Display::progressBar(int x, int y, int w, int h, uint8_t val, uint16_t color, uint16_t bgcolor) {
  drawRect(x, y, w, h, color);
  if( val>100) val = 100;
  if( val==0 ) {
    fillRect(x + 1,         y + 1, w-2,       h - 2, bgcolor);
  } else {
    int fillw = (w * (((float)val) / 100.0)) -2;
    fillRect(x + 1,         y + 1, fillw-2,   h - 2, color);
    fillRect(x + fillw + 1, y + 1, w-fillw-2, h - 2, bgcolor);
  }
}
