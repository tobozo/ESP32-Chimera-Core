// Copyright (c) M5Stack. All rights reserved.

// Licensed under the MIT license. See LICENSE file in the project root for full license information.
/**
 * \par Copyright (C), 2016-2017, M5Stack
 * \class M5Stack
 * \brief   A clone of M5Stack library with multiple devices support
 * @file    ESP32-Chimera-Core
 * @author  M5Stack, tobozo
 * @version V1.1.0
 * @date    2020/09/19
 * @brief   Header for ESP32-Chimera-Core.cpp module
 *
 * \par Description
 * This file is a drive for M5Stack/M5Core2/Odroid-Go/TWatch (and more) core.
 *
 * \par Method List:
 *
 *  System:
        M5.begin();
        M5.update();

    Power:
        M5.Power.setPowerBoostKeepOn()
        M5.Power.setCharge(uint8_t mode);
        M5.Power.setPowerBoostKeepOn(bool en);
        M5.Power.isChargeFull();
        M5.Power.setWakeupButton(uint8_t button);
        M5.Power.powerOFF();

        bool setPowerBoostOnOff(bool en);
        bool setPowerBoostSet(bool en);
        bool setPowerVin(bool en);
        bool setPowerWLEDSet(bool en);

    LCD:
        M5.lcd.setBrightness(uint8_t brightness);
        M5.Lcd.drawPixel(int16_t x, int16_t y, uint16_t color);
        M5.Lcd.drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
        M5.Lcd.fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        M5.Lcd.fillScreen(uint16_t color);
        M5.Lcd.drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        M5.Lcd.drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color);
        M5.Lcd.fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        M5.Lcd.fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta, uint16_t color);
        M5.Lcd.drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        M5.Lcd.fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        M5.Lcd.drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
        M5.Lcd.fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
        M5.Lcd.drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
        M5.Lcd.drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h),
        M5.Lcd.drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
        M5.Lcd.setCursor(uint16_t x0, uint16_t y0);
        M5.Lcd.setTextColor(uint16_t color);
        M5.Lcd.setTextColor(uint16_t color, uint16_t backgroundcolor);
        M5.Lcd.setTextSize(uint8_t size);
        M5.Lcd.setTextWrap(boolean w);
        M5.Lcd.printf();
        M5.Lcd.print();
        M5.Lcd.println();
        M5.Lcd.drawCentreString(const char *string, int dX, int poY, int font);
        M5.Lcd.drawRightString(const char *string, int dX, int poY, int font);
        M5.Lcd.drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x, uint16_t y);
        M5.Lcd.drawJpgFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
        M5.Lcd.drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);

    Button:
        M5.BtnA/B/C.read();
        M5.BtnA/B/C.isPressed();
        M5.BtnA/B/C.isReleased();
        M5.BtnA/B/C.wasPressed();
        M5.BtnA/B/C.wasReleased();
        M5.BtnA/B/C.wasreleasedFor()
        M5.BtnA/B/C.pressedFor(uint32_t ms);
        M5.BtnA/B/C.releasedFor(uint32_t ms);
        M5.BtnA/B/C.lastChange();

    Speaker:
        M5.Speaker.tone(uint32_t freq);
        M5.Speaker.tone(freq, time);
        M5.Speaker.beep();
        M5.Speaker.setBeep(uint16_t frequency, uint16_t duration);
        M5.Speaker.mute();

 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Zibin Zheng         2017/07/14        0.0.1          Rebuild the new.
 * Bin                 2018/10/29        0.2.4          Add Button API
 * </pre>
 *
 */

