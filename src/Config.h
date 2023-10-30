#pragma once

#define ECC_XSTR(x) ECC_STR(x)
#define ECC_STR(x) #x

#define HAS_SDCARD
//#define USE_SCREENSHOTS
//#define USE_NVSUTILS

// buttons debounce time (milliseconds)
#define DEBOUNCE_MS 10
#define GPIO_BTN_INVERT true

#include "pins_arduino.h"
#include "esp32-hal-log.h"
#include <sdkconfig.h>
#if __has_include(<esp_arduino_version.h>) // platformio has optional esp_arduino_version
  #include <esp_arduino_version.h>
#endif

#if !defined ECC_NO_PRAGMAS && CORE_DEBUG_LEVEL>=ARDUHAL_LOG_LEVEL_ERROR
  #define ECC_PRAGMA_XMESSAGE(msg) ECC_PRAGMA_MESSAGE(msg)
  #define ECC_PRAGMA_MESSAGE(msg) \
    _Pragma( ECC_STR( message msg ) )
  #define ECC_NO_PRAGMAS // stop flag propagation
#else
  #define ECC_PRAGMA_XMESSAGE(msg)
  #define ECC_PRAGMA_MESSAGE(msg)
#endif


#if defined ESP_ARDUINO_VERSION_VAL
  #if __has_include(<core_version.h>) // platformio has optional core_version.h
    #include <core_version.h>
  #endif

  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2,0,1) \
   || ARDUINO_ESP32_GIT_VER == 0x15bbd0a1 \
   || ARDUINO_ESP32_GIT_VER == 0xd218e58f \
   || ARDUINO_ESP32_GIT_VER == 0xcaef4006 \
   || ARDUINO_ESP32_GIT_VER == 0x1e388a24 \
   || ARDUINO_ESP32_GIT_VER == 0x142fceb8 \
   || ARDUINO_ESP32_GIT_VER == 0xc93bf11f \
   || ARDUINO_ESP32_GIT_VER == 0x2d6ca351 \
   || ARDUINO_ESP32_GIT_VER == 0xb63f9470 \
   || ARDUINO_ESP32_GIT_VER == 0x213f9769 \
   || ARDUINO_ESP32_GIT_VER == 0xd3254f75 \
   || ARDUINO_ESP32_GIT_VER == 0x3670e2bf
    // FS::open() can create subfolders
    #define FS_CAN_CREATE_PATH
  #endif

  #define ECC_VERSION_MSG esp32-arduino ESP_ARDUINO_VERSION_MAJOR.ESP_ARDUINO_VERSION_MINOR.ESP_ARDUINO_VERSION_PATCH (ARDUINO_ESP32_GIT_VER) detected
  ECC_PRAGMA_XMESSAGE( ECC_XSTR(ECC_VERSION_MSG) )

  #if ARDUINO_ESP32_GIT_VER == 0x15bbd0a1 // Introduces Wire::end()
    ECC_PRAGMA_MESSAGE("ESP32 Arduino 2.0.1 RC1 (0x15bbd0a1) is only partially supported")
  #elif ARDUINO_ESP32_GIT_VER == 0xd218e58f|| ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2,0,1)
    ECC_PRAGMA_MESSAGE("ESP32 Arduino 2.0.1 (0xd218e58f) has OTA support broken!!")
  #elif ARDUINO_ESP32_GIT_VER == 0xcaef4006 || ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2,0,2) // Introduces SD::readRAW() and SD::writeRAW() support
    ECC_PRAGMA_MESSAGE("ESP32 Arduino 2.0.2 (0xcaef4006) has SD support broken!!")
  #elif ARDUINO_ESP32_GIT_VER == 0x1e388a24 // Introduces ESP32S3, SD::numSectors() and SD::sectorSize() support
    ECC_PRAGMA_MESSAGE("ESP32 Arduino 2.0.3 RC1 (0x1e388a24) is only partially supported")
  #elif ARDUINO_ESP32_GIT_VER == 0x142fceb8 || ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2,0,3) // Introduces ESP32S3, SD::numSectors() and SD::sectorSize() support
    //ECC_PRAGMA_MESSAGE("ESP32 Arduino 2.0.3 (0x142fceb8) detected")
  #elif ARDUINO_ESP32_GIT_VER == 0xc93bf11f || ESP_ARDUINO_VERSION == ESP_ARDUINO_VERSION_VAL(2,0,4)
    // // Misc fixes, however the qspi_opi => qio_opi renaming messed up some S3 devices (including S3Box) in boards.txt
    #if defined ARDUINO_M5STACK_FIRE && defined BOARD_HAS_PSRAM && defined CONFIG_SPIRAM_SPEED_80M && defined CONFIG_SPIRAM_OCCUPY_VSPI_HOST
      ECC_PRAGMA_MESSAGE("4MB PSRAM modules may conflict with SD ( see https://github.com/espressif/arduino-esp32/issues/7192 )")
    #endif
  #endif
