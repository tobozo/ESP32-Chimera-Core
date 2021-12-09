#include "TouchButton.h"

#if !defined LGFX_HAS_TOUCHBUTTON


TouchButton::TouchButton(void) {
  _gfx       = 0;
  _xd        = 0;
  _yd        = 0;
  _textdatum = MC_DATUM;
}

void TouchButton::initButton( TFT_eSPI *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  isSprite = false;
  initButton( (void*)gfx, x, y, w, h, outline, fill, textcolor, label, textsize);
}

void TouchButton::initButton( TFT_eSprite *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  isSprite = true;
  initButton( (void*)gfx, x, y, w, h, outline, fill, textcolor, label, textsize);
}


void TouchButton::initButtonUL( TFT_eSPI *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  isSprite = false;
  initButton( (void*)gfx, x, y, w, h, outline, fill, textcolor, label, textsize);
}

void TouchButton::initButtonUL( TFT_eSprite *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  isSprite = true;
  initButton( (void*)gfx, x, y, w, h, outline, fill, textcolor, label, textsize);
}


// Classic initButton() function: pass center & size
void TouchButton::initButton( void *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void TouchButton::initButtonUL( void *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  //isSprite = false;
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  _gfx          = gfx;
  strncpy(_label, label, 11);
}


// Adjust text datum and x, y deltas
void TouchButton::setLabelDatum(int16_t x_delta, int16_t y_delta, textdatum_t datum)
{
  _xd        = x_delta;
  _yd        = y_delta;
  _textdatum = datum;
}


void TouchButton::drawButton(bool inverted, String long_name) {
  if( isSprite ) {
    auto gfx = reinterpret_cast<TFT_eSprite*>(_gfx);
    drawButton( gfx, inverted, long_name );
  } else {
    auto gfx = reinterpret_cast<TFT_eSPI*>(_gfx);
    drawButton( gfx, inverted, long_name );
  }
}


boolean TouchButton::contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void TouchButton::press(boolean p) {
  laststate = currstate;
  currstate = p;
}

boolean TouchButton::isPressed()    { return currstate; }
boolean TouchButton::justPressed()  { return (currstate && !laststate); }
boolean TouchButton::justReleased() { return (!currstate && laststate); }

#endif
