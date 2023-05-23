#pragma once

#if defined ARDUINO_ESP32_S3_BOX

  // - S3BoxLite has no touch, and ST7789 display
  // - S3Box     has TT21xxx touch, and ILI9342 display

  // #define S3_BOX_LITE

  #include <LovyanGFX.hpp>
  #include <driver/i2c.h>

  class LGFX_S3Box : public lgfx::LGFX_Device {

    lgfx::Bus_SPI        _bus_instance;
    lgfx::Light_PWM      _light_instance;

    #if !defined S3_BOX_LITE
      #pragma message "S3Box support (panel=ILI9342,touch=TT21xxx)"
      lgfx::Panel_ILI9342  _panel_instance;
      lgfx::Touch_TT21xxx  _touch_instance;
      #define PANEL_INVERT false
      #define PANEL_OFFSET_ROTATION 1
    #else
      #pragma message "S3Box 'lite' support (panel=ST7789,no touch)"
      lgfx::Panel_ST7789   _panel_instance;
      #define PANEL_INVERT true
      #define PANEL_OFFSET_ROTATION 2
    #endif

    public:

      LGFX_S3Box(void)
      {

        {
          auto cfg = _bus_instance.config();
          cfg.spi_host = SPI2_HOST;
          cfg.spi_3wire  = true;
          cfg.spi_mode = 0;
          cfg.freq_write = 40000000;
          cfg.freq_read  = 16000000;
          cfg.pin_sclk = GPIO_NUM_7;
          cfg.pin_mosi = GPIO_NUM_6;
          cfg.pin_miso = GPIO_NUM_0;
          cfg.pin_dc   = GPIO_NUM_4;
          _bus_instance.config(cfg);
          _panel_instance.setBus(&_bus_instance);
          log_d("Attached SPI bus instance");
        }

        {
          auto cfg = _light_instance.config();
          cfg.pin_bl = GPIO_NUM_45;
          cfg.freq   = 12000;
          cfg.pwm_channel = 0;
          cfg.invert = false;
          _light_instance.config(cfg);
          log_d("Attached Light instance");
        }

        {
          auto cfg = _panel_instance.config();
          cfg.pin_cs   = GPIO_NUM_5;
          cfg.pin_rst  = GPIO_NUM_48;
          cfg.offset_rotation = PANEL_OFFSET_ROTATION;
          cfg.invert = PANEL_INVERT;
          _panel_instance.config(cfg);
          _panel_instance.setRotation(1);
          _panel_instance.light( &_light_instance );
          log_d("Attached Panel instance");
        }

        #if !defined S3_BOX_LITE
        {
          auto cfg = _touch_instance.config();
          cfg.pin_int    = GPIO_NUM_3;
          cfg.pin_sda    = GPIO_NUM_8;
          cfg.pin_scl    = GPIO_NUM_18;
          cfg.i2c_addr   = 0x24;
          cfg.i2c_port   = I2C_NUM_1;
          cfg.freq       = 400000;
          cfg.x_min      = 0;
          cfg.x_max      = 319;
          cfg.y_min      = 0;
          cfg.y_max      = 239;
          cfg.bus_shared = true;
          _touch_instance.config(cfg);
          _panel_instance.touch(&_touch_instance);
          float affine[6] = { 1, 0, 0, 0, -1, 240 };
          _panel_instance.setCalibrateAffine(affine);
          log_d("Attached Touch instance");
        }
        #endif

        setPanel(&_panel_instance);
      }
  };

  #define LGFX LGFX_S3Box
  using M5Display = LGFX_S3Box;

#endif
