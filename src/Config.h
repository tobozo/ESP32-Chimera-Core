#pragma once

#define HAS_SDCARD

// buttons debounce time (milliseconds)
#define DEBOUNCE_MS 10


#if defined( LGFX_ONLY ) // LGFX config loaded externally

  #define SPEAKER_PIN   -1
  #define SD_ENABLE      0
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1

  //#undef LGFX_AUTODETECT

#elif defined(ARDUINO_ESP32_DEV)

  #define TFT_LED_PIN  -1
  #define TFT_DC_PIN   32
  #define TFT_CS_PIN    5
  #define TFT_MOSI_PIN 23
  #define TFT_CLK_PIN  18
  #define TFT_RST_PIN  -1
  #define TFT_MISO_PIN 19

  // SD card
  #define TFCARD_CS_PIN 15
  #define SD_ENABLE     0
  #define TFCARD_CS_PIN       15
  #define TFCARD_MISO_PIN     12
  #define TFCARD_MOSI_PIN     13
  #define TFCARD_SCLK_PIN     14
  #define TFCARD_USE_WIRE1

  #define SPEAKER_PIN  -1

  #define BUTTON_A_PIN 39
  #define BUTTON_B_PIN 27
  #define BUTTON_C_PIN 26

  #undef  TOUCH_CS // using I2C touch
  #define HAS_TOUCH
  #define TOUCH_SDA    21
  #define TOUCH_SCL    22
  #define TOUCH_INT     4

  #define LORA_CS_PIN   33
  #define LORA_RST_PIN  -1
  #define LORA_IRQ_PIN  36

  #define RTC_IRQ_PIN   34
  #define IOE_IRQ_PIN   35
  #define GNSS_IRQ_PIN   2

  #define ETH_IRQ_PIN   25
  #define ETH_CS_PIN     0

#elif defined( ARDUINO_LOLIN_D32_PRO )

  //#warning "USING LoLin D32 Pro setup with Touch enabled for ESP32Marauder"
  #define SPEAKER_PIN   -1
  #define SD_ENABLE      1
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1 // BUTTON_MENU
  //#define TOUCH_CS      21 // enable touch
  #define TFCARD_CS_PIN  4
  #define TFCARD_SPI_FREQ 25000000
  #define TFCARD_USE_WIRE1
  #define TFCARD_MISO_PIN     MISO
  #define TFCARD_MOSI_PIN     MOSI
  #define TFCARD_SCLK_PIN     SCK
  #define TFCARD_SPI_HOST     SPI_HOST

#elif defined( ARDUINO_ESP32_WROVER_KIT )

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


#elif defined ARDUINO_TWATCH_BASE || defined ARDUINO_TWATCH_2020_V1 || defined ARDUINO_TWATCH_2020_V2 // TTGO T-Watch

  #define BUTTON_A_PIN        36
  #define BUTTON_B_PIN        -1
  #define BUTTON_C_PIN        -1

  #define TFCARD_CS_PIN       13
  #define TFCARD_MISO_PIN      2
  #define TFCARD_MOSI_PIN     15
  #define TFCARD_SCLK_PIN     14

  #define TFCARD_USE_WIRE1

  #define SEN_SDA             21
  #define SEN_SCL             22

  #define UART_TX             33
  #define UART_RX             34

  #define TOUCH_SDA           23
  #define TOUCH_SCL           32
  #define TOUCH_INT           38

  #define SPEAKER_PIN          4

  #if defined( LILYGO_WATCH_2019_WITH_TOUCH )
    // #include "board/twatch2019_with_touch.h"
    //#warning "Selected LILYGO_WATCH_2019_WITH_TOUCH"
    #define SD_ENABLE            1
    #define LILYGO_WATCH_HAS_BMA423
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BACKLIGHT
    #define LILYGO_WATCH_HAS_BUTTON
    #define HAS_TOUCH
  #elif defined(LILYGO_WATCH_2019_NO_TOUCH)
    //#include "board/twatch2019_with_not_touch.h"
    //#warning "Selected LILYGO_WATCH_2019_NO_TOUCH"
    #define SD_ENABLE            1
    #define LILYGO_WATCH_HAS_BMA423
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BACKLIGHT
    #define LILYGO_WATCH_HAS_BUTTON
  #elif defined(LILYGO_WATCH_BLOCK) // should be called "brick" :-)
    //#include "board/twatch_block.h"
    //#warning "Selected LILYGO_WATCH_BLOCK"
    #define SD_ENABLE            0
    #undef HAS_SDCARD
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BUTTON
    #define LILYGO_WATCH_HAS_MPU6050
    #define HAS_TOUCH
  #else // all LILYGO_WATCH_2020 models
    // common settings across v1/v2/v3
    #define SD_ENABLE            0
    #undef HAS_SDCARD
    #undef  SPEAKER_PIN
    #define SPEAKER_PIN         -1
    #define HAS_TOUCH
    #if defined(BOARD_HAS_PSRAM)
      //#include "board/twatch2020_v1.h"
      //#warning "Defaulting to LILYGO_WATCH_2020_V1"
      #define LILYGO_WATCH_HAS_MOTOR
      #define LILYGO_WATCH_HAS_BMA423
      #define HAS_AXP202
      #define LILYGO_WATCH_HAS_IRREMOTE
      #define HAS_PCF8563
      #define HAS_RTC
      //#define LILYGO_WATCH_HAS_BACKLIGHT
    #else // TODO: implement v3
      //#include "board/twatch2020_v2.h"
      //#warning "Defaulting to LILYGO_WATCH_2020_V2"
      #define TOUCH_RST                   4 // !! 2020_v2 only
      #define LILYGO_WATCH_HAS_BMA423
      #define HAS_AXP202
      #define LILYGO_WATCH_HAS_IRREMOTE
      #define LILYGO_WATCH_HAS_BACKLIGHT
      #define LILYGO_WATCH_HAS_GPS
    #endif
  #endif // LILYGO_WATCH_2020 models


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
  #define SPEAKER_PIN   25
  #define TFCARD_CS_PIN 13
  #define SD_ENABLE      0
  #define BUTTON_A_PIN  39
  #define BUTTON_B_PIN  34
  #define BUTTON_C_PIN  35


