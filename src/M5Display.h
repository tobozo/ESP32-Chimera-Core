#ifndef _M5DISPLAY_H_
#define _M5DISPLAY_H_

#define M5STACK

#include <SPIFFS.h>
#include <SD.h>

#include "lgfx/lgfx_common.hpp"         // common include (always include)

#include "lgfx/utility/lgfx_tjpgd.h"    // JPEG decode support (optional)
#include "lgfx/utility/lgfx_pngle.h"    // PNG decode support (optional)
#include "lgfx/utility/lgfx_qrcode.h"   // QR code support (optional)
#include "lgfx/lgfx_img_support.hpp"    // image format extention (optional)
#include "lgfx/lgfx_font_support.hpp"   // font extention (optional)

#include "lgfx/LGFXBase.hpp"           // base class (always include)

#include "lgfx/LGFX_Sprite.hpp"         // sprite class (optional)

#include "lgfx/panel/Panel_HX8357.hpp"
#include "lgfx/panel/Panel_ILI9163.hpp"
#include "lgfx/panel/Panel_ILI9341.hpp"   // and ILI9342 / M5Stack / ODROID-GO / ESP-WROVER-KIT4.1
#include "lgfx/panel/Panel_ILI9486.hpp"
#include "lgfx/panel/Panel_ILI9488.hpp"
#include "lgfx/panel/Panel_SSD1351.hpp"
#include "lgfx/panel/Panel_ST7789.hpp"    // LilyGO TTGO T-Watch
#include "lgfx/panel/Panel_ST7735.hpp"    // M5StickC

#include "lgfx/platforms/LGFX_SPI_ESP32.hpp"

//  #include "lgfx/utility/In_eSPI.h"
//  #include "utility/In_eSPI.h"
//  #include "utility/Sprite.h"
//  #include "utility/In_imgDecoder.h"
#ifdef TOUCH_CS
  #include "utility/TouchButton.h"
#endif

typedef lgfx::bgr888_t RGBColor;

namespace textdatum
{
  static constexpr textdatum_t TL_DATUM   = textdatum_t::top_left;
  static constexpr textdatum_t TC_DATUM   = textdatum_t::top_center;
  static constexpr textdatum_t TR_DATUM   = textdatum_t::top_right;
  static constexpr textdatum_t ML_DATUM   = textdatum_t::middle_left;
  static constexpr textdatum_t CL_DATUM   = textdatum_t::middle_left;
  static constexpr textdatum_t MC_DATUM   = textdatum_t::middle_center;
  static constexpr textdatum_t CC_DATUM   = textdatum_t::middle_center;
  static constexpr textdatum_t MR_DATUM   = textdatum_t::middle_right;
  static constexpr textdatum_t CR_DATUM   = textdatum_t::middle_right;
  static constexpr textdatum_t BL_DATUM   = textdatum_t::bottom_left;
  static constexpr textdatum_t BC_DATUM   = textdatum_t::bottom_center;
  static constexpr textdatum_t BR_DATUM   = textdatum_t::bottom_right;
  static constexpr textdatum_t L_BASELINE = textdatum_t::baseline_left;
  static constexpr textdatum_t C_BASELINE = textdatum_t::baseline_center;
  static constexpr textdatum_t R_BASELINE = textdatum_t::baseline_right;
};

namespace attribute
{
  static constexpr attribute_t CP437_SWITCH = attribute_t::cp437_switch;
  static constexpr attribute_t UTF8_SWITCH  = attribute_t::utf8_switch;
}

