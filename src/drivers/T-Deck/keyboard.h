#pragma once

// inspired from https://github.com/moononournation/T-Deck

#include <Wire.h>

namespace ChimeraCore
{

  namespace TDeck_Keyboard
  {
    static uint32_t key_scan_interval_ms = 200;
    static bool keyboard_interrupted = false;
    static void IRAM_ATTR ISR_key() { keyboard_interrupted = true; }
  }


  class TDeck_Keyboard_Class
  {

    typedef void(*fire_event_t)(uint8_t key); // event callback signature

    public:

      TDeck_Keyboard_Class( TwoWire* bus, uint8_t dev_addr, fire_event_t fire_event )
        : _bus(bus), _dev_addr(dev_addr), _fire_event(fire_event) { init(); }
      TDeck_Keyboard_Class( TwoWire* bus, uint8_t dev_addr, gpio_num_t pin_int, fire_event_t fire_event )
        : _bus(bus), _pin_int(pin_int), _dev_addr(dev_addr), _fire_event(fire_event) { _has_interrupt = true; init(); }
      TDeck_Keyboard_Class( gpio_num_t pin_sda, gpio_num_t pin_scl, uint8_t dev_addr, fire_event_t fire_event )
        : _pin_sda(pin_sda), _pin_scl(pin_scl), _dev_addr(dev_addr), _fire_event(fire_event) { _bus=&Wire1; _has_pins=true; init();  }
      TDeck_Keyboard_Class( gpio_num_t pin_sda, gpio_num_t pin_scl, gpio_num_t pin_int, uint8_t dev_addr, fire_event_t fire_event )
        : _pin_sda(pin_sda), _pin_scl(pin_scl), _pin_int(pin_int), _dev_addr(dev_addr), _fire_event(fire_event) { _bus=&Wire; _has_pins=true; _has_interrupt = true; init();  }

      ~TDeck_Keyboard_Class()
      {
        deinit(); // detach interrupts
      }


      bool init()
      {
        // Check keyboard
        if( ! _bus ) {
          Serial.println("No bus instance!");
          return false;
        }
        if( _has_pins ) {
          _bus->begin(_pin_sda, _pin_scl/*, _i2c_freq*/);
          log_d("Starting I2C bus sda=%d,scl=%d", _pin_sda, _pin_scl );
        }
        _bus->requestFrom(_dev_addr, (uint8_t)1);
        if (_bus->read() == -1) {
          Serial.println("LILYGO Keyboad not online!");
          return false;
        }
        if( _has_interrupt ) {
          log_d("Attaching interrupt");
          pinMode(_pin_int, INPUT_PULLUP);
          attachInterrupt(_pin_int, TDeck_Keyboard::ISR_key, FALLING);
        }
        return true;
      }


      void deinit()
      {
        if( _has_interrupt ) {
          detachInterrupt( _pin_int );
        }
      }


      void setInterval( uint32_t interval_ms )
      {
        TDeck_Keyboard::key_scan_interval_ms = interval_ms;
      }


      void setCallback( fire_event_t cb )
      {
        _fire_event = cb;
      }


      char get_key()
      {
        if( _bus ) {
          _bus->requestFrom(_dev_addr, (uint8_t)1); // Read key value from esp32c3
          if (_bus->available() > 0) {
            return _bus->read();
          }
        }
        return 0;
      }


      void update()
      {
        if( _has_interrupt ) {
          static uint32_t next_key_scan_ms = 0;
          if (TDeck_Keyboard::keyboard_interrupted || (millis() > next_key_scan_ms)) {
            char key;
            do {
              key = get_key();
              if( key !=0 ) {
                if( _fire_event ) _fire_event( key );
                next_key_scan_ms = millis() + TDeck_Keyboard::key_scan_interval_ms;
              }
            } while( key !=0 );
            TDeck_Keyboard::keyboard_interrupted = false;
          }
        } else {
          int key = get_key();
          if(key > 0) {
            if( _fire_event ) _fire_event( key );
          }
        }
      }

    private:

      TwoWire* _bus;

      gpio_num_t _pin_sda;
      gpio_num_t _pin_scl;
      gpio_num_t _pin_int;

      uint8_t _dev_addr = -1;

      fire_event_t _fire_event;

      uint32_t _i2c_freq = 800000UL;

      bool _has_interrupt = false;
      bool _has_pins = false;


  }; // end class TDeck_Keyboard_Class

} // end namespace ChimeraCore
