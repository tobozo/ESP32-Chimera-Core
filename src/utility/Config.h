#ifndef _CONFIG_H_
  #define _CONFIG_H_

  // Screen
  #ifdef ARDUINO_ODROID_ESP32
      #define TFT_LED_PIN 	14
      #define TFT_DC_PIN 		21
      #define TFT_CS_PIN 		5
      #define TFT_MOSI_PIN 	23
      #define TFT_CLK_PIN 	18
      #define TFT_RST_PIN 	-1
      #define TFT_MISO_PIN 	19

      // SD card
      #define TFCARD_CS_PIN 	22
  #else
      #define TFT_LED_PIN 32
      #define TFT_DC_PIN 27
      #define TFT_CS_PIN 14
      #define TFT_MOSI_PIN 23
      #define TFT_CLK_PIN 18
      #define TFT_RST_PIN 33
      #define TFT_MISO_PIN 19

      // SD card
      #define TFCARD_CS_PIN 4
  #endif

  ////#ifdef ARDUINO_ODROID_ESP32
  #define BTN_MENU 	2
  #define BTN_SELECT 	3
  #define BTN_VOLUME 	4
  #define BTN_START 	5	
  #define BTN_JOY_Y 	6
  #define BTN_JOY_X 	7
  ////#endif

  // Buttons
  #define BTN_A 0
  #define BTN_B 1
  #define BTN_C 2
  #define BUTTON_A 0
  #define BUTTON_B 1
  #define BUTTON_C 2 
  
  ////#ifdef ARDUINO_ODROID_ESP32
  #define BUTTON_MENU 	2
  #define BUTTON_SELECT 	3
  #define BUTTON_VOLUME 	4
  #define BUTTON_START 	5	
  #define BUTTON_JOY_Y 	6
  #define BUTTON_JOY_X 	7
  ////#endif

  #ifdef ARDUINO_ODROID_ESP32
      #define BUTTON_A_PIN 	32
      #define BUTTON_B_PIN 	33
      #define BUTTON_C_PIN 	13 	// BUTTON_MENU	
  #else
      #define BUTTON_A_PIN 	39
      #define BUTTON_B_PIN 	38
      #define BUTTON_C_PIN 	37
  #endif  

  #define BUTTON_MENU_PIN 	13
  #define BUTTON_SELECT_PIN 	27
  #define BUTTON_VOLUME_PIN 	0
  #define BUTTON_START_PIN 	39	
  #define BUTTON_JOY_Y_PIN 	35
  #define BUTTON_JOY_X_PIN 	34

  // BEEP PIN
  #ifdef ARDUINO_ODROID_ESP32
      #define SPEAKER_PIN 26
  #else
      #define SPEAKER_PIN 25
  #endif

  #define TONE_PIN_CHANNEL 0

  // LORA
  #define LORA_CS_PIN   5
  #define LORA_RST_PIN  26
  #define LORA_IRQ_PIN  36

  // UART
  #define USE_SERIAL Serial

#endif /* SETTINGS_C */
