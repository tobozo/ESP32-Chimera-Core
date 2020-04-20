#ifndef _CONFIG_H_
#define _CONFIG_H_

#if defined( ARDUINO_LOLIN_D32_PRO )

  //#warning "USING LoLin D32 Pro setup with Touch enabled for ESP32Marauder"
  #define SPEAKER_PIN   -1
  #define SD_ENABLE     0
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1  // BUTTON_MENU


#elif defined(ARDUINO_ESP32_DEV)

  #define TFT_LED_PIN   14
  #define TFT_DC_PIN    21
  #define TFT_CS_PIN     5
  #define TFT_MOSI_PIN  23
  #define TFT_CLK_PIN   18
  #define TFT_RST_PIN   -1
  #define TFT_MISO_PIN  19
  // SD card
  #define TFCARD_CS_PIN 22
  #define SPEAKER_PIN   -1
  #define SD_ENABLE      0
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1  // BUTTON_MENU

#elif defined(ARDUINO_D) || defined(ARDUINO_DDUINO32_XS)

  #define TFT_LED_PIN  22
  #define TFT_DC_PIN   23
  #define TFT_RST_PIN  32
  #define TFT_MOSI_PIN 26
  #define TFT_CLK_PIN 27
  #define NEOPIXEL_PIN 25    // Digital IO pin connected to the NeoPixels.
  #define SPEAKER_PIN  33
  #define SD_ENABLE     0
  #define BUTTON_A_PIN  5
  #define BUTTON_B_PIN 18
  #define BUTTON_C_PIN 19

#elif defined(ARDUINO_T) // TTGO T-Watch

  #define TFCARD_CS_PIN 13

  #define SPEAKER_PIN  -1
  #define SD_ENABLE     1
  #define BUTTON_A_PIN  36
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1

#elif defined(ARDUINO_TTGO_T1)

  // TFT/OLED display
  #define TFT_CS_PIN    16
  #define TFT_RST_PIN    9  // you can also connect this to the Arduino reset
  #define TFT_DC_PIN    17
  #define TFT_CLK_PIN    5   // set these to be whatever pins you like!
  #define TFT_MOSI_PIN  23   // set these to be whatever pins you like!
  #define TFT_LED_PIN   27
  #define TFT_MISO_PIN  -1
  // if (!SD.begin(13, 15, 2, 14)) {            //T1:13,15,2,14  T2: 23,5,19,18
  // boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN, int8_t mosi = -1, int8_t miso = -1, int8_t sck = -1);
  #define SPEAKER_PIN 25
  #define TFCARD_CS_PIN 13
  #define SD_ENABLE      0
  #define BUTTON_A_PIN  39
  #define BUTTON_B_PIN  34
  #define BUTTON_C_PIN  35

#elif defined(ARDUINO_ODROID_ESP32)

  #define TFT_LED_PIN       32
  #define TFT_DC_PIN        27
  #define TFT_CS_PIN        14
  #define TFT_MOSI_PIN      23
  #define TFT_CLK_PIN       18
  #define TFT_RST_PIN       33
  #define TFT_MISO_PIN      19
  // SD card
  #define TFCARD_CS_PIN      4
  #define SD_ENABLE 1
  #define SPEAKER_PIN       26
  #define BUTTON_A_PIN      32
  #define BUTTON_B_PIN      33
  #define BUTTON_C_PIN      13  // BUTTON_MENU
  // odroid-go has many buttons
  #define BUTTON_MENU_PIN   13
  #define BUTTON_SELECT_PIN 27
  #define BUTTON_VOLUME_PIN  0
  #define BUTTON_START_PIN  39
  #define BUTTON_JOY_Y_PIN  35
  #define BUTTON_JOY_X_PIN  34

#elif defined(ARDUINO_M5Stick_C) // M5Stick C

  #define SPEAKER_PIN   -1
  #define TFT_DC_PIN   23
  #define TFT_CS_PIN    5
  #define TFT_MOSI_PIN 15
  #define TFT_CLK_PIN  13
  #define TFT_RST_PIN  18
  #define TFT_MISO_PIN 14

  #define M5_IR      9
  #define M5_LED     10
  #define M5_BUTTON_HOME 37
  #define M5_BUTTON_RST  39

  #define BUTTON_A_PIN 37
  #define BUTTON_B_PIN 39
  #define BUTTON_C_PIN  -1
  #define SD_ENABLE      0

#else // m5stack classic/fire
  #define TFT_LED_PIN  32
  #define TFT_DC_PIN   27
  #define TFT_CS_PIN   14
  #define TFT_MOSI_PIN 23
  #define TFT_CLK_PIN  18
  #define TFT_RST_PIN  33
  #define TFT_MISO_PIN 19
  // SD card
  #define TFCARD_CS_PIN 4
  #define SD_ENABLE     1
  #define SPEAKER_PIN  25
  #define BUTTON_A_PIN 39
  #define BUTTON_B_PIN 38
  #define BUTTON_C_PIN 37

#endif

// Default Buttons (M5Stack or assimilate)
#define BTN_A 0
#define BTN_B 1
#define BTN_C 2
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_C 2
// Extra Buttons (Odroid-Go or assimilate)
#define BTN_MENU   2
#define BTN_SELECT 3
#define BTN_VOLUME 4
#define BTN_START  5
#define BTN_JOY_Y  6
#define BTN_JOY_X  7

#define TONE_PIN_CHANNEL 0

// LORA
#define LORA_CS_PIN   5
#define LORA_RST_PIN  26
#define LORA_IRQ_PIN  36

// UART
#define USE_SERIAL Serial


#if !defined(TFCARD_CS_PIN)
 #define TFCARD_CS_PIN SS
#endif

#endif // _CONFIG_H_
