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

#ifdef TOUCH_CS

void M5Stack::setTouchSpiShared( int32_t csPin, int32_t IRQPin ) {
  if( ts == nullptr ) {
    ts = new XPT2046_Touchscreen(csPin, IRQPin);
    Lcd.initTouch( ts );
  }
  // TODO: also implement FT5206_Clas
}

#endif


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

  // TF Card
  if (SDEnable == true) {
    sd_begin();
  }

  // LCD INIT
  if (LCDEnable == true) {
    log_d("Enabling LCD");
    #if defined(PANEL_INIT)
      panelInit()
    #endif
    Lcd.begin();

    // provide consistent getWidth()/getHeight() to both Sprite and TFT image decoding
//    Lcd.setWidthGetter        = &setWidthGetter;
//    Lcd.setHeightGetter       = &setHeightGetter;
//    //Lcd.setColorPusher        = &setColorPusher;
//    Lcd.setWindowSetter       = &setWindowSetter;
//    //Lcd.setColorWriter        = &setColorWriter;
//    Lcd.setColorWriterArray   = &setColorWriterArray;
//    Lcd.setRgb565Converter    = &setRgb565Converter;
//    Lcd.setTransactionStarter = &setTransactionStarter;
//    Lcd.setTransactionEnder   = &setTransactionEnder;
//
//    setJpgRenderer( true );
//
//    Lcd.setPngRenderCallBack  = &pngCallBackSetter;
//    Lcd.pngFlashRenderFunc    = &pngRenderer;
//    Lcd.pngFSRenderFunc       = &pngRenderer;
//    Lcd.pngStreamRenderFunc   = &pngRenderer;

    if( ScreenShotEnable == true ) {
       ScreenShot.init( &Lcd, M5STACK_SD );
       ScreenShot.begin();
    }

    #if defined( TOUCH_CS )
      delay(100);
      //ts = new XPT2046_Touchscreen(21);
      if(ts != nullptr ) {
        ts->begin();
        ts->setRotation(0);
      }
    #endif


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


  // Set wakeup button
  Power.setWakeupButton(BUTTON_A_PIN);

  // I2C init
  if (I2CEnable == true) {
    log_d("Enabling I2C");
    Wire.begin(SDA, SCL);
    I2C.scan();
  }

  if (SerialEnable == true) {
    Serial.println("OK");
  }
}

void M5Stack::update() {
  //Button update
  BtnA.read();
  BtnB.read();
  BtnC.read();

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

void M5Stack::sd_begin(void)
{
  #if defined ( USE_TFCARD_CS_PIN ) && defined( TFCARD_CS_PIN )

    log_d("Enabling SD from TFCARD_CS_PIN");

    M5STACK_SD.end();
    SPI.end();
    SPI.begin();
    M5STACK_SD.begin(TFCARD_CS_PIN, SPI, 20000000);

    if ( lgfx::LGFX_Config::spi_host == HSPI_HOST ) {
      Lcd.setSPIShared(false);
    }
  #else
    log_d("Enabling SD_MMC");
    M5STACK_SD.begin();
    Lcd.setSPIShared(false);
  #endif
}

void M5Stack::sd_end(void)
{
  M5STACK_SD.end();
}

M5Stack M5;