#endif



#if defined( LGFX_ONLY ) // LGFX config loaded externally

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("LGFX Only")
  #endif

  #define SPEAKER_PIN   -1
  #define SD_ENABLE      0
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1

  //#undef LGFX_AUTODETECT

#elif defined( ARDUINO_LOLIN_D32_PRO )

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("LOLIN_D32_PRO SELECTED")
  #endif

  //#pragma message "USING LoLin D32 Pro setup with Touch enabled for ESP32Marauder"
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


#elif defined ARDUINO_LOLIN_S3 || defined ARDUINO_LOLIN_S3_PRO || defined LGFX_LOLIN_S3_PRO

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("LOLIN_S32_PRO SELECTED")
  #endif

  #define SPEAKER_PIN   -1
  #define SD_ENABLE      1
  #define BUTTON_A_PIN  -1
  #define BUTTON_B_PIN  -1
  #define BUTTON_C_PIN  -1 // BUTTON_MENU

  #define TFCARD_SPI_FREQ 20000000
  #define TFCARD_CS_PIN   46
  #define TFCARD_MISO_PIN 13
  #define TFCARD_MOSI_PIN 11
  #define TFCARD_SCLK_PIN 12
  #define TFCARD_USE_WIRE1
  #define TFCARD_SPI_HOST SPI2_HOST

  #define LGFX_LOLIN_S3_PRO

  #define ECC_SDA_PIN 9
  #define ECC_SCL_PIN 10



#elif defined( ARDUINO_ESP32_WROVER_KIT )

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("WROVER_KIT SELECTED")
  #endif

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

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("DDUINO32_XS SELECTED")
  #endif

  #define TFT_LED_PIN  22
  #define TFT_DC_PIN   23
  #define TFT_RST_PIN  32
  #define TFT_MOSI_PIN 26
  #define TFT_CLK_PIN 27
  #define NEOPIXEL_PIN 25    // Digital IO pin connected to the NeoPixels.
  #define SPEAKER_PIN  33
  #define SD_ENABLE     0
  #define HAS_BUTTONS
  #define BUTTON_A_PIN  5
  #define BUTTON_B_PIN 18
  #define BUTTON_C_PIN 19


#elif defined ARDUINO_TWATCH_BASE || defined ARDUINO_TWATCH_2020_V1 || defined ARDUINO_TWATCH_2020_V2 // TTGO T-Watch

  #define HAS_BUTTONS
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
    #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
      ECC_PRAGMA_MESSAGE("LILYGO_WATCH_2019_WITH_TOUCH SELECTED")
    #endif
    // #include "board/twatch2019_with_touch.h"
    //#pragma message "Selected LILYGO_WATCH_2019_WITH_TOUCH"
    #define SD_ENABLE            1
    #define LILYGO_WATCH_HAS_BMA423
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BACKLIGHT
    #define LILYGO_WATCH_HAS_BUTTON
    #define HAS_TOUCH
  #elif defined(LILYGO_WATCH_2019_NO_TOUCH)
    #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
      ECC_PRAGMA_MESSAGE("LILYGO_WATCH_2019_NO_TOUCH SELECTED")
    #endif
    //#include "board/twatch2019_with_not_touch.h"
    //#pragma message "Selected LILYGO_WATCH_2019_NO_TOUCH"
    #define SD_ENABLE            1
    #define LILYGO_WATCH_HAS_BMA423
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BACKLIGHT
    #define LILYGO_WATCH_HAS_BUTTON
  #elif defined(LILYGO_WATCH_BLOCK) // should be called "brick" :-)
    #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
      ECC_PRAGMA_MESSAGE("LILYGO_WATCH_BLOCK SELECTED")
    #endif
    //#include "board/twatch_block.h"
    //#pragma message "Selected LILYGO_WATCH_BLOCK"
    #define SD_ENABLE            0
    #undef HAS_SDCARD
    #define HAS_AXP202
    #define LILYGO_WATCH_HAS_BUTTON
    #define LILYGO_WATCH_HAS_MPU6050
    #define HAS_TOUCH
  #else // all LILYGO_WATCH_2020 models
    #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
      ECC_PRAGMA_MESSAGE("LILYGO_WATCH_2020 SELECTED")
    #endif
    // common settings across v1/v2/v3
    #define SD_ENABLE            0
    #undef HAS_SDCARD
    #undef  SPEAKER_PIN
    #define SPEAKER_PIN         -1
    #define HAS_TOUCH
    #if defined(BOARD_HAS_PSRAM)
      //#include "board/twatch2020_v1.h"
      //#pragma message "Defaulting to LILYGO_WATCH_2020_V1"
      #define LILYGO_WATCH_HAS_MOTOR
      #define LILYGO_WATCH_HAS_BMA423
      #define HAS_AXP202
      #define LILYGO_WATCH_HAS_IRREMOTE
      #define HAS_PCF8563
      #define HAS_RTC
      //#define LILYGO_WATCH_HAS_BACKLIGHT
    #else // TODO: implement v3
      //#include "board/twatch2020_v2.h"
      //#pragma message "Defaulting to LILYGO_WATCH_2020_V2"
      #define TOUCH_RST                   4 // !! 2020_v2 only
      #define LILYGO_WATCH_HAS_BMA423
      #define HAS_AXP202
      #define LILYGO_WATCH_HAS_IRREMOTE
      #define LILYGO_WATCH_HAS_BACKLIGHT
      #define LILYGO_WATCH_HAS_GPS
    #endif
  #endif // LILYGO_WATCH_2020 models


