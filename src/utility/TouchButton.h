#pragma once
#define _TOUCH_BUTTON_H

#include "../ESP32-Chimera-Core.h"

// LGFX has native TouchButton support since version 1.0.2
#if defined LGFX_VERSION_MAJOR && defined LGFX_VERSION_MINOR && defined LGFX_VERSION_PATCH
  #if ((100*LGFX_VERSION_MAJOR) + (10*LGFX_VERSION_MINOR) + LGFX_VERSION_PATCH) >=102
    #define LGFX_HAS_TOUCHBUTTON
  #endif
#endif

#if defined LGFX_HAS_TOUCHBUTTON

  // inherit from LovyanGFX button support
  #define TouchButton LGFX_Button

#else

  #warning Older version of LovyanGFX detected, please update !! Using legacy TouchButton instead.

  /***************************************************************************************
  // Inspired by LGFX TouchButton
  // Templated to allow drawing buttons to a sprite (not the best way though)
  ***************************************************************************************/

  class TouchButton {

   public:
    TouchButton(void);
    // "Classic" initButton() uses center & size
    void initButton(void *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    void initButton(LGFX *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    void initButton(LGFX_Sprite *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    // New/alt initButton() uses upper-left corner & size
    void initButtonUL(void *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    void initButtonUL(LGFX *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    void initButtonUL(LGFX_Sprite *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);
    // Adjust text datum and x, y deltas
    void setLabelDatum(int16_t x_delta, int16_t y_delta, textdatum_t datum = MC_DATUM);
    void drawButton(bool inverted = false, String long_name = "");

    template<typename T> // using TFT or Sprite
    void drawButton( T *gfx, bool inverted = false, String long_name = "") {
      if( _drawCb ) {
        _drawCb( gfx, _x1, _y1, _w, _h, inverted, long_name!="" ? long_name.c_str() : _label );
        return;
      }
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
      gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
      gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);
      gfx->setTextColor(text, fill);
      gfx->setTextSize(_textsize);
      textdatum_t tempdatum = gfx->getTextDatum();
      gfx->setTextDatum(_textdatum);
      //uint16_t tempPadding = _gfx->getTextPadding();
      //_gfx->setTextPadding(0);
      if (long_name == "")
        gfx->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);
      else
        gfx->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);
      gfx->setTextDatum(tempdatum);
      //_gfx->setTextPadding(tempPadding);
    }

    //void drawButton(String label, bool inverted, uint16_t margin=22);
    //void drawButton(bool inverted = false);
    bool contains(int16_t x, int16_t y);
    void press(bool p);
    bool isPressed();
    bool justPressed();
    bool justReleased();
    typedef void (*drawCb)( LovyanGFX *_gfx, int32_t x, int32_t y, int32_t w, int32_t h, bool invert, const char* long_name );
    void setDrawCb(drawCb cb) { _drawCb=cb; };

   private:
    bool isSprite = false;
    void *_gfx; // using TFT or Sprite
    drawCb _drawCb  = nullptr;
    int16_t  _x1, _y1; // Coordinates of top-left corner
    int16_t  _xd, _yd; // Button text datum offsets (wrt center of button)
    uint16_t _w, _h;
    uint8_t  _textsize;
    textdatum_t _textdatum; // Text size multiplier and text datum for button
    uint16_t _outlinecolor, _fillcolor, _textcolor;
    char     _label[12];

    bool  currstate, laststate;
  };

#endif // defined LGFX_HAS_TOUCHBUTTON

