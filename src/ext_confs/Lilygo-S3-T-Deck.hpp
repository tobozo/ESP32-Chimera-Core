#pragma once

#if defined ARDUINO_LILYGO_T_DECK

  // Lilygo S3 T-Deck has ST7789 display and GT911 touch
  // https://www.lilygo.cc/products/t-deck

  #include <LovyanGFX.hpp>
  #include <driver/i2c.h>

  class LGFX_S3TDeck : public lgfx::LGFX_Device {

    lgfx::Bus_SPI      _bus_instance;
    lgfx::Light_PWM    _light_instance;
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Touch_GT911  _touch_instance;

    public:

      LGFX_S3TDeck(void)
      {

        // Power on display (pin=TDECK_PERI_POWERON)
        lgfx::pinMode(GPIO_NUM_10, lgfx::pin_mode_t::output);
        lgfx::gpio_hi(GPIO_NUM_10 );

        // Wakeup touch chip (pin=TDECK_TOUCH_INT)
        lgfx::pinMode(GPIO_NUM_16, lgfx::pin_mode_t::output);
        lgfx::gpio_hi(GPIO_NUM_16 );
        delay(20);

        // Set touch int input (pin=TDECK_TOUCH_INT)
        lgfx::pinMode(GPIO_NUM_16, lgfx::pin_mode_t::input);
        delay(20);

        // tf card (pin=TFCARD_CS_PIN)
        lgfx::pinMode(GPIO_NUM_39, lgfx::pin_mode_t::output);
        lgfx::gpio_hi(GPIO_NUM_39 );

        // Wakeup LoRa (pin=TDECK_RADIO_CS)
        lgfx::pinMode(GPIO_NUM_9,  lgfx::pin_mode_t::output);
        lgfx::gpio_hi(GPIO_NUM_9 );

        {
          auto cfg = _bus_instance.config();
          cfg.spi_host   = SPI2_HOST;
          cfg.spi_3wire  = true;
          cfg.spi_mode   = 0;
          cfg.freq_write = 40000000;
          cfg.freq_read  = 16000000;
          cfg.pin_sclk   = GPIO_NUM_40;
          cfg.pin_mosi   = GPIO_NUM_41;
          cfg.pin_miso   = GPIO_NUM_38;
          cfg.pin_dc     = GPIO_NUM_11;
          _bus_instance.config(cfg);
          _panel_instance.setBus(&_bus_instance);
          log_d("Attached SPI bus instance");
        }

        {
          auto cfg = _light_instance.config();
          cfg.pin_bl      = GPIO_NUM_42;
          cfg.freq        = 12000;
          cfg.pwm_channel = 0;
          cfg.invert      = false;
          _light_instance.config(cfg);
          log_d("Attached Light instance");
        }

        {
          auto cfg = _panel_instance.config();
          cfg.panel_width  = 240;
          cfg.panel_height = 320;
          cfg.pin_cs       = GPIO_NUM_12;
          cfg.pin_rst      = -1;
          cfg.invert       = true; // default = false
          _panel_instance.config(cfg);
          _panel_instance.setRotation(1);
          _panel_instance.light( &_light_instance );
          log_d("Attached Panel instance");
        }

        {
          auto cfg = _touch_instance.config();
          cfg.x_min = 0;
          cfg.y_min = 0;
          cfg.x_max = 240;
          cfg.y_max = 320;
          cfg.bus_shared = true;
          // I2C
          cfg.i2c_port = I2C_NUM_1; // Wire1
          cfg.i2c_addr = 0x5D;
          cfg.pin_sda  = GPIO_NUM_18;
          cfg.pin_scl  = GPIO_NUM_8;
          cfg.pin_int  = GPIO_NUM_16;
          cfg.pin_rst  = -1;
          cfg.freq     = 800000;
          _touch_instance.config(cfg);
          _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
      }
  };

  #define LGFX LGFX_S3TDeck
  using M5Display = LGFX_S3TDeck;

#endif