#elif defined(ARDUINO_TTGO_T1)
  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("ARDUINO_TTGO_T1 SELECTED")
  #endif
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
  #define HAS_BUTTONS
  #define BUTTON_A_PIN  39
  #define BUTTON_B_PIN  34
  #define BUTTON_C_PIN  35



#elif defined ARDUINO_TTGO_LoRa32_V2 || defined ARDUINO_TTGO_LORA32_V2// || defined ARDUINO_TTGO_LoRa32_V1 || defined ARDUINO_TTGO_LoRa32_v21new

  // ARDUINO_BOARD="TTGO_LoRa32_V2"
  // ARDUINO_VARIANT="ttgo-lora32-v2"

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
    ECC_PRAGMA_MESSAGE("TTGO_LoRa32_v2 SELECTED")
    #undef LGFX_AUTODETECT
  #endif

  // !!! ARDUINO_TTGO_LoRa32_v2 does not exist in boards.txt !!!
  // this profile will only be loaded if a custom entry is inserted in boards.local.txt
  // or if -D ARDUINO_TTGO_LoRa32_v2 is added to the compile flags.
  // Similar boards exist in board.txt but use a different pinout: ARDUINO_TTGO_LoRa32_V1, ARDUINO_TTGO_LoRa32_v21new
  // TFT/OLED display
  #define OLED_SDA        21
  #define OLED_SCL        22
  #define OLED_RST        16
  #define HAS_OLED
  // LoRa device
  #define LORA_MOSI_PIN   27
  #define LORA_MISO_PIN   19
  #define LORA_CS_PIN     18
  #define LORA_SCK_PIN     5
  #define LORA_RST_PIN    12 // attached to EN
  #define LORA_IRQ_PIN    26 // DIO0
  #define LORA_SPI_HOST   VSPI_HOST
  // SD-MMC used as 1bit SD
  #define TFCARD_SCLK_PIN 14
  #define TFCARD_MISO_PIN  2
  #define TFCARD_MOSI_PIN 15
  #define TFCARD_CS_PIN   13
  #define SD_ENABLE        1
  #define TFCARD_USE_WIRE1
  #define TFCARD_SPI_HOST SPI_HOST
  // disabled features
  #define SPEAKER_PIN     -1
  #define BUTTON_A_PIN    -1
  #define BUTTON_B_PIN    -1
  #define BUTTON_C_PIN    -1

/*
  SPI.begin(14, 2, 15);
  if(!SD.begin(13)){
      Serial.println("Card Mount Failed");
      return;
  }
*/


#elif defined( ARDUINO_ODROID_ESP32 )

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("ODROID_ESP32 SELECTED")
  #endif

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
  #define HAS_BUTTONS
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

#elif defined ARDUINO_M5Stick_C || defined ARDUINO_M5STICK_C // M5Stick C

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("M5Stick_C SELECTED")
  #endif

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

  #define HAS_BUTTONS
  #define BUTTON_A_PIN 37
  #define BUTTON_B_PIN 39
  #define BUTTON_C_PIN  -1
  #define SD_ENABLE      0

  #define HAS_RTC
  #define HAS_AXP192
  #define HAS_BM8563

