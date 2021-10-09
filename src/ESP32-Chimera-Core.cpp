// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ESP32-Chimera-Core.h"


M5Stack::M5Stack() : isInited(0) {
}

//void M5Stack::setJpgRenderer( bool legacy ) {
//  if( legacy ) {
//    islegacyJpegDecoder = true;
//    Lcd.setJpegRenderCallBack = nullptr;
//    Lcd.jpgFlashRenderFunc    = &jpgLegacyRenderer;
//    Lcd.jpgFSRenderFunc       = &jpgLegacyRenderer;
//    Lcd.jpgStreamRenderFunc   = &jpgLegacyRenderer;
//  } else {
//    islegacyJpegDecoder = false;
//    Lcd.setJpegRenderCallBack = &jpgCallBackSetter;
//    Lcd.jpgFlashRenderFunc    = &jpgRenderer;
//    Lcd.jpgFSRenderFunc       = &jpgRenderer;
//    Lcd.jpgStreamRenderFunc   = &jpgRenderer;
//  }
//}


void M5Stack::begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable, bool ScreenShotEnable) {
  // Correct init once
  if (isInited == true) {
    log_d("ESP32-Chimera-Core Already inited");
    return;
  } else {
    isInited = true;
  }

  // UART
  if (SerialEnable == true) {
    Serial.begin(115200);
    Serial.flush();
    delay(50);
    Serial.print("ESP32-Chimera-Core initializing...");
  }

  #if defined( ARDUINO_M5STACK_Core2 ) // M5Core2 starts APX after display is on
    // I2C init
    if (I2CEnable == true) {
      I2C.begin(32, 33);
    }
    Axp.begin();
    Axp.SetLDOEnable( 3,0 ); // turn any vibration off
  #else
    // LGFX_VERSION_MAJOR is only defined since LGFX V1
    #if !defined LGFX_VERSION_MAJOR
      // LGFX V0 has TF Card preinit before TFT
      if (SDEnable == true) {
        sd_begin();
      }
    #endif
  #endif

  #if defined ARDUINO_TWATCH_BASE || defined ARDUINO_TWATCH_2020_V1 || defined ARDUINO_TWATCH_2020_V2 // TTGO T-Watch
    #if defined LILYGO_WATCH_HAS_AXP202

      Wire1.begin(21, 22);

      //int ret = Axp->begin(i2cReadBytes, i2cWriteBytes);
      int ret = Axp->begin(Wire1);
      if (ret == AXP_FAIL) {
        log_e("AXP Power begin failed");
      } else {
        log_n("AXP Power begin success!");
        //Change the shutdown time to 4 seconds
        Axp->setShutdownTime(AXP_POWER_OFF_TIME_4S);
        // Turn off the charging instructions, there should be none
        Axp->setChgLEDMode(AXP20X_LED_OFF);
        // Turn off external enable
        Axp->setPowerOutPut(AXP202_EXTEN, false);
        //axp202 allows maximum charging current of 1800mA, minimum 300mA
        Axp->setChargeControlCur(300);
      }
      //#ifdef  LILYGO_WATCH_2020_V1
        //In the 2020V1 version, the ST7789 chip power supply
        //is shared with the backlight, so LDO2 cannot be turned off
        log_w("Setting power output for ST7789");
        Axp->setPowerOutPut(AXP202_LDO2, AXP202_ON);
      //#endif  /*LILYGO_WATCH_2020_V1*/
      //#ifdef  LILYGO_WATCH_2020_V2
        //GPS power domain is AXP202 LDO4
        //Axp->setPowerOutPut(AXP202_LDO3, false);
        //Axp->setPowerOutPut(AXP202_LDO4, false);
        //Axp->setLDO4Voltage(AXP202_LDO4_3300MV);
      //#endif  /*LILYGO_WATCH_2020_V2*/
    #endif

  #endif

  // LCD INIT
  if (LCDEnable == true) {
    log_d("Enabling LCD");

    Lcd.begin();
    if( ScreenShotEnable == true ) {
       #if defined HAS_SDCARD
         ScreenShot.init( &Lcd, M5STACK_SD );
         ScreenShot.begin();
       #endif
    }

  }

  #if defined( ARDUINO_M5STACK_Core2 ) // M5Core2 starts APX after display is on
    Touch.begin(); // Touch begin after AXP begin. (Reset at the start of AXP)

    Button* _btns[3] = { &BtnA, &BtnB, &BtnC };
    M5Core2TouchButtonEmu = new ButtonTouchReader( &Lcd, _btns );

  #endif

  #if defined HAS_SDCARD
    // TF Card ( reinit )
    if (SDEnable == true && M5STACK_SD.cardSize() == 0) {
      sd_begin();
    }
  #endif

  // TONE (let the user enable that manually)
  #ifdef HAS_SPEAKER
    //Speaker.begin();
  #endif


  // Buttons init
  #ifdef ARDUINO_DDUINO32_XS
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(BUTTON_B_PIN, INPUT_PULLUP);
    pinMode(BUTTON_C_PIN, INPUT_PULLUP);
  #endif
  #ifdef ARDUINO_ODROID_ESP32
    pinMode(BUTTON_MENU_PIN, INPUT_PULLUP);
    pinMode(BUTTON_VOLUME_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
    pinMode(BUTTON_START_PIN, INPUT_PULLUP);
    pinMode(BUTTON_JOY_Y_PIN, INPUT_PULLDOWN);
    pinMode(BUTTON_JOY_X_PIN, INPUT_PULLDOWN);
  #endif

  #if defined(ARDUINO_M5Stick_C) // M5Stick C
    Axp.begin();
    Rtc.begin();
  #endif

  #if defined HAS_POWER
    // Set wakeup button
    Power.setWakeupButton(BUTTON_A_PIN);
  #endif

  #if !defined( ARDUINO_M5STACK_Core2 )
    // I2C init
    if (I2CEnable == true) {
      log_d("Enabling I2C");
      if (M5.Lcd.getBoard() != lgfx::board_M5StackCore2) {
        I2C.begin(SDA, SCL);
      } else {
        I2C.begin(32, 33);
      }
    }
  #endif

  if (SerialEnable == true) {
    Serial.println("OK");
  }

  #if defined( ARDUINO_M5STACK_Core2 )
    Rtc.begin();
  #elif defined ARDUINO_TWATCH_BASE || defined ARDUINO_TWATCH_2020_V1 || defined ARDUINO_TWATCH_2020_V2 // TTGO T-Watch
    #if defined( LILYGO_WATCH_HAS_PCF8563 )
      Rtc = new PCF8563_Class(I2C);
    #endif
  #endif

  #if defined HAS_RTC
    setSystemTimeFromRtc();
  #endif
}



void M5Stack::update() {
  //Button update
  #if defined ARDUINO_M5STACK_Core2
    M5Core2TouchButtonEmu->read();
  #else
    BtnA.read();
    BtnB.read();
    BtnC.read();
  #endif
  //Speaker update
  #ifdef HAS_SPEAKER
    Speaker.update();
  #endif

  #ifdef ARDUINO_ODROID_ESP32
    BtnMenu.read();
    BtnVolume.read();
    BtnSelect.read();
    BtnStart.read();
    JOY_Y.readAxis();
    JOY_X.readAxis();
    battery.update();
  #endif
}

#if defined HAS_POWER

  /**
    * Function has been move to Power class.(for compatibility)
    * This name will be removed in a future release.
    */
  void M5Stack::setPowerBoostKeepOn(bool en) {
    M5.Power.setPowerBoostKeepOn(en);
  }
  /**
    * Function has been move to Power class.(for compatibility)
    * This name will be removed in a future release.
    */
  void M5Stack::setWakeupButton(uint8_t button) {
    M5.Power.setWakeupButton(button);
  }
  /**
    * Function has been move to Power class.(for compatibility)
    * This name will be removed in a future release.
    */
  void M5Stack::powerOFF() {
    M5.Power.deepSleep();
  }

#endif



bool M5Stack::sd_begin(void)
{
  if( sd_begun ) return true;
  bool ret = false;
  SD_CORE_ID = xPortGetCoreID();
  #if defined ( USE_TFCARD_CS_PIN ) && defined( TFCARD_CS_PIN )

    #if defined ( TFCARD_USE_WIRE1 )

      if( sd_force_enable == 0 ) {
        log_w("SD Disabled by config, aborting");
        return true;
      }

      if ( SD_SPI == nullptr ) {
        #if defined TFCARD_SPI_HOST
          if( TFCARD_SPI_HOST==VSPI_HOST ) {
            SD_SPI = new SPIClass(VSPI);
            log_i("SD will use VSPI");
          }else if( TFCARD_SPI_HOST==HSPI_HOST ) {
            SD_SPI = new SPIClass(HSPI);
            log_i("SD will use HSPI");
          } else if( TFCARD_SPI_HOST==SPI_HOST ) {
            SD_SPI = new SPIClass(SPI);
            log_i("SD will use SPI");
          } else {
            log_e("No TFCARD_SPI_HOST selected in config");
            return false;
          }
          log_i("TFCARD_SPI_HOST=%d, SPI_HOST=%d, HSPI_HOST=%d, VSPI_HOST=%d from core #%d", TFCARD_SPI_HOST, SPI_HOST, HSPI_HOST, VSPI_HOST, SD_CORE_ID );
//           #define SPI_HOST    SPI1_HOST
//           #define HSPI_HOST   SPI2_HOST
//           #define VSPI_HOST   SPI3_HOST
        #else
          log_i("SD will use HSPI (default)");
          SD_SPI = new SPIClass(HSPI);
        #endif
        SD_SPI->begin(TFCARD_SCLK_PIN, TFCARD_MISO_PIN, TFCARD_MOSI_PIN, TFCARD_CS_PIN);
      }
      if (!SD.begin(TFCARD_CS_PIN, *SD_SPI)) {
        log_e("SD Card Mount Failed pins scl/miso/mosi/cs %d/%d/%d/%d from core #%d", TFCARD_SCLK_PIN, TFCARD_MISO_PIN, TFCARD_MOSI_PIN, TFCARD_CS_PIN, SD_CORE_ID );
        return false;
      } else {
        log_w( "SD Card Mount Success on pins scl/miso/mosi/cs %d/%d/%d/%d from core #%d", TFCARD_SCLK_PIN, TFCARD_MISO_PIN, TFCARD_MOSI_PIN, TFCARD_CS_PIN, SD_CORE_ID );
      }
      sd_begun = true;
      return true;

    #else

      #if defined HAS_SDCARD
        log_w("Enabling SD from TFCARD_CS_PIN #%d at %d Hz from core #%d", TFCARD_CS_PIN, TFCARD_SPI_FREQ, SD_CORE_ID );
        M5STACK_SD.end();
        ret = M5STACK_SD.begin(TFCARD_CS_PIN, SPI, TFCARD_SPI_FREQ);
      #endif

    #endif
  #else
    #if defined HAS_SDCARD
      log_d("Enabling SD_MMC");
      ret = M5STACK_SD.begin();
    #endif
  #endif
  if( ret ) sd_begun = true;
  return ret;
}

void M5Stack::sd_end(void)
{
  #if defined HAS_SDCARD
    M5STACK_SD.end();
  #endif
  sd_begun = false;
}



#if defined HAS_RTC
  #include <sys/time.h> // needed to set system time
  // Pickup the time from RTC module and apply it to system time (ESP32's internal, battery-less RTC).
  // This is particularly useful as the SD Card writes and file creations inherit system date/time.
  void M5Stack::setSystemTimeFromRtc()
  {
    struct tm rtcnow;
    struct tm sysnow;
    time_t unixtime;


    #if defined( ARDUINO_M5Stick_C ) || defined ( ARDUINO_M5Stick_C_Plus ) || defined ARDUINO_M5STACK_Core2
      // BM8563: "Core2/M5StickC/Plus" style RTC
      RTC_TimeTypeDef RTCtime;
      RTC_DateTypeDef RTCDate;
      Rtc.GetTime(&RTCtime); //Gets the time in the real-time clock.
      Rtc.GetDate(&RTCDate);
      rtcnow.tm_year  = RTCDate.Year-1900;  // Year - 1900
      rtcnow.tm_mon   = RTCDate.Month-1;    // Month, where 0 = jan
      rtcnow.tm_mday  = RTCDate.Date;       // Day of the month
      rtcnow.tm_hour  = RTCtime.Hours;
      rtcnow.tm_min   = RTCtime.Minutes;
      rtcnow.tm_sec   = RTCtime.Seconds;
    #elif defined LILYGO_WATCH_HAS_PCF8563
      // PCF8563
      RTC_Date RTCDate = Rtc->getDateTime();
      rtcnow.tm_year  = RTCDate.year-1900;  // Year - 1900
      rtcnow.tm_mon   = RTCDate.month-1;    // Month, where 0 = jan
      rtcnow.tm_mday  = RTCDate.day;        // Day of the month
      rtcnow.tm_hour  = RTCDate.hour;
      rtcnow.tm_min   = RTCDate.minute;
      rtcnow.tm_sec   = RTCDate.second;
    #else
      #warning "HAS_RTC is set but no means are provided to retrieve data/time"
    #endif

    rtcnow.tm_isdst = -1; // Is DST on? 1=yes, 0=no, -1=unknown

    unixtime = mktime(&rtcnow); // to unix time
    timeval epoch = {(time_t)unixtime, 0}; // to epoch time
    const timeval *tv = &epoch; // to timeval pointer
    settimeofday(tv, NULL);

    if( getLocalTime(&sysnow,0) ) {
      //log_w("System time adjusted from M5Core2 RTC (BM8563)");
      Serial.printf("Fetched RTC Date/Time: %d/%02d/%02d %02d:%02d:%02d (DST=%d)\n",
        rtcnow.tm_year+1900,  // Year - 1900
        rtcnow.tm_mon+1,      // Month, where 0 = jan
        rtcnow.tm_mday,
        rtcnow.tm_hour,
        rtcnow.tm_min,
        rtcnow.tm_sec,
        rtcnow.tm_isdst
      );
      Serial.println(&sysnow,"System Date/Time set to: %B %d %Y %H:%M:%S (%A)");
    } else {
      log_e("System time could not be adjusted from RTC");
    }
  }


  void M5Stack::setRtcTime( uint16_t year, uint8_t month, uint8_t day , uint8_t hours, uint8_t minutes, uint8_t seconds )
  {
    #if defined( ARDUINO_M5Stick_C ) || defined ( ARDUINO_M5Stick_C_Plus ) || defined ARDUINO_M5STACK_Core2
      RTC_DateTypeDef RTCDate;
      RTC_TimeTypeDef RTCtime;
      RTCDate.Year    = year;    // (e.g. 2021)
      RTCDate.Month   = month;   // (e.g. January=1)
      RTCDate.Date    = day;     // (e.g. first=1)
      RTCtime.Hours   = hours;
      RTCtime.Minutes = minutes;
      RTCtime.Seconds = seconds;
      M5.Rtc.SetTime(&RTCtime);
      M5.Rtc.SetDate(&RTCDate);
      // should call M5.setSystemTimeFromRtc();
    #elif defined LILYGO_WATCH_HAS_PCF8563
      RTC_Date RTCDate;
      RTCDate.year   = year;
      RTCDate.month  = month;
      RTCDate.day    = day;
      RTCDate.hour   = hours;
      RTCDate.minute = minutes;
      RTCDate.second = seconds;
      Rtc->setDateTime( RTCDate );
    #else
      #warning "HAS_RTC is set but no means are provided to set data/time"
    #endif

  }

#endif


M5Stack M5;
