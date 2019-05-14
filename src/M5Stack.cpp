// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "M5Stack.h"

M5Stack::M5Stack() : isInited(0) {
}

void M5Stack::begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable) {
  // Correct init once
  if (isInited == true) {
    return;
  } else {
    isInited = true;
  }

  // UART
  if (SerialEnable == true) {
    Serial.begin(115200);
    Serial.flush();
    delay(50);
    Serial.print("M5Stack initializing...");
  }

  // LCD INIT
  if (LCDEnable == true) {
    Lcd.begin();
  }

  // TF Card
  if (SDEnable == true) {
    SD.begin(TFCARD_CS_PIN, SPI, 40000000);
  }

  // TONE
  // Speaker.begin();

#ifdef ARDUINO_ODROID_ESP32
  pinMode(BUTTON_MENU_PIN, INPUT_PULLUP);
  pinMode(BUTTON_VOLUME_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_START_PIN, INPUT_PULLUP);
  pinMode(BUTTON_JOY_Y_PIN, INPUT_PULLDOWN);
  pinMode(BUTTON_JOY_X_PIN, INPUT_PULLDOWN);
#endif

  // Set wakeup button
  Power.setWakeupButton(BUTTON_A_PIN);

  // I2C init
  if (I2CEnable == true) {
    Wire.begin(21, 22);
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

M5Stack M5;