#elif defined ARDUINO_M5Stick_C_Plus || defined ARDUINO_M5STICK_C_PLUS  // M5Stick C Plus

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    #pragma message "M5Stick_C_Plus SELECTED"
  #endif

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

  #define HAS_BUTTONS
  #define BUTTON_A_PIN 37
  #define BUTTON_B_PIN 39
  #define BUTTON_C_PIN  -1
  #define SD_ENABLE      0

  #define HAS_RTC
  #define HAS_AXP192
  #define HAS_BM8563

#elif defined ARDUINO_M5STACK_Core2 || defined ARDUINO_M5STACK_CORE2 // M5Core2

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("M5STACK_Core2 SELECTED")
  #endif

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

  #define TOUCH_INT GPIO_NUM_39
  #define TOUCH_WATERLINE 240 // any Y touch coord higher is in buttons zone
  #define TFCARD_CS_PIN 4
  #define SD_ENABLE     1
  #define SPEAKER_PIN  25
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1


#elif defined ARDUINO_M5STACK_CORES3

  //#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("M5STACK_CoreS3 SELECTED")
  //#endif

  #undef HAS_SPEAKER
  //#define SPEAKER_PIN  -1
  #define HAS_TOUCH
  #define TFCARD_CS_PIN    4
  #define TFCARD_MISO_PIN 35
  #define TFCARD_MOSI_PIN 37
  #define TFCARD_SCLK_PIN 36
  //#define TFCARD_USE_WIRE1
  //#define TFCARD_SPI_HOST SPI2_HOST
  #define TFCARD_SPI_FREQ 25000000

  #define SD_ENABLE     1
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1

  #define USE_SCREENSHOTS

  //       I2C
  //  Chip     Addr   Role
  // ========|======|======
  // GC0308  | 0X21 | Camera
  // LTR553  | 0x23 | Proximity
  // AXP2101 | 0x34 | Power
  // AW88298 | 0x36 | Speaker
  // FT6336  | 0x38 | Touch
  // ES7210  | 0x40 | Audio
  // BM8563  | 0x51 | RTC
  // AW9523  | 0x58 | GPIO Expander
  // BMI270  | 0x69 | Accel/Gyro
  // BMM150  | 0x10 | Magnet

  #define HAS_BM8563
  #define HAS_RTC

  #define HAS_AXP2101


#elif defined ARDUINO_M5Stack_Core_ESP32 || defined ARDUINO_M5STACK_CORE_ESP32 || defined( ARDUINO_M5STACK_FIRE) // m5stack classic/fire

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("M5Stack_Core_ESP32 / M5STACK_FIRE SELECTED")
  #endif

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
  #define HAS_BUTTONS
  #define BUTTON_A_PIN 39
  #define BUTTON_B_PIN 38
  #define BUTTON_C_PIN 37

#elif defined ARDUINO_ESP32_DEV

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("ESP32_DEV SELECTED")
  #endif

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

  #define HAS_BUTTONS
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

#elif defined CONFIG_IDF_TARGET_ESP32S2 // ESP32-S2 basic support

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE
    ECC_PRAGMA_MESSAGE("ESP32S2 SELECTED")
  #endif

  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1
  #define SD_ENABLE     0
  #define SPEAKER_PIN  -1
  #define VSPI FSPI
  #undef HAS_SDCARD


#elif defined ARDUINO_ESP32_S3_BOX // ESP32-S3-BOX support

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_WARN
    ECC_PRAGMA_MESSAGE("ESP32-S3-BOX SELECTED")
  #endif

  #define ECC_LGFX_EXT_CONF "ext_confs/ESP32-S3-Box.hpp"

  //#undef HAS_SDCARD
  //#define SD_ENABLE 0

  #undef GPIO_BTN_INVERT
  #define GPIO_BTN_INVERT false

  #define SD_ENABLE     1
  #define TFCARD_CS_PIN     GPIO_NUM_10
  #define TFCARD_MISO_PIN   GPIO_NUM_13
  #define TFCARD_MOSI_PIN   GPIO_NUM_11
  #define TFCARD_SCLK_PIN   GPIO_NUM_12
  #define TFCARD_USE_WIRE1
  #define TFCARD_SPI_FREQ   40000000
  #define TFCARD_SPI_HOST   SPI2_HOST

  #define I2S_BCK_PIN     GPIO_NUM_17
  #define I2S_LRCK_PIN    GPIO_NUM_47
  #define I2S_DATA_PIN    GPIO_NUM_15
  #define I2S_DATA_IN_PIN GPIO_NUM_16
  #define I2S_MCLK_PIN    GPIO_NUM_2
  #define AUDIO_POWER_PIN GPIO_NUM_46

  //#define HAS_BUTTONS
  #define BUTTON_A_PIN GPIO_NUM_1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1
  #define SPEAKER_PIN  -1

  #define HAS_TOUCH
  #define TOUCH_INT GPIO_NUM_3
  #define TOUCH_WATERLINE 200 // any Y touch coord higher is in buttons zone

  // #if !defined SDA
  //   #define SDA I2C_SDA
  // #endif
  // #if !defined SCL
  //   #define SCL I2C_SCL
  // #endif
  // #define HAS_SPEAKER
  // #define PA_PIN     46 //Audio Amp Power
  // #define MUTE_PIN    1 //MUTE Button


  //#undef USE_SCREENSHOTS
  #undef USE_NVSUTILS


