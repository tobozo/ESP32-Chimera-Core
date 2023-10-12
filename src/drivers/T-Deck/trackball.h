#pragma once

// inspired from https://github.com/moononournation/T-Deck

namespace ChimeraCore
{

  namespace TDeck_TrackBall
  {
    static int speed = 2;
    static bool interrupted    = false;
    static int16_t up_count    = 1;
    static int16_t down_count  = 1;
    static int16_t left_count  = 1;
    static int16_t right_count = 1;
    static int16_t click_count = 0;

    static void reset()
    {
      interrupted = false;
      up_count    = 1;
      down_count  = 1;
      left_count  = 1;
      right_count = 1;
      click_count = 0;
    }

    static void init()
    {
      reset();
      speed = 2;
    }

    static void IRAM_ATTR ISR_up()    { interrupted = true; up_count <<= speed;    }
    static void IRAM_ATTR ISR_down()  { interrupted = true; down_count <<= speed;  }
    static void IRAM_ATTR ISR_left()  { interrupted = true; left_count <<= speed;  }
    static void IRAM_ATTR ISR_right() { interrupted = true; right_count <<= speed; }
    static void IRAM_ATTR ISR_click() { interrupted = true; ++click_count;         }
  };


  class TDeck_TrackBall_Class
  {

    typedef void(*fire_event_t)(int16_t x, int16_t y, bool click);  // event callback signature

    public:

      TDeck_TrackBall_Class(
        uint8_t pin_up, uint8_t pin_down,
        uint8_t pin_left, uint8_t pin_right, uint8_t pin_click,
        uint32_t width, uint32_t height,
        fire_event_t fire_event, int16_t x=0, int16_t y=0, int speed=2
      ) :
        _pin_up(pin_up), _pin_down(pin_down),
        _pin_left(pin_left), _pin_right(pin_right), _pin_click(pin_click),
        _width(width), _height(height),
        _fire_event(fire_event),
        _x(x), _y(x)
      {
        init(); // attach interrupts
        setSpeed( speed );
        log_d("Trackball enabled");
      }


      ~TDeck_TrackBall_Class()
      {
        deinit(); // detach interrupts
      }


      int16_t getX()
      {
        update();
        return _x;
      }


      int16_t getY()
      {
        update();
        return _y;
      }


      bool clicked()
      {
        bool has_click = TDeck_TrackBall::click_count;
        update();
        return has_click;
        return false;
      }


      void setSpeed( int speed )
      {
        TDeck_TrackBall::speed = speed;
      }


      void setCoords( int16_t x, int16_t y )
      {
        _x = x;
        _y = y;
      }

      void setCallback( fire_event_t cb )
      {
        _fire_event = cb;
      }


      void update()
      {
        if (TDeck_TrackBall::interrupted) {
          // Serial.printf("x: %d, y: %d, left: %d, right: %d, up: %d, down: %d, click: %d\n", x, y, _left_count, _right_count, _up_count, _down_count, _click_count);
          _x -= TDeck_TrackBall::left_count;
          _x += TDeck_TrackBall::right_count;
          _y -= TDeck_TrackBall::up_count;
          _y += TDeck_TrackBall::down_count;

          if (_x < 0) {
            _x = 0;
          } else if (_x >= _width) {
            _x = _width - 1;
          }
          if (_y < 0) {
            _y = 0;
          } else if (_y >= _height) {
            _y = _height - 1;
          }
          if (TDeck_TrackBall::click_count) {
            if( _fire_event ) _fire_event( _x, _y, true );
            if( _fire_event ) _fire_event( _x, _y, false );
          } else {
            if( _fire_event ) _fire_event( _x, _y, false );
          }
          TDeck_TrackBall::reset();
        }
      }


    private:

      const uint8_t _pin_up;
      const uint8_t _pin_down;
      const uint8_t _pin_left;
      const uint8_t _pin_right;
      const uint8_t _pin_click;

      const uint32_t _width;
      const uint32_t _height;

      fire_event_t _fire_event;

      int16_t _x;
      int16_t _y;

      void init()
      {
        TDeck_TrackBall::init();
        pinMode( _pin_up, INPUT_PULLUP );
        attachInterrupt( _pin_up, TDeck_TrackBall::ISR_up, FALLING );
        pinMode( _pin_down, INPUT_PULLUP );
        attachInterrupt( _pin_down, TDeck_TrackBall::ISR_down, FALLING );
        pinMode( _pin_left, INPUT_PULLUP );
        attachInterrupt( _pin_left, TDeck_TrackBall::ISR_left, FALLING );
        pinMode( _pin_right, INPUT_PULLUP );
        attachInterrupt( _pin_right, TDeck_TrackBall::ISR_right, FALLING );
        pinMode( _pin_click, INPUT_PULLUP );
        attachInterrupt( _pin_click, TDeck_TrackBall::ISR_click, FALLING );
      }


      void deinit()
      {
        detachInterrupt( _pin_up );
        detachInterrupt( _pin_down );
        detachInterrupt( _pin_left );
        detachInterrupt( _pin_right );
        detachInterrupt( _pin_click );
      }

  }; // end class TDeck_TrackBall_Class


} // end namespace ChimeraCore

