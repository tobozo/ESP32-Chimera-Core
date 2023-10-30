/*----------------------------------------------------------------------*
 * Arduino Button Library v1.0                                          *
 * Jack Christensen Mar 2012                                            *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/
#pragma once
#define Button_h

#include <stdint.h>

#define DPAD_V_FULL 2
#define DPAD_V_HALF 1
#define DPAD_V_NONE 0

class Button
{
  public:
    Button(uint8_t pin=0xff, uint8_t invert=0, uint32_t dbTime_millis=10);
    uint8_t  read();
    uint8_t  setState(uint8_t);
    uint8_t  isPressed();
    uint8_t  isReleased();
    uint8_t  wasPressed();
    uint8_t  wasReleased();
    uint8_t  pressedFor(uint32_t ms);
    uint8_t  releasedFor(uint32_t ms);
    uint8_t  wasReleasefor(uint32_t ms);
    uint8_t  wasReleaseFor(uint32_t ms) { return wasReleasefor(ms); } // compliance with M5Unified camel casing
    uint32_t lastChange();
    void     setDebounce( uint32_t dbTime_millis) { _dbTime_millis = dbTime_millis; }
#ifdef ARDUINO_ODROID_ESP32
    uint8_t  readAxis();
    uint8_t  isAxisPressed(void);
    uint8_t  wasAxisPressed(void);
#endif
  private:
    uint8_t _pin;           //arduino pin number
    uint8_t _puEnable;      //internal pullup resistor enabled
    uint8_t _invert;        //if 0, interpret high state as pressed, else interpret low state as pressed
    uint8_t _state;         //current button state
    uint8_t _lastState;     //previous button state
    uint8_t _changed;       //state changed since last read
    uint32_t _time;         //time of current state (all times are in ms)
    uint32_t _lastTime;     //time of previous state
    uint32_t _lastChange;   //time of last state change
    uint32_t _dbTime_millis;//debounce time
    uint32_t _pressTime;    //press time
    uint32_t _hold_time;    //hold time call wasreleasefor

    uint8_t _axis;          //state changed since last read
};