#elif defined ARDUINO_LILYGO_T_DECK

  #define ECC_LGFX_EXT_CONF "ext_confs/Lilygo-S3-T-Deck.hpp"

  #define HAS_TOUCH
  #define TOUCH_INT 16
  #define TOUCH_SDA 18
  #define TOUCH_SCL 8
  #define TOUCH_ADDR 0x5D
  #define TOUCH_WATERLINE 200 // any Y touch coord higher is in buttons zone

  #define TFCARD_CS_PIN 39
  #define SD_ENABLE 1

  //#define HAS_BUTTONS
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1

  #define HAS_TRACKBALL "drivers/T-Deck/trackball.h"
  #define TRACKBALL_UP_PIN    GPIO_NUM_3
  #define TRACKBALL_DOWN_PIN  GPIO_NUM_15
  #define TRACKBALL_LEFT_PIN  GPIO_NUM_1
  #define TRACKBALL_RIGHT_PIN GPIO_NUM_2
  #define TRACKBALL_CLICK_PIN GPIO_NUM_0
  #define TrackBall_Class TDeck_TrackBall_Class

  #define SPEAKER_PIN  -1

  // LoRa mapped pins
  #define LORA_CS_PIN   9
  #define LORA_BUSY_PIN 13
  #define LORA_RST_PIN  17
  #define LORA_DIO1_PIN 45

  #define HAS_KEYBOARD "drivers/T-Deck/keyboard.h"
  #define Keyboard_Class TDeck_Keyboard_Class

  #define KEYBOARD_SDA_PIN GPIO_NUM_18
  #define KEYBOARD_SCL_PIN GPIO_NUM_8
  #define KEYBOARD_INT_PIN GPIO_NUM_46
  #define KEYBOARD_I2C_ADDR 0x55

  // Keyboard_Class( SDA, SCL, KEYBOARD_INT_PIN, KEYBOARD_ADDR, nullptr )

  // #define TDECK_PERI_POWERON 10
  // #define TDECK_BAT_ADC 4

  // #define TDECK_ES7210_MCLK 48
  // #define TDECK_ES7210_LRCK 21
  // #define TDECK_ES7210_SCK 47
  // #define TDECK_ES7210_DIN 14
  //
  // #define TDECK_I2S_WS 5
  // #define TDECK_I2S_BCK 7
  // #define TDECK_I2S_DOUT 6
  //

#elif defined ARDUINO_SUNTON_4827S043

  #define ECC_LGFX_EXT_CONF "ext_confs/Sunton-4827S043.hpp"
  #define HAS_TOUCH

  #define TFCARD_CS_PIN   GPIO_NUM_10
  #define TFCARD_MOSI_PIN GPIO_NUM_11
  #define TFCARD_MISO_PIN GPIO_NUM_13
  #define TFCARD_CLK_PIN  GPIO_NUM_12

  #define SD_ENABLE 1
  #define TFCARD_SPI_FREQ 80000000

  //#define MONITOR_SERIAL Serial
  #define RADAR_SERIAL Serial1
  #define RADAR_RX_PIN GPIO_NUM_17
  #define RADAR_TX_PIN GPIO_NUM_18

  //#define HAS_BUTTONS
  #define BUTTON_A_PIN -1
  #define BUTTON_B_PIN -1
  #define BUTTON_C_PIN -1

  #define SPEAKER_PIN  -1


#else

  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_WARN
    ECC_PRAGMA_MESSAGE("NO BOARD SELECTED")
  #endif

  #pragma message "No know board detected, disabling Buttons and SD"
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

#if defined SPEAKER_PIN && SPEAKER_PIN != -1
  #define HAS_SPEAKER
#else
  #if !defined SPEAKER_PIN
    #define SPEAKER_PIN -1
  #endif
  #undef HAS_SPEAKER
  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_WARN
    //#pragma message "Speaker disabled"
  #endif
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

#define M5STACK
