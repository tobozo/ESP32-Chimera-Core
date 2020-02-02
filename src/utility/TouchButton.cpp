/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_GFX library and enhanced to handle any label font
***************************************************************************************/

#include "TouchButton.h"


TouchButton::TouchButton(void) {
  _gfx = 0;
}

// Classic initButton() function: pass center & size
void TouchButton::initButton(
 TFT_eSPI *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill,
    textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void TouchButton::initButtonUL(
 TFT_eSPI *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  _gfx          = gfx;
  strncpy(_label, label, 9);
}


void TouchButton::drawButton(String label, boolean inverted, uint16_t margin) {
  uint16_t fill, outline, text;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  _gfx->setTextColor(text);
  _gfx->setTextSize(_textsize);

  uint8_t tempdatum = _gfx->getTextDatum();
  _gfx->setTextDatum(ML_DATUM);
  if (label == "")
  _gfx->drawString(_label, _x1 + margin, _y1 + (_h/2));
  else
  _gfx->drawString(label, _x1 + margin, _y1 + (_h/2));
  _gfx->setTextDatum(tempdatum);
}

void TouchButton::drawButton(boolean inverted) {
  uint16_t fill, outline, text;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  _gfx->setTextColor(text);
  _gfx->setTextSize(_textsize);

  uint8_t tempdatum = _gfx->getTextDatum();
  _gfx->setTextDatum(MC_DATUM);
  _gfx->drawString(_label, _x1 + (_w/2), _y1 + (_h/2));
  _gfx->setTextDatum(tempdatum);
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
