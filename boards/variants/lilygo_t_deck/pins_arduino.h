#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>
#include "soc/soc_caps.h"

#define USB_VID 0x303a
#define USB_PID 0x1001

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<49)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

static const uint8_t BAT_ADC = 4; // battery voltage

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t SDA = 18;
static const uint8_t SCL = 8;

static const uint8_t SS    = 39;
static const uint8_t MOSI  = 41;
static const uint8_t MISO  = 38;
static const uint8_t SCK   = 40;

static const uint8_t TFT_CS      = 12;
static const uint8_t TFT_DC      = 11;
static const uint8_t TFT_BL      = 42;
static const uint8_t TFT_POWERON = 10;

static const uint8_t RADIO_CS   = 9;
static const uint8_t RADIO_BUSY = 13;
static const uint8_t RADIO_RST  = 17;
static const uint8_t RADIO_DIO1 = 45;

static const uint8_t TOUCH_INT     = 16;
static const uint8_t KEYBOARD_INT  = 46;
static const uint8_t KEYBOARD_ADDR = 0x55;

static const uint8_t TRACKBALL_UP    = 3;
static const uint8_t TRACKBALL_DOWN  = 15;
static const uint8_t TRACKBALL_LEFT  = 1;
static const uint8_t TRACKBALL_RIGHT = 2;
static const uint8_t TRACKBALL_CLICK = 0;

static const uint8_t ES7210_MCLK = 48;
static const uint8_t ES7210_LRCK = 21;
static const uint8_t ES7210_SCK  = 47;
static const uint8_t ES7210_DIN  = 14;

static const uint8_t I2S_WS   = 5;
static const uint8_t I2S_BCK  = 7;
static const uint8_t I2S_DOUT = 6;

// P1
static const uint8_t PIN_43 = 43;
static const uint8_t PIN_44 = 44;
static const uint8_t PIN_18 = 18;
static const uint8_t PIN_17 = 17;
static const uint8_t PIN_21 = 21;
static const uint8_t PIN_16 = 16;

// P2
static const uint8_t PIN_1  = 1;
static const uint8_t PIN_2  = 2;
static const uint8_t PIN_3  = 3;
static const uint8_t PIN_10 = 10;
static const uint8_t PIN_11 = 11;
static const uint8_t PIN_12 = 12;
static const uint8_t PIN_13 = 13;

// Analog
static const uint8_t A0  = 1;
static const uint8_t A1  = 2;
static const uint8_t A2  = 3;
static const uint8_t A9  = 10;
static const uint8_t A10 = 11;
static const uint8_t A11 = 12;
static const uint8_t A12 = 13;
static const uint8_t A15 = 16;
static const uint8_t A16 = 17;
static const uint8_t A17 = 18;


// Touch
static const uint8_t T1  = 1;
static const uint8_t T2  = 2;
static const uint8_t T3  = 3;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;

#endif /* Pins_Arduino_h */


