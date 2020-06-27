#ifndef _M5DISPLAY_H_
#define _M5DISPLAY_H_

#define M5STACK

#include <SPIFFS.h>
#include <SD.h>

//#include "LovyanGFX/src/LGFX_TFT_eSPI.hpp"
#include <LGFX_TFT_eSPI.hpp>


#ifdef TOUCH_CS
  #include "utility/TouchButton.h"
#endif

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

    inline void writePixels(uint16_t * colors, uint32_t len){
      pushColors(colors, len, true);
    }

    void progressBar(int x, int y, int w, int h, uint8_t val);

    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint8_t *data);
    void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data, uint16_t transparent);
};
#endif
