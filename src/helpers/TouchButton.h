/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this TFT_eSPI library allows the button
// legends to be in any font
***************************************************************************************/

//#include "In_eSPI.h"
//#include <Arduino.h>
#ifndef _TOUCH_BUTTON_H
#define _TOUCH_BUTTON_H

#include "../M5Display.h"

class TouchButton {

 public:
  TouchButton(void);
  // "Classic" initButton() uses center & size
  void     initButton(TFT_eSPI *gfx, int16_t x, int16_t y,
                      uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
                      uint16_t textcolor, char *label, uint8_t textsize);

  // New/alt initButton() uses upper-left corner & size
  void     initButtonUL(TFT_eSPI *gfx, int16_t x1, int16_t y1,
                        uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
                        uint16_t textcolor, char *label, uint8_t textsize);

  // Adjust text datum and x, y deltas
  void     setLabelDatum(int16_t x_delta, int16_t y_delta, textdatum_t datum = MC_DATUM);

  void drawButton(bool inverted = false, String long_name = "");

  //void drawButton(String label, bool inverted, uint16_t margin=22);
  //void drawButton(bool inverted = false);
  bool  contains(int16_t x, int16_t y);

  void     press(bool p);
  bool  isPressed();
  bool  justPressed();
  bool  justReleased();

 private:
  TFT_eSPI *_gfx;
  int16_t  _x1, _y1; // Coordinates of top-left corner
  int16_t  _xd, _yd; // Button text datum offsets (wrt center of button)
  uint16_t _w, _h;
  uint8_t  _textsize;
  textdatum_t _textdatum; // Text size multiplier and text datum for button
  uint16_t _outlinecolor, _fillcolor, _textcolor;
  char     _label[10];

  bool  currstate, laststate;
};

#endif
