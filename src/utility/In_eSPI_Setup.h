#define M5STACK

#if defined( ARDUINO_DDUINO32_XS )
  #define TFT_BL         22  // LED back-light
  #define TFT_DC         23
  #define TFT_RST        32
  #define TFT_MOSI       26
  #define TFT_MISO       -1
  #define TFT_SCLK       27
  #define TFT_BGR         0   // Colour order Blue-Green-Red
  #define TFT_RGB         1   // Colour order Red-Green-Blue
  #define TFT_BACKLIGHT_ON HIGH
  #define TFT_WIDTH     240
  #define TFT_HEIGHT    240
  #define TFT_DRIVER 0x7789
  #define SPI_FREQUENCY 40000000
  #define SPI_READ_FREQUENCY 20000000
  #define TFT_SDA_READ      // This option if for ESP32 ONLY, tested with ST7789 display only
  #include "TFT_Drivers/ST7789_DDUINO32-XS_Defines.h"
  #define DDUINO32_XS_LCD_DRIVER
#elif defined ( ARDUINO_ESP32_DEV )
  #define TFT_MISO       25
  #define TFT_MOSI       23
  #define TFT_SCLK       19
  #define TFT_CS         22
  #define TFT_DC         21
  #define TFT_RST        18
  #define TFT_BL          5
  #define TFT_WIDTH     240
  #define TFT_HEIGHT    320
  #define TFT_DRIVER 0x7789
  #define SPI_FREQUENCY 40000000
  #define SPI_READ_FREQUENCY 16000000
  #include "TFT_Drivers/ST7789_WROVER_KIT_LCD_Defines.h"
  #define WROVER_KIT_LCD_DRIVER
#elif defined(ARDUINO_ODROID_ESP32)
  #define TFT_MISO       19
  #define TFT_MOSI       23
  #define TFT_SCLK       18
  #define TFT_CS          5  // Chip select control pin
  #define TFT_DC         21  // Data Command control pin
  #define TFT_RST        -1  // Reset pin (could connect to Arduino RESET pin)
  #define TFT_BL         14  // LED back-light
  #define TFT_DRIVER 0x9341
  #define SPI_FREQUENCY 40000000 // Maximum to use SPIFFS
  #define SPI_READ_FREQUENCY 20000000
  #define TFT_SDA_READ      // This option if for ESP32 ONLY, tested with ST7789 display only
  #include "TFT_Drivers/ILI9341_Defines.h"
  #define ILI9341_DRIVER
#elif defined(ARDUINO_TTGO_T1)
  #define TFT_MISO       -1
  #define TFT_MOSI       23
  #define TFT_SCLK        5
  #define TFT_CS         16
  #define TFT_DC         17
  #define TFT_RST         9
  #define TFT_BL         27
  #define TFT_DRIVER 0x9341
  #define SPI_FREQUENCY 32000000
  #define SPI_READ_FREQUENCY 16000000
  #define TFT_SDA_READ      // This option if for ESP32 ONLY, tested with ST7789 display only
  #include "TFT_Drivers/ILI9341_Defines.h"
  #define ILI9341_DRIVER
#else // M5Stack (default)
  #define TFT_MISO       19
  #define TFT_MOSI       23
  #define TFT_SCLK       18
  #define TFT_CS         14  // Chip select control pin
  #define TFT_DC         27  // Data Command control pin
  #define TFT_RST        33  // Reset pin (could connect to Arduino RESET pin)
  #define TFT_BL         32  // LED back-light (required for M5Stack)
  #define TFT_DRIVER 0x9341
  #define SPI_FREQUENCY 40000000 // Maximum to use SPIFFS
  #define SPI_READ_FREQUENCY 16000000
  #define TFT_SDA_READ      // This option if for ESP32 ONLY, tested with ST7789 display only
  #include "TFT_Drivers/ILI9341_Defines.h"
  #define ILI9341_DRIVER
#endif

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT

//#define USE_HSPI_PORT

// Comment out the following #define if "SPI Transactions" do not need to be
// supported. When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!

// Transaction support is needed to work with SD library but not needed with TFT_SdFat
// Transaction support is required if other SPI devices are connected.

// Transactions are automatically enabled by the library for an ESP32 (to use HAL mutex)
// so changing it here has no effect

// #define SUPPORT_TRANSACTIONS
// These are the pins for all ESP8266 boards
//      Name   GPIO    Function
#define PIN_D0  16  // WAKE
#define PIN_D1   5  // User purpose
#define PIN_D2   4  // User purpose
#define PIN_D3   0  // Low on boot means enter FLASH mode
#define PIN_D4   2  // TXD1 (must be high on boot to go to UART0 FLASH mode)
#define PIN_D5  14  // HSCLK
#define PIN_D6  12  // HMISO
#define PIN_D7  13  // HMOSI  RXD2
#define PIN_D8  15  // HCS    TXD0 (must be low on boot to enter UART0 FLASH mode)
#define PIN_D9   3  //        RXD0
#define PIN_D10  1  //        TXD0

#define PIN_MOSI 8  // SD1
#define PIN_MISO 7  // SD0
#define PIN_SCLK 6  // CLK
#define PIN_HWCS 0  // D3

#define PIN_D11  9  // SD2
#define PIN_D12 10  // SD4