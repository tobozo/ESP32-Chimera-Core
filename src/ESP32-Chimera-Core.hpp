#pragma once

#define _CHIMERA_CORE_H_
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
  #include <SD_MMC.h>
  #define M5STACK_SD SD_MMC
#else
  #if defined HAS_SDCARD
    #include <SD.h>
    #define M5STACK_SD SD
    #define USE_TFCARD_CS_PIN
  #endif
#endif

#include "M5Display.h"

// ChimeraCore utilities
#if defined HAS_SDCARD && defined USE_SCREENSHOTS
  #include "utility/Memory.h"
  #include "utility/ScreenShotService/ScreenShot.h" // ScreenShot Service
#endif
#include "drivers/common/I2C/I2CUtil.h"           // I2C Scanner


#if defined USE_NVSUTILS
  #include "drivers/common/NVS/NVSUtils.h"          // NVS Utilities
#endif

// Hardware misc drivers support

#include "drivers/common/Button/Button.h"         // BtnA/BtnB/BtnC Support

#if defined HAS_SPEAKER
  #include "drivers/common/Speaker/Speaker.h"
  //#include "drivers/common/Audio/Speaker_Class.hpp"
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

#if defined HAS_TOUCH
  // hardware support
  #include "utility/Touch/Touch_Class.hpp" // Touch Support with gestures from M5Unified
  #include "utility/Touch/TouchButton.hpp" // alias to LGFX_Button, used as UI Draw helpers for Touch Buttons
#endif

#if defined ARDUINO_ODROID_ESP32
  // TODO: move this to generic AnalogSensor class
  #include "drivers/Odroid-Go/Battery/battery.h"
#endif

#if defined ARDUINO_ESP32_S3_BOX
  // TODO: use https://github.com/pschatzmann/arduino-audiokit
  #include "drivers/ESP32-S3-Box/Audio/esp-adf.hpp"
#endif

// primary and/or secondary MPU support

#if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE) // m5stack classic/fire
  #if defined HAS_MPU6886
    #define HAS_PRIMARY_IMU
    #define PRIMARY_IMU MPU6886
    #define PRIMARY_IMU_ARG &Wire
    #if defined HAS_MPU9250
      #pragma message "Enabling secondary IMU (MPU9250)"
      #define HAS_SECONDARY_IMU
      #define SECONDARY_IMU MPU9250
      #define SECONDARY_IMU_ARG &I2C
    #endif
  #elif defined HAS_MPU9250
    #define HAS_PRIMARY_IMU
    #define PRIMARY_IMU MPU9250
    #define PRIMARY_IMU_ARG &I2C
  #endif
#endif

#if defined HAS_MPU6886
  #if !defined HAS_PRIMARY_IMU
    #define HAS_PRIMARY_IMU
    #define PRIMARY_IMU MPU6886
    #define PRIMARY_IMU_ARG &I2C
  #endif
  // allow secondary MPU
  #if defined HAS_MPU9250
    #if !defined HAS_SECONDARY_IMU
      #pragma message "Enabling secondary IMU (MPU9250)"
      #define HAS_SECONDARY_IMU
      #define SECONDARY_IMU MPU9250
      #define SECONDARY_IMU_ARG &I2C
    #endif
  #endif
#endif


namespace ChimeraCore
{

  class ECCKernel
  {
    public:
      ECCKernel();
      void begin(bool LCDEnable = true, bool SDEnable = SD_ENABLE, bool SerialEnable = true, bool I2CEnable = false, bool ScreenShotEnable = false);
      void update();

      bool sd_force_enable = SD_ENABLE;
      bool sd_begin(void);
      bool sd_begun = false;
      void sd_end(void);
      #if defined ( TFCARD_USE_WIRE1 )
        SPIClass *SD_SPI = nullptr;
      #endif
      BaseType_t SD_CORE_ID = 1;

      M5Display Lcd; // LCD

      #if defined HAS_SDCARD && defined USE_SCREENSHOTS
        ScreenShotService *ScreenShot; // ScreenShots !
      #endif

      #if defined USE_NVSUTILS
        NVSUtils NVS; // NVS Utilities
      #endif
      I2CUtil I2C = I2CUtil(); // I2C Scanner && Twatch I2C bus

      // TODO: source agnostic Button_Class
      Button BtnA = Button(BUTTON_A_PIN, GPIO_BTN_INVERT, DEBOUNCE_MS);
      Button BtnB = Button(BUTTON_B_PIN, GPIO_BTN_INVERT, DEBOUNCE_MS);
      Button BtnC = Button(BUTTON_C_PIN, GPIO_BTN_INVERT, DEBOUNCE_MS);

      #if defined ARDUINO_ODROID_ESP32
        #define DEBOUNCE_MS_XY 5
        Button JOY_Y     = Button(BUTTON_JOY_Y_PIN,  GPIO_BTN_INVERT, DEBOUNCE_MS_XY);
        Button JOY_X     = Button(BUTTON_JOY_X_PIN,  GPIO_BTN_INVERT, DEBOUNCE_MS_XY);
        Button BtnMenu   = Button(BUTTON_MENU_PIN,   GPIO_BTN_INVERT, DEBOUNCE_MS);
        Button BtnVolume = Button(BUTTON_VOLUME_PIN, GPIO_BTN_INVERT, DEBOUNCE_MS);
        Button BtnSelect = Button(BUTTON_SELECT_PIN, GPIO_BTN_INVERT, DEBOUNCE_MS);
        Button BtnStart  = Button(BUTTON_START_PIN,  GPIO_BTN_INVERT, DEBOUNCE_MS);
        // Odroid-Go battery controller uses raw readings on adc1
        Battery battery;
      #endif

      #if defined HAS_BM8563
        RTC_BM8563 Rtc;
      #endif

      #if defined HAS_TOUCH
        Touch_Class Touch;
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
        // PWM implementation
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

};


using M5Stack  = ChimeraCore::ECCKernel;
// more object aliasing for other M5Stack clones
//using M5StickC = ChimeraCore::ECCKernel;
//using OdroidGo = ChimeraCore::ECCKernel;
//using TWatch   = ChimeraCore::ECCKernel;

extern M5Stack M5;
