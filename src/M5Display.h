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

  #include "lgfx/lgfx_base.hpp"           // base class (always include)

  #include "lgfx/lgfx_sprite.hpp"         // sprite class (optional)

  #include "lgfx/panel/panel_HX8357.hpp"
  #include "lgfx/panel/panel_ILI9163.hpp"
  #include "lgfx/panel/panel_ILI9341.hpp"   // and ILI9342 / M5Stack / ODROID-GO / ESP-WROVER-KIT4.1
  #include "lgfx/panel/panel_ILI9486.hpp"
  #include "lgfx/panel/panel_ILI9488.hpp"
  #include "lgfx/panel/panel_SSD1351.hpp"
  #include "lgfx/panel/panel_ST7789.hpp"    // LilyGO TTGO T-Watch
  #include "lgfx/panel/panel_ST7735.hpp"    // M5StickC

  #include "lgfx/platforms/lgfx_spi_esp32.hpp"

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

  using namespace textdatum;
  using namespace attribute;

  namespace lgfx {

  #if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE ) // M5Stack

    typedef Panel_M5Stack Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = 19;
      static constexpr int spi_sclk = 18;
    };

  #elif defined( ARDUINO_M5Stick_C ) // M5Stick C

    typedef Panel_M5StickC Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 15;
      static constexpr int spi_miso = 14;
      static constexpr int spi_sclk = 13;
    };

  #elif defined( ARDUINO_ODROID_ESP32 ) // ODROID-GO

    typedef Panel_ODROID_GO Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = 19;
      static constexpr int spi_sclk = 18;
    };

  #elif defined( ARDUINO_TTGO_T1 ) // TTGO TS

    typedef Panel_TTGO_TS Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = -1;
      static constexpr int spi_sclk =  5;
    };

  #elif defined( ARDUINO_T ) // TTGO T-Watch

    typedef Panel_TTGO_TWatch Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = HSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 19;
      static constexpr int spi_miso = -1;
      static constexpr int spi_sclk = 18;
    };

  #elif defined( ARDUINO_D ) || defined( ARDUINO_DDUINO32_XS )

    typedef Panel_DDUINO32_XS Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 26;
      static constexpr int spi_miso = -1;
      static constexpr int spi_sclk = 27;
    };

  #elif defined( ARDUINO_LOLIN_D32_PRO ) // LoLin D32 Pro

    typedef Panel_LoLinD32 Panel_default;
    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = 19;
      static constexpr int spi_sclk = 18;
    };

  #elif defined( ARDUINO_ESP32_DEV ) // ESP-WROVER-KIT

    // ESP-WROVER-KIT is available in two types of panels. (ILI9341 or ST7789)

    struct Panel_default : public lgfx::Panel_ILI9341 {
      Panel_default(void) {
        spi_3wire = false;
        spi_cs   = 22;
        spi_dc   = 21;
        gpio_rst = 18;
        gpio_bl  = 5;
        pwm_ch_bl = 7;
        freq_write = 40000000;
        freq_read  = 20000000;
        freq_fill  = 80000000;
        backlight_level = false;
      }
    };

    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = 25;
      static constexpr int spi_sclk = 19;
    };

  #elif defined( ESP32 ) || ( CONFIG_IDF_TARGET_ESP32 ) // ESP-IDF ( or other panel )

    typedef Panel_M5Stack Panel_default;

    struct LGFX_Config {
      static constexpr spi_host_device_t spi_host = VSPI_HOST;
      static constexpr int dma_channel = 1;
      static constexpr int spi_mosi = 23;
      static constexpr int spi_miso = 19;
      static constexpr int spi_sclk = 18;
    };

  #endif
  }

  class LGFX : public lgfx::LGFX_SPI<lgfx::LGFX_Config>
  {
  public:
    LGFX(void)
    {
      static lgfx::Panel_default panel;
      setPanel(&panel);
    }

  #if defined( ARDUINO_ESP32_DEV ) // ESP-WROVER-KIT
  // ESP-WROVER-KIT is available in two types of panels. (ILI9341 or ST7789)
    void initPanel(void) override {
      if (!_panel) return;
      _panel->init();

      if (readPanelID() > 0) {  // check panel (ILI9341 or ST7789)
        ESP_LOGI("M5Display", "[Autodetect] Using Panel_ST7789");

        static lgfx::Panel_ST7789 panel;
        panel.spi_3wire = false;
        panel.spi_cs   = 22;
        panel.spi_dc   = 21;
        panel.gpio_rst = 18;
        panel.gpio_bl  = 5;
        panel.pwm_ch_bl = 7;
        panel.freq_write = 80000000;
        panel.freq_read  = 16000000;
        panel.freq_fill  = 80000000;
        panel.backlight_level = false;
        panel.spi_mode_read = 1;
        panel.len_dummy_read_pixel = 16;

        setPanel(&panel);
      } else {
        ESP_LOGI("M5Display", "[Autodetect] Using Panel_ILI9341");
      }
      lgfx::LGFX_SPI<lgfx::LGFX_Config>::initPanel();
    }
  #endif

  };

  typedef LGFX TFT_eSPI;

  class TFT_eSprite : public lgfx::LGFX_Sprite {
  public:
    TFT_eSprite() : LGFX_Sprite() {}
    TFT_eSprite(LovyanGFX* parent) : LGFX_Sprite(parent) {}

    void* frameBuffer(uint8_t dummy) { return getBuffer(); }
  };


  __attribute__((unused)) static TFT_eSprite *jpegSprite = nullptr;

  class M5Display : public LGFX {
    public:
      M5Display();
  //      void begin();
  //      void sleep();
  //      void wakeup();
      void clearDisplay(uint32_t color=0) { fillScreen(color); }
      void display() {}

  //    void setBrightness(uint8_t brightness);
  //    void clear(uint32_t color=ILI9341_BLACK) { fillScreen(color); }
  //      inline void startWrite(void){
  //        #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  //          if (locked) {
  //            locked = false; SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
  //          }
  //        #endif
  //        CS_L;
  //      }
  //      inline void endWrite(void){
  //        #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  //          if(!inTransaction) {
  //            if (!locked) {
  //              locked = true;
  //              SPI.endTransaction();
  //            }
  //          }
  //        #endif
  //        CS_H;
  //      }
  //      inline void writePixel(uint16_t color) {
  //        SPI.write16(color);
  //      }
      inline void writePixels(uint16_t * colors, uint32_t len){
        SPI.writePixels((uint8_t*)colors , len * 2);
      }
      void progressBar(int x, int y, int w, int h, uint8_t val);

  //    void qrcode(const char *string, uint16_t x = 50, uint16_t y = 10, uint8_t width = 220, uint8_t version = 6);
  //    void qrcode(const String &string, uint16_t x = 50, uint16_t y = 10, uint8_t width = 220, uint8_t version = 6);

      void drawBmp(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
  //      void drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);

      void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data);
      void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t *data);
      void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t *data);
      void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint8_t *data);
      void drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data, uint16_t transparent);

  //      bool setupImgDecoder( int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0 );
  //
  //      void drawJpgFile( fs::FS &fs, const char *path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);
  //      void drawJpgFile( Stream *dataSource, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE );
  //      void drawJpg( const uint8_t *jpg_data, uint32_t jpg_len, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE  );
  //
  //      // PNG implementation by https://github.com/kikuchan
  //      void drawPng(const uint8_t *png_data, size_t png_len, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  //      void drawPngFile(fs::FS &fs, const char *path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  //      void drawPngFile(Stream &readSource, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  //      // deprecated, this is bloating M5Display.cpp
  //      __attribute__((deprecated)) void drawPngUrl(const char *url, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK );

    private:
  };
#endif