namespace colors  // Color definitions for backwards compatibility with old sketches
{
  [[deprecated("use TFT_BLACK"      )]] static constexpr int ILI9341_BLACK       = 0x0000;      /*   0,   0,   0 */
  [[deprecated("use TFT_NAVY"       )]] static constexpr int ILI9341_NAVY        = 0x000F;      /*   0,   0, 128 */
  [[deprecated("use TFT_DARKGREEN"  )]] static constexpr int ILI9341_DARKGREEN   = 0x03E0;      /*   0, 128,   0 */
  [[deprecated("use TFT_DARKCYAN"   )]] static constexpr int ILI9341_DARKCYAN    = 0x03EF;      /*   0, 128, 128 */
  [[deprecated("use TFT_MAROON"     )]] static constexpr int ILI9341_MAROON      = 0x7800;      /* 128,   0,   0 */
  [[deprecated("use TFT_PURPLE"     )]] static constexpr int ILI9341_PURPLE      = 0x780F;      /* 128,   0, 128 */
  [[deprecated("use TFT_OLIVE"      )]] static constexpr int ILI9341_OLIVE       = 0x7BE0;      /* 128, 128,   0 */
  [[deprecated("use TFT_LIGHTGREY"  )]] static constexpr int ILI9341_LIGHTGREY   = 0xC618;      /* 192, 192, 192 */
  [[deprecated("use TFT_DARKGREY"   )]] static constexpr int ILI9341_DARKGREY    = 0x7BEF;      /* 128, 128, 128 */
  [[deprecated("use TFT_BLUE"       )]] static constexpr int ILI9341_BLUE        = 0x001F;      /*   0,   0, 255 */
  [[deprecated("use TFT_GREEN"      )]] static constexpr int ILI9341_GREEN       = 0x07E0;      /*   0, 255,   0 */
  [[deprecated("use TFT_CYAN"       )]] static constexpr int ILI9341_CYAN        = 0x07FF;      /*   0, 255, 255 */
  [[deprecated("use TFT_RED"        )]] static constexpr int ILI9341_RED         = 0xF800;      /* 255,   0,   0 */
  [[deprecated("use TFT_MAGENTA"    )]] static constexpr int ILI9341_MAGENTA     = 0xF81F;      /* 255,   0, 255 */
  [[deprecated("use TFT_YELLOW"     )]] static constexpr int ILI9341_YELLOW      = 0xFFE0;      /* 255, 255,   0 */
  [[deprecated("use TFT_WHITE"      )]] static constexpr int ILI9341_WHITE       = 0xFFFF;      /* 255, 255, 255 */
  [[deprecated("use TFT_ORANGE"     )]] static constexpr int ILI9341_ORANGE      = 0xFD20;      /* 255, 165,   0 */
  [[deprecated("use TFT_GREENYELLOW")]] static constexpr int ILI9341_GREENYELLOW = 0xAFE5;      /* 173, 255,  47 */
  [[deprecated("use TFT_PINK"       )]] static constexpr int ILI9341_PINK        = 0xF81F;

  [[deprecated("use TFT_BLACK"      )]] static constexpr int BLACK               = 0x0000;      /*   0,   0,   0 */
  [[deprecated("use TFT_NAVY"       )]] static constexpr int NAVY                = 0x000F;      /*   0,   0, 128 */
  [[deprecated("use TFT_DARKGREEN"  )]] static constexpr int DARKGREEN           = 0x03E0;      /*   0, 128,   0 */
  [[deprecated("use TFT_DARKCYAN"   )]] static constexpr int DARKCYAN            = 0x03EF;      /*   0, 128, 128 */
  [[deprecated("use TFT_MAROON"     )]] static constexpr int MAROON              = 0x7800;      /* 128,   0,   0 */
  [[deprecated("use TFT_PURPLE"     )]] static constexpr int PURPLE              = 0x780F;      /* 128,   0, 128 */
  [[deprecated("use TFT_OLIVE"      )]] static constexpr int OLIVE               = 0x7BE0;      /* 128, 128,   0 */
  [[deprecated("use TFT_LIGHTGREY"  )]] static constexpr int LIGHTGREY           = 0xC618;      /* 192, 192, 192 */
  [[deprecated("use TFT_DARKGREY"   )]] static constexpr int DARKGREY            = 0x7BEF;      /* 128, 128, 128 */
  [[deprecated("use TFT_BLUE"       )]] static constexpr int BLUE                = 0x001F;      /*   0,   0, 255 */
  [[deprecated("use TFT_GREEN"      )]] static constexpr int GREEN               = 0x07E0;      /*   0, 255,   0 */
  [[deprecated("use TFT_CYAN"       )]] static constexpr int CYAN                = 0x07FF;      /*   0, 255, 255 */
  [[deprecated("use TFT_RED"        )]] static constexpr int RED                 = 0xF800;      /* 255,   0,   0 */
  [[deprecated("use TFT_MAGENTA"    )]] static constexpr int MAGENTA             = 0xF81F;      /* 255,   0, 255 */
  [[deprecated("use TFT_YELLOW"     )]] static constexpr int YELLOW              = 0xFFE0;      /* 255, 255,   0 */
  [[deprecated("use TFT_WHITE"      )]] static constexpr int WHITE               = 0xFFFF;      /* 255, 255, 255 */
  [[deprecated("use TFT_ORANGE"     )]] static constexpr int ORANGE              = 0xFD20;      /* 255, 165,   0 */
  [[deprecated("use TFT_GREENYELLOW")]] static constexpr int GREENYELLOW         = 0xAFE5;      /* 173, 255,  47 */
  [[deprecated("use TFT_PINK"       )]] static constexpr int PINK                = 0xF81F;
}