#ifndef _CHIMERA_CORE_H_
  #define _CHIMERA_CORE_H_
  // create marker to ease core (legacy or chimera) detection
  // e.g.:
  // #ifdef _CHIMERA_CORE_
  //    M5.ScreenShot.snap();
  // #endif
  #define _CHIMERA_CORE_

  // Swap any type, this was removed by LGFX but still needed
  template <typename T> static inline void
  swap_coord(T& a, T& b) { T t = a; a = b; b = t; }

  #if defined(ESP32)

    //#define MPU9250_INSDE // M5Core2 : enable this only if plugging a secondary MPU !
    #include "gitTagVersion.h"
    #include <Arduino.h>
    #include <Wire.h>
    #include <SPI.h>
    #include <FS.h>

    #if defined ( ARDUINO_ESP32_DEV ) || defined( ARDUINO_DDUINO32_XS ) || defined( ARDUINO_ESP32_WROVER_KIT )
      #include "SD_MMC.h"
      #define M5STACK_SD SD_MMC
    #else
      #include "SD.h"
      #define M5STACK_SD SD
      #define USE_TFCARD_CS_PIN
    #endif

    #include "M5Display.h"
    #include "Config.h"

    #include "helpers/TouchButton.h"
    #include "helpers/ScreenShot.h"

    #include "drivers/common/Button/Button.h"
    #include "drivers/common/Speaker/Speaker.h"
    #include "drivers/common/IP5306/Power.h"
    #include "drivers/common/I2C/CommUtil.h"
    #include "drivers/common/NVS/NVSUtils.h"

    // allow multiple MPU
    #if defined( MPU9250_INSDE )
      #include "drivers/M5Stack/MPU9250/MPU9250.h"
    #endif
    #if defined( ARDUINO_M5STACK_Core2 ) // M5Core2
      #include "drivers/M5Core2/MPU6886/MPU6886_M5Core2.h"
    #endif

    #ifdef ARDUINO_ODROID_ESP32
      #include "drivers/Odroid-Go/Battery/battery.h"
    #endif

    #if defined(ARDUINO_M5Stick_C) // M5Stick C
      #include "drivers/M5StickC/AXP192.h"
      #include "drivers/M5StickC/RTC.h"
    #elif defined( ARDUINO_M5STACK_Core2 ) // M5Core2
      #include "drivers/M5Core2/FT6336U/Touch_M5Core2.h"
      #include "drivers/M5Core2/AXP192/AXP192_M5Core2.h"
      #include "drivers/M5Core2/BM8563/RTC_M5Core2.h"
    #elif defined( ARDUINO_T_Watch )
      #if defined( LILYGO_WATCH_HAS_PCF8563 )
        #include "drivers/TWatch/rtc/pcf8563.h"
      #endif
      #if defined HAS_TOUCH
        // TODO: implement TWatch Touch
      #endif
    #endif

    #ifndef HAS_TOUCH
      // dummy Touch object for detection
      #include "drivers/common/DummyTouch/DummyTouch.h"
    #endif

    class M5Stack
    {
      public:
        M5Stack();
        void begin(bool LCDEnable = true, bool SDEnable = SD_ENABLE, bool SerialEnable = true, bool I2CEnable = false, bool ScreenShotEnable = false);
        void update();

        bool sd_force_enable = SD_ENABLE;
        bool sd_begin(void);
        void sd_end(void);

        //void setTouchSpiShared( int32_t csPin, int32_t IRQPin=255 );

        #if defined( ARDUINO_M5STACK_Core2 )// M5Core2 C
          #define DEBOUNCE_MS 1
        #else
          #define DEBOUNCE_MS 10
        #endif
        // Button API
        Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS);
        Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS);
        Button BtnC = Button(BUTTON_C_PIN, true, DEBOUNCE_MS);

        #ifdef ARDUINO_ODROID_ESP32
        #define DEBOUNCE_MS_XY 5
          Button JOY_Y = Button(BUTTON_JOY_Y_PIN, true, DEBOUNCE_MS_XY);
          Button JOY_X = Button(BUTTON_JOY_X_PIN, true, DEBOUNCE_MS_XY);
          Button BtnMenu = Button(BUTTON_MENU_PIN, true, DEBOUNCE_MS);
          Button BtnVolume = Button(BUTTON_VOLUME_PIN, true, DEBOUNCE_MS);
          Button BtnSelect = Button(BUTTON_SELECT_PIN, true, DEBOUNCE_MS);
          Button BtnStart = Button(BUTTON_START_PIN, true, DEBOUNCE_MS);

          Battery battery;
        #endif

        // SPEAKER
        SPEAKER Speaker;
        // LCD
        M5Display Lcd;
        // ScreenShots !
        ScreenShotService ScreenShot;

        NVSUtils NVS;

        #ifdef TOUCH_CS
          // TODO: deprecate this
          //XPT2046_Touchscreen* ts = nullptr;
        #endif

        #if defined(ARDUINO_M5Stick_C) // M5Stick C
          //!Power
          AXP192 Axp = AXP192();
          //!RTC
          RTC Rtc;
        #elif defined( ARDUINO_M5STACK_Core2 )// M5Core2 C
          //!Power
          AXP192_M5Core2 Axp = AXP192_M5Core2();
          //!RTC
          RTC_M5Core2 Rtc;
          Touch_M5Core2 Touch;
          // accel/gyro
          MPU6886_M5Core2 IMU = MPU6886_M5Core2();
          // allow secondary MPU
          #ifdef MPU9250_INSDE
            MPU9250 IMU2 = MPU9250();
          #endif

        #elif defined( ARDUINO_T_Watch )
          #if defined( LILYGO_WATCH_HAS_PCF8563 )
            PCF8563_Class *Rtc  = nullptr;
          #endif

        #else
          #define HAS_POWER
          POWER Power;
          // MPU9250
          #ifdef MPU9250_INSDE
            MPU9250 IMU = MPU9250();
          #endif
        #endif


        #ifndef HAS_TOUCH
          touch Touch; // create dummy touch object
        #endif

        // UART
        // HardwareSerial Serial0 = HardwareSerial(0);
        // HardwareSerial Serial2 = HardwareSerial(2);

        // I2C
        CommUtil I2C = CommUtil();

        #if defined HAS_POWER // legacy M5Stack support
          /**
          * Function has been move to Power class.(for compatibility)
          * This name will be removed in a future release.
          */
          void setPowerBoostKeepOn(bool en);
          void setWakeupButton(uint8_t button);
          void powerOFF();

        #else // M5Core2
          /**
          * Function has been move to Power class.(for compatibility)
          * This name will be removed in a future release.
          */
          void setPowerBoostKeepOn(bool en) __attribute__((deprecated));
          void setWakeupButton(uint8_t button) __attribute__((deprecated));
          void powerOFF() __attribute__((deprecated));
        #endif
      private:
          bool isInited;
    };

    extern M5Stack M5;
    #define m5 M5
    #define lcd Lcd
  #else
    #error “This library only supports boards with ESP32 processor.”
  #endif
#endif
