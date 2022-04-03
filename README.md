# ESP32-Chimera-Core Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32-Chimera-Core.svg?)](https://www.ardu-badge.com/ESP32-Chimera-Core)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/tobozo/library/ESP32-Chimera-Core.svg)](https://registry.platformio.org/packages/libraries/tobozo/ESP32-Chimera-Core)
![Arduino Build](https://github.com/tobozo/ESP32-Chimera-Core/actions/workflows/ArduinoBuild.yml/badge.svg)
![Platformio Build](https://github.com/tobozo/ESP32-Chimera-Core/actions/workflows/PlatformioBuild.yml/badge.svg)

![Illustration By Jacopo Ligozzi](https://user-images.githubusercontent.com/1893754/71980273-ed9bb100-321f-11ea-8982-49702af29c9f.png)


This library is a *substitute* of the original [M5Stack](https://github.com/m5stack/M5Stack/) library, with added support for the following devices:

  - [M5Stack Classic](https://m5stack.com/products/basic-core-iot-development-kit)
  - [M5Stack Fire](https://m5stack.com/collections/m5-core/products/fire-iot-development-kit)
  - [M5Stack Core2](https://m5stack.com/products/m5stack-core2-esp32-iot-development-kit)
  - [M5StickC](https://m5stack.com/collections/m5-core/products/stick-c)
  - [Odroid-Go](https://www.hardkernel.com/shop/odroid-go/)
  - [ESP32-S3-Box](https://github.com/espressif/esp-box)
  - [D-Duino-32-XS](https://www.tindie.com/products/lspoplove/dstike-d-duino-32-xs/)
  - [LoLin D32-Pro](https://www.aliexpress.com/item/32883116057.html)
  - [ESP32-Wrover-Kit (v4.1, v4.2, v4.3)](https://www.sparkfun.com/products/14917)
  - [Lilygo TTGO-TS](https://www.banggood.com/LILYGO-TTGO-TS-ESP32-1_44-Inch-TFT-MicroSD-Card-Slot-Speakers-bluetooth-Wifi-Module-p-1273383.html)
  - [Lilygo TTGO-Twatch](http://www.lilygo.cn/prod_view.aspx?TypeId=50053&Id=1290)
  - [Soon] Lilygo TTGO-LoRa32 (v1, v2, v2.1)



It also implements a set of extra features:

  - Zero-config automatic device selection based on the Arduino Boards menu selection
  - Screenshots (BMP, JPG, PNG, GIF)
  - I2C Scanner


GFX implementation is a courtesy of @lovyan03 who (in the first days of this library) did a marvelous work of integrating his
[LovyanGFX](https://github.com/lovyan03/LovyanGFX) library into his own [ESP32-Chimera-Core fork](https://github.com/lovyan03/ESP32-Chimera-Core). Since then, [LovyanGFX](https://github.com/lovyan03/LovyanGFX) has become an external dependency of this library and even given birth to [M5Unified](https://github.com/M5Stack/M5Unified) which has the same spirit as Chimera-Core but focuses on M5 products only.

[LovyanGFX](https://github.com/lovyan03/LovyanGFX) is an optimized display driver maintained by [@lovyan03](https://github.com/lovyan03/),
it brings an average 30% speed improvement compared to other drivers along with more flexibility towards adding custom boards.

This library will forever be beta quality, but the tradeoff is really worth it!

See [LovyanGFX Readme](https://github.com/lovyan03/LovyanGFX/blob/master/README.md) for an overview of those minor changes
along with the new features such as shapes, arcs, polygon drawing and filling, and much more ...


# Usage

Choose between managed install:

  - Get `ESP32-Chimera-Core` from the Arduino Library Manager
  - Let the Arduino Library Manager download the `LovyanGFX` library dependency

or manual install:

  - Download the [latest LovyanGFX display driver](https://github.com/lovyan03/LovyanGFX/releases) and unzip it in the `~/Arduino/libraries` folder.
  - Download the [latest ESP32-Chimera-Core release](https://github.com/tobozo/ESP32-Chimera-Core/releases) and unzip it in the `~/Arduino/libraries` folder.
  - You're done!

# Developers

Replace this:

  ```C
    #include <M5Stack.h>
  ```

by this:

  ```C
    #include <ESP32-Chimera-Core.h>
  ```

You can use `#if defined(_CHIMERA_CORE_)` macros to isolate ESP32-Chimera-Core specific statements.

  ```C
    #if defined(_CHIMERA_CORE_)
      M5.ScreenShot->init( &M5.Lcd, M5STACK_SD );
      M5.ScreenShot->begin();
      M5.ScreenShot->snap();
    #endif

  ```

Automatic board selection is based on the boards.txt definition, so changing the board from the Arduino Tools menu is enough to trigger the detection.
Sketch compilation can eventually be tuned-up to a specific device by using macros.

  ```C
    #if defined(_CHIMERA_CORE_)
      #if defined( ARDUINO_M5Stack_Core_ESP32 )
        #warning M5STACK CLASSIC DETECTED !!
      #elif defined( ARDUINO_M5STACK_FIRE )
        #warning M5STACK FIRE DETECTED !!
      #elif defined( ARDUINO_ODROID_ESP32 )
        #warning ODROID DETECTED !!
      #elif defined( ARDUINO_TTGO_T1 )
        #warning Lilygo TTGO-TS DETECTED !!
      #elif defined ( ARDUINO_ESP32_WROVER_KIT )
        #warning ESP32 WROVER Kit DETECTED !!
      #else
        #warning NOTHING DETECTED !!
      #endif
    #else
      #warning M5Stack legacy core detected
    #endif
  ```


# Credits & Thanks

  - Special thanks to [らびやん (Lovyan03)](https://github.com/lovyan03) for providing strong inspiring support, unconditional kindness and unlimited patience
  - Kudos to [M5Stack](https://github.com/m5stack) for being the creators of the original M5Stack and its legacy library without which this project would not exist.
  - [紅樹　タカオ (Takao Akaki)](https://github.com/mongonta0716)
  - [Nochi](https://github.com/shikarunochi)
  - [こばさん](https://github.com/wakwak-koba)
  - [Illustration By Jacopo Ligozzi](https://commons.wikimedia.org/w/index.php?curid=53514521)
