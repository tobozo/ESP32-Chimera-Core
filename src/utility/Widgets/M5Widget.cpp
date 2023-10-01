#include "M5Widget.h"

M5Widget::M5Widget( lgfx::LGFX_Device* _gfx ) {
  assert( _gfx );
  this->x = 0;
  this->y = 0;
  this->width = 64;
  this->height = 64;
  this->invalidated = true;
  this->visible = 0x1;
  WidgetGfx = _gfx;
}

M5Widget::~M5Widget() {
  // Make sure to de-register this widget from the display
  undraw();
}

void M5Widget::setBounds(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  invalidate();
}

void M5Widget::setFont(const lgfx::GFXfont* font) {
  this->myFont = font;
  WidgetGfx->setFont(font);
}

void M5Widget::setSize(int width, int height) {
  this->width = width;
  this->height = height;
  invalidate();
}

void M5Widget::undraw() {
  WidgetGfx->fillRect(x, y, width+1, height+1, color(BACKGROUND));
}

void M5Widget::setVisible(bool visible) {
  if (isVisible() != visible) {
    if (visible) {
      this->visible |= 0x1;
    } else {
      this->visible &= ~0x1;
    }
    invalidate();
  }
}

void M5Widget::setDrawingEnabled(bool drawing) {
  if (drawing) {
    visible &= ~0x4;
  } else {
    visible |= 0x4;
  }
}

bool M5Widget::isDrawingEnabled() {
  return (visible & 0x4) != 0x4;
}

bool M5Widget::isVisible() {
  return (visible & 0x1) == 0x1;
}

bool M5Widget::isDrawn() {
  return (visible & 0x2) == 0x2;
}

void M5Widget::setColor(int colorId, uint16_t color) {
  myColors[colorId]=color;
  invalidate();
}

void M5Widget::invalidate() {
  invalidated = true;
}

bool M5Widget::isInvalidated() {
  return invalidated;
}

void M5Widget::draw_validate() {
  if ((visible & 0x4) != 0x4) {
    if (visible & 0x1) {
      visible |= 0x2;
      draw();
    } else if (visible & 0x2) {
      visible &= ~0x2;
      undraw();
    }
    invalidated = false;
  }
}

void drawStringMiddle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text, const lgfx::GFXfont* myFont, uint16_t bg, uint16_t fg) {
  WidgetGfx->setFont(myFont);
  WidgetGfx->setTextColor(bg, fg);
  int16_t tw = WidgetGfx->textWidth(text);
  int16_t th = WidgetGfx->fontHeight();
  WidgetGfx->drawString(text, x+(width - tw) / 2, y+(height-th)/2 - 6);
}

// fill a rectangle and draw a border
void M5Widget::fillBorderRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t borderColor) {
  WidgetGfx->drawRect(x, y, w, h, borderColor);
  WidgetGfx->fillRect(x+1, y+1, w-2, h-2, color);
}

// fill a rounded rectangle and draw a border
void M5Widget::fillBorderRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t radius, uint16_t color, uint16_t borderColor) {
  WidgetGfx->fillRoundRect(x, y, w, h, radius, color);
  WidgetGfx->drawRoundRect(x, y, w, h, radius, borderColor);
}

// draw a triangle!
void M5Widget::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  WidgetGfx->drawLine(x0, y0, x1, y1, color);
  WidgetGfx->drawLine(x1, y1, x2, y2, color);
  WidgetGfx->drawLine(x2, y2, x0, y0, color);
}

uint16_t calculateRGB565(uint8_t r, uint8_t g, uint8_t b) {
  return (((r & 248)<<8)+((g & 252)<<3)+((b & 248)>>3));
}


lgfx::LGFX_Device* WidgetGfx = nullptr;