#elif defined( ARDUINO_ODROID_ESP32 )

  #define TFT_LED_PIN       32
  #define TFT_DC_PIN        27
  #define TFT_CS_PIN        14
  #define TFT_MOSI_PIN      23
  #define TFT_CLK_PIN       18
  #define TFT_RST_PIN       33
  #define TFT_MISO_PIN      19
  // SD card
  #define TFCARD_CS_PIN     22
  #define SD_ENABLE          1
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

#elif defined( ARDUINO_M5Stick_C ) // M5Stick C

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

  #define HAS_RTC
  #define HAS_AXP192
  #define HAS_BM8563

#elif defined( ARDUINO_M5Stick_C_Plus ) // M5Stick C Plus

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

  #define HAS_RTC
  #define HAS_AXP192
  #define HAS_BM8563

#elif defined( ARDUINO_M5STACK_Core2  ) // M5Core2

  #define TFT_LED_PIN  -1
  #define TFT_DC_PIN   15
  #define TFT_CS_PIN    5
  #define TFT_MOSI_PIN 23
  #define TFT_CLK_PIN  18
  #define TFT_RST_PIN  -1
  #define TFT_MISO_PIN 38

  #undef TOUCH_CS // using I2C touch
  #define HAS_TOUCH
  #define HAS_RTC
  #define HAS_AXP192
  #define HAS_BM8563
  #define HAS_MPU6886

  #define TFCARD_CS_PIN 4
  #define SD_ENABLE     1
  #define SPEAKER_PIN  25
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1

#elif defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE) // m5stack classic/fire

  #define HAS_IP5306
  #define HAS_MPU9250

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


#elif defined CONFIG_IDF_TARGET_ESP32S2 // ESP32-S2 basic support

  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1
  #define SD_ENABLE     0
  #define SPEAKER_PIN  -1
  #define VSPI FSPI
  #undef HAS_SDCARD

#else
  #pragma "No know board detected, disabling Buttons and SD"
  #define SD_ENABLE 0
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1
  #define SPEAKER_PIN  -1
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

#if defined MPU9250_INSDE && ! defined HAS_MPU9250
  #define HAS_MPU9250
#endif

#if SPEAKER_PIN != -1
  #define HAS_SPEAKER
#endif


#define TONE_PIN_CHANNEL 0

// LORA
#if !defined(LORA_CS_PIN)
  #define LORA_CS_PIN   5
  #define LORA_RST_PIN  26
  #define LORA_IRQ_PIN  36
#endif

// UART
#define USE_SERIAL Serial


#if !defined(TFCARD_CS_PIN)
 #define TFCARD_CS_PIN SS
#endif

#if !defined(TFCARD_SPI_FREQ)
  #define TFCARD_SPI_FREQ 25000000
#endif

#if defined M5STACK_MPU6886 && !defined HAS_MPU6886
  #define HAS_MPU6886
#endif


#if defined ESP_ARDUINO_VERSION_VAL
  #if __has_include("core_version.h") // for platformio
    #include "core_version.h"
  #endif
  #if ESP_ARDUINO_VERSION_VAL(2,0,1) >= ESP_ARDUINO_VERSION || ARDUINO_ESP32_GIT_VER == 0x15bbd0a || ARDUINO_ESP32_GIT_VER == 0xd218e58f
    // #pragma message "Filesystem can create subfolders on file creation"
    #define FS_CAN_CREATE_PATH
  #endif
#endif