using namespace textdatum;
using namespace attribute;
using namespace colors;

#if defined( LGFX_M5STACK ) || defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE ) // M5Stack
  #include "config/LGFX_Config_M5Stack.hpp"

#elif defined( LGFX_M5STICKC ) || defined( ARDUINO_M5Stick_C ) // M5Stick C
  #include "config/LGFX_Config_M5StickC.hpp"

#elif defined( LGFX_ODROID_GO ) || defined( ARDUINO_ODROID_ESP32 ) // ODROID-GO
  #include "config/LGFX_Config_ODROID_GO.hpp"

#elif defined( LGFX_TTGO_TS ) || defined( ARDUINO_TTGO_T1 ) // TTGO TS
  #include "config/LGFX_Config_TTGO_TS.hpp"

#elif defined( LGFX_TTGO_TWATCH ) || defined( ARDUINO_T ) // TTGO T-Watch
  #include "config/LGFX_Config_TTGO_TWatch.hpp"

#elif defined( LGFX_DDUINO32_XS ) || defined( ARDUINO_D ) || defined( ARDUINO_DDUINO32_XS )
  #include "config/LGFX_Config_DDUINO32_XS.hpp"

#elif defined( LGFX_LOLIN_D32 ) || defined( ARDUINO_LOLIN_D32_PRO ) // LoLin D32 Pro
  #include "config/LGFX_Config_LoLinD32.hpp"

#elif defined( LGFX_ESP_WROVER_KIT ) || defined( ARDUINO_ESP32_DEV ) // ESP-WROVER-KIT
  #include "config/LGFX_Config_ESP_WROVER_KIT.hpp"

#else

  // If none of the above apply, Put a copy of "config/LGFX_Config_Custom" in libraries folder,
  // and modify the content according to the environment.
  // 上記のいずれにも該当しない場合、"config/LGFX_Config_Custom" のコピーをライブラリフォルダに配置し、
  // 動作環境に応じて内容を修正してください。
  #include "../LGFX_Config_Custom.hpp"

#endif

typedef LGFX TFT_eSPI;

class TFT_eSprite : public lgfx::LGFX_Sprite {
public:
  TFT_eSprite() : LGFX_Sprite() {}
  TFT_eSprite(LovyanGFX* parent) : LGFX_Sprite(parent) {}

  void* frameBuffer(uint8_t dummy) { return getBuffer(); }
};


//  __attribute__((unused)) static TFT_eSprite *jpegSprite = nullptr;

class M5Display : public LGFX {
  public:
    M5Display();

    void clearDisplay(uint32_t color=0) { fillScreen(color); }
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
