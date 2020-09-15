// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "M5Stack.h"

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
      Wire.begin(32, 33);
    }
    Axp.begin();
  #else
    // TF Card
    if (SDEnable == true) {
      sd_begin();
    }
  #endif

  // LCD INIT
  if (LCDEnable == true) {
    log_d("Enabling LCD");
    #if defined(PANEL_INIT)
      // is that even used ?
      panelInit();
    #endif
    Lcd.begin();

    if( ScreenShotEnable == true ) {
       ScreenShot.init( &Lcd, M5STACK_SD );
       ScreenShot.begin();
    }

    #if defined( TOUCH_CS )
    /*
      // TODO: deprecate this
      delay(100);
      //ts = new XPT2046_Touchscreen(21);
      if(ts != nullptr ) {
        ts->begin();
        ts->setRotation(0);
      }
    */
    #endif

  }

  #if  defined( ARDUINO_M5STACK_Core2 ) // M5Core2 starts APX after display is on
    // Touch init
    Touch.begin(); // Touch begin after AXP begin. (Reset at the start of AXP)

  #endif

  // TF Card ( reinit )
  if (SDEnable == true && M5STACK_SD.cardSize() == 0) {
    sd_begin();
  }

  // TONE
  // Speaker.begin();
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
        Wire.begin(SDA, SCL);
      } else {
        Wire.begin(32, 33);
      }
      I2C.scan();
    }
  #endif

  if (SerialEnable == true) {
    Serial.println("OK");
  }

  #if defined( ARDUINO_M5STACK_Core2 )
    Rtc.begin();
  #elif defined( ARDUINO_T_Watch )
    #if defined( LILYGO_WATCH_HAS_PCF8563 )
      Rtc = new PCF8563_Class(I2C);
    #endif
  #endif
}

void M5Stack::update() {
  //Button update
  if (M5.Lcd.getBoard() == lgfx::board_M5StackCore2) {
    int32_t x, y;
    int idx = -1;

    if (M5.Lcd.getTouchRaw(&x, &y) && y >= 256) {
      idx = x * 3 / 320;
    }

    BtnA.setState(idx == 0);
    BtnB.setState(idx == 1);
    BtnC.setState(idx == 2);
  } else {
    BtnA.read();
    BtnB.read();
    BtnC.read();
  }
  //Speaker update
  Speaker.update();
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


#if defined( ARDUINO_T_Watch )
  SPIClass *sdhander = nullptr;
#endif

bool M5Stack::sd_begin(void)
{
  bool ret = false;
  #if defined ( USE_TFCARD_CS_PIN ) && defined( TFCARD_CS_PIN )

    #if defined( ARDUINO_T_Watch )

      if( SD_ENABLE == 0 ) return true;

      if (!sdhander) {
        sdhander = new SPIClass(HSPI);
        sdhander->begin(TFCARD_SCLK_PIN, TFCARD_MISO_PIN, TFCARD_MOSI_PIN, TFCARD_CS_PIN);
      }
      if (!SD.begin(TFCARD_CS_PIN, *sdhander)) {
        log_e("SD Card Mount Failed");
        return false;
      }
      return true;

    #else

      log_d("Enabling SD from TFCARD_CS_PIN #%d at %d Hz", TFCARD_CS_PIN, TFCARD_SPI_FREQ);

      M5STACK_SD.end();
      ret = M5STACK_SD.begin(TFCARD_CS_PIN, SPI, TFCARD_SPI_FREQ);

      if ( lgfx::LGFX_Config::spi_host == HSPI_HOST ) {
        Lcd.setSPIShared(false);
      }

    #endif
  #else
    log_d("Enabling SD_MMC");
    ret = M5STACK_SD.begin();
    Lcd.setSPIShared(false);
  #endif
  return ret;
}

void M5Stack::sd_end(void)
{
  M5STACK_SD.end();
}

M5Stack M5;
