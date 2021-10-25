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

  #if ! defined ESP32
    #error “This library only supports boards with ESP32 family processors”
  #endif

  // #define MPU9250_INSDE // M5Core2 : enable this only if plugging a secondary MPU !

  #include "gitTagVersion.h"
  #include <Arduino.h>
  #include <Wire.h>
  #include <SPI.h>
  #include <FS.h>
  #include "Config.h"

  #ifdef BOARD_HAS_1BIT_SDMMC
    #include "SD_MMC.h"
    #define M5STACK_SD SD_MMC
  #else
    #if defined HAS_SDCARD
      #include "SD.h"
      #define M5STACK_SD SD
      #define USE_TFCARD_CS_PIN
    #endif
  #endif

  #include "M5Display.h"

  #include "helpers/Memory.h"
  #include "helpers/ScreenShotService/ScreenShot.h" // ScreenShot Service
  #include "drivers/common/Button/Button.h"         // BtnA/BtnB/BtnC Support
  #include "drivers/common/I2C/I2CUtil.h"           // I2C Scanner
  #include "drivers/common/NVS/NVSUtils.h"          // NVS Utilities

  #if defined HAS_TOUCH
    #include "helpers/TouchButton.h"
  #endif

  #if defined HAS_SPEAKER
    #include "drivers/common/Speaker/Speaker.h"
  #endif

  #if defined HAS_IP5306
    #include "drivers/common/IP5306/Power.h"
  #endif

  #if defined HAS_AXP192
    #include "drivers/common/AXP192/AXP192.h"
  #endif

  #if defined HAS_AXP202
    #include "drivers/common/AXP202/axp20x.h"
  #endif

  #if defined HAS_BM8563
    #include "drivers/common/RTC_BM8563/RTC_BM8563.h"
  #endif

  #if defined HAS_PCF8563
    #include "drivers/common/RTC_PCF8563/pcf8563.h"
  #endif

  #if defined HAS_MPU6886
    #include "drivers/common/MPU6886/MPU6886.h"
  #endif

  #if defined HAS_MPU9250
    #include "drivers/common/MPU9250/MPU9250.h"
  #endif

  #if defined ARDUINO_ODROID_ESP32
    #include "drivers/Odroid-Go/Battery/battery.h"
  #endif

  #if defined ARDUINO_M5STACK_Core2  // M5Core2
    // M5Core2 'Physical' Button emulation using off-display touch zone
    #include "drivers/M5Core2/ButtonTouchReader/ButtonTouchReader.h"
    #undef DEBOUNCE_MS // unset base debounce
    #define DEBOUNCE_MS 100 // this device has emulated touch screen buttons and needs slower polls
  #endif

  #if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE) // m5stack classic/fire
    #if defined HAS_MPU9250
      #define HAS_PRIMARY_IMU
      #define PRIMARY_IMU MPU9250
      #define PRIMARY_IMU_ARG &I2C
    #endif
  #endif

  #if defined HAS_MPU6886
    #define HAS_PRIMARY_IMU
    #define PRIMARY_IMU MPU6886
    #define PRIMARY_IMU_ARG
    // allow secondary MPU
    #if defined HAS_MPU9250
      #warning "Enabling secondary IMU (MPU9250)"
      #define HAS_SECONDARY_IMU
      #define SECONDARY_IMU MPU9250
      #define SECONDARY_IMU_ARG &I2C
    #endif
  #endif


  class M5Stack
  {
    public:
      M5Stack();
      void begin(bool LCDEnable = true, bool SDEnable = SD_ENABLE, bool SerialEnable = true, bool I2CEnable = false, bool ScreenShotEnable = false);
      void update();

      bool sd_force_enable = SD_ENABLE;
      bool sd_begin(void);
      bool sd_begun = false;
      void sd_end(void);
      SPIClass *SD_SPI = nullptr;
      BaseType_t SD_CORE_ID = 1;

      M5Display Lcd; // LCD
      ScreenShotService ScreenShot; // ScreenShots !
      NVSUtils NVS; // NVS Utilities
      I2CUtil I2C = I2CUtil(); // I2C Scanner && Twatch I2C bus
      Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS);
      Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS);
      Button BtnC = Button(BUTTON_C_PIN, true, DEBOUNCE_MS);

      #if defined ARDUINO_ODROID_ESP32
        #define DEBOUNCE_MS_XY 5
        Button JOY_Y = Button(BUTTON_JOY_Y_PIN, true, DEBOUNCE_MS_XY);
        Button JOY_X = Button(BUTTON_JOY_X_PIN, true, DEBOUNCE_MS_XY);
        Button BtnMenu = Button(BUTTON_MENU_PIN, true, DEBOUNCE_MS);
        Button BtnVolume = Button(BUTTON_VOLUME_PIN, true, DEBOUNCE_MS);
        Button BtnSelect = Button(BUTTON_SELECT_PIN, true, DEBOUNCE_MS);
        Button BtnStart = Button(BUTTON_START_PIN, true, DEBOUNCE_MS);
        // Odroid-Go battery controller uses raw readings on adc1
        Battery battery;
      #endif

      #if defined HAS_BM8563
        RTC_BM8563 Rtc;
      #endif

      #if defined( ARDUINO_M5STACK_Core2 ) // M5Core2 C
        // Buttons emulation
        ButtonTouchReader *M5Core2TouchButtonEmu = nullptr;
      #endif

      #if defined HAS_PCF8563
        PCF8563_Class *Rtc  = nullptr;
      #endif
      #if defined HAS_AXP202
        AXP20X_Class *Axp = new AXP20X_Class();
      #endif

      #if defined HAS_RTC
        void setSystemTimeFromRtc();
        void setRtcTime( uint16_t year, uint8_t month, uint8_t day , uint8_t hours, uint8_t minutes, uint8_t seconds );
      #endif

      #if defined HAS_SPEAKER
        // Can make noise
        SPEAKER Speaker;
      #endif

      #if defined HAS_PRIMARY_IMU
        // primary accel/gyro from board config
        PRIMARY_IMU IMU = PRIMARY_IMU(PRIMARY_IMU_ARG);
      #else
        #if defined HAS_MPU9250
          // default primary accel/gyro
          MPU9250 IMU = MPU9250( &I2C );
        #endif
      #endif

      #if defined HAS_SECONDARY_IMU
        SECONDARY_IMU IMU2 = SECONDARY_IMU(SECONDARY_IMU_ARG);
      #endif

      #if defined HAS_AXP192
        //!Power
        AXP192 Axp = AXP192();
        void setPowerBoostKeepOn(bool en) __attribute__((deprecated)); // Function has been moved to Power class.(for compatibility) This name will be removed in a future release.
        void setWakeupButton(uint8_t button) __attribute__((deprecated)); // Function has been moved to Power class.(for compatibility) This name will be removed in a future release.
        void powerOFF() { Axp.PowerOff(); }
        void powerOff() { Axp.PowerOff(); }
      #endif

      #if defined HAS_IP5306 // legacy M5Stack/M5Fire support
        POWER Power;
        void setPowerBoostKeepOn(bool en) { Power.setPowerBoostKeepOn(en); } ;
        void setWakeupButton(uint8_t button) { Power.setWakeupButton(button); };
        void powerOFF() { Power.deepSleep(); }
        void powerOff() { powerOFF(); }
      #endif

      // UART
      // HardwareSerial Serial0 = HardwareSerial(0);
      // HardwareSerial Serial2 = HardwareSerial(2);

    private:
        bool isInited;
  };

  extern M5Stack M5;
  #define m5 M5
  #define lcd Lcd

#endif
