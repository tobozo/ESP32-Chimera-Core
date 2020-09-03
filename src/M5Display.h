#ifndef _M5DISPLAY_H_
#define _M5DISPLAY_H_

#define M5STACK

#include <SPIFFS.h>
#include <SD.h>

#include "utility/Config.h"
#if 0 // ifdef TOUCH_CS
  #include "utility/Touch_Drivers/XPT2046/XPT2046.h"
#endif

#define LGFX_AUTODETECT
#include <LGFX_TFT_eSPI.hpp>


//#ifdef TOUCH_CS
  //#include "utility/Touch.h"
  //#include "utility/TouchButton.h"
//#endif

namespace colors  // Color definitions for backwards compatibility with old sketches
{
  [[deprecated("use TFT_BLACK"      )]] static constexpr int BLACK       = 0x0000;      /*   0,   0,   0 */
  [[deprecated("use TFT_NAVY"       )]] static constexpr int NAVY        = 0x000F;      /*   0,   0, 128 */
  [[deprecated("use TFT_DARKGREEN"  )]] static constexpr int DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
  [[deprecated("use TFT_DARKCYAN"   )]] static constexpr int DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
  [[deprecated("use TFT_MAROON"     )]] static constexpr int MAROON      = 0x7800;      /* 128,   0,   0 */
  [[deprecated("use TFT_PURPLE"     )]] static constexpr int PURPLE      = 0x780F;      /* 128,   0, 128 */
  [[deprecated("use TFT_OLIVE"      )]] static constexpr int OLIVE       = 0x7BE0;      /* 128, 128,   0 */
  [[deprecated("use TFT_LIGHTGREY"  )]] static constexpr int LIGHTGREY   = 0xC618;      /* 192, 192, 192 */
  [[deprecated("use TFT_DARKGREY"   )]] static constexpr int DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
  [[deprecated("use TFT_BLUE"       )]] static constexpr int BLUE        = 0x001F;      /*   0,   0, 255 */
  [[deprecated("use TFT_GREEN"      )]] static constexpr int GREEN       = 0x07E0;      /*   0, 255,   0 */
  [[deprecated("use TFT_CYAN"       )]] static constexpr int CYAN        = 0x07FF;      /*   0, 255, 255 */
  [[deprecated("use TFT_RED"        )]] static constexpr int RED         = 0xF800;      /* 255,   0,   0 */
  [[deprecated("use TFT_MAGENTA"    )]] static constexpr int MAGENTA     = 0xF81F;      /* 255,   0, 255 */
  [[deprecated("use TFT_YELLOW"     )]] static constexpr int YELLOW      = 0xFFE0;      /* 255, 255,   0 */
  [[deprecated("use TFT_WHITE"      )]] static constexpr int WHITE       = 0xFFFF;      /* 255, 255, 255 */
  [[deprecated("use TFT_ORANGE"     )]] static constexpr int ORANGE      = 0xFD20;      /* 255, 165,   0 */
  [[deprecated("use TFT_GREENYELLOW")]] static constexpr int GREENYELLOW = 0xAFE5;      /* 173, 255,  47 */
  [[deprecated("use TFT_PINK"       )]] static constexpr int PINK        = 0xF81F;
}

using namespace colors;

class M5Display : public TFT_eSPI {
  public:
    template<typename T>
    void clearDisplay(T color=0) { fillScreen(color); }

    void display() {}

    void progressBar(int x, int y, int w, int h, uint8_t val);

    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data, uint16_t transparent);

#if 0 // ifdef TOUCH_CS
  // TOUCH

  public:
            // init XPT2046 based touchscreen
    void     initTouch( XPT2046_Touchscreen* ts ) { _ts = ts; }
            // Get raw x,y ADC values from touch controller
    uint8_t  getTouchRaw(uint16_t *x, uint16_t *y);
            // Get raw z (i.e. pressure) ADC value from touch controller
    uint16_t getTouchRawZ(void);
            // Convert raw x,y values to calibrated and correctly rotated screen coordinates
    void     convertRawXY(uint16_t *x, uint16_t *y);
            // Get the screen touch coordinates, returns true if screen has been touched
            // if the touch cordinates are off screen then x and y are not updated
    uint8_t  getTouch(uint16_t *x, uint16_t *y, uint16_t threshold = 600);

            // Run screen calibration and test, report calibration values to the serial port
    void     calibrateTouch(uint16_t *data, uint32_t color_fg, uint32_t color_bg, uint8_t size);
            // Set the screen calibration values
    void     setTouch(uint16_t *data);

  private:
    // TODO: also implement FT5206_Class
    XPT2046_Touchscreen* _ts = nullptr;

            // Private function to validate a touch, allow settle time and reduce spurious coordinates
    uint8_t  validTouch(uint16_t *x, uint16_t *y, uint16_t threshold = 600);

            // Initialise with example calibration values so processor does not crash if setTouch() not called in setup()
    uint16_t touchCalibration_x0 = 300, touchCalibration_x1 = 3600, touchCalibration_y0 = 300, touchCalibration_y1 = 3600;
    uint8_t  touchCalibration_rotate = 1, touchCalibration_invert_x = 2, touchCalibration_invert_y = 0;

    uint32_t _pressTime;        // Press and hold time-out
    uint16_t _pressX, _pressY;  // For future use (last sampled calibrated coordinates)
#endif


};

#endif
