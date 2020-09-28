#include "M5Display.h"

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data, uint16_t transparent) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data, (uint32_t)transparent);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, (const uint16_t*)data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint8_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, (uint16_t*)data);
  setSwapBytes(swap);
}

void M5Display::progressBar(int x, int y, int w, int h, uint8_t val) {
  drawRect(x, y, w, h, 0x09F1);
  fillRect(x + 1, y + 1, w * (((float)val) / 100.0), h - 1, 0x09F1);
}
