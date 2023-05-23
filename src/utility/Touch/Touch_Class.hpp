// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// https://github.com/m5stack/M5Unified

#pragma once


#if __has_include(<LovyanGFX.hpp>)

  #define LGFX_AUTODETECT
  #define LGFX_USE_V1
  #include <LovyanGFX.hpp>

#endif

namespace ChimeraCore
{
  enum touch_state_t
  { none         = 0b0000
  , touch        = 0b0001
  , touch_end    = 0b0010
  , touch_begin  = 0b0011

  , hold         = 0b0101
  , hold_end     = 0b0110
  , hold_begin   = 0b0111

  , flick        = 0b1001
  , flick_end    = 0b1010
  , flick_begin  = 0b1011

  , drag         = 0b1101
  , drag_end     = 0b1110
  , drag_begin   = 0b1111

  , mask_touch   = 0b0001
  , mask_change  = 0b0010
  , mask_holding = 0b0100
  , mask_moving  = 0b1000
  };

  class Touch_Class
  {
  public:
    static constexpr std::size_t TOUCH_MAX_POINTS = 3;

    struct point_t
    {
      std::int16_t x;
      std::int16_t y;
    };

    struct touch_detail_t : public lgfx::touch_point_t
    {
      union
      {
        struct
        {
          std::int16_t prev_x;
          std::int16_t prev_y;
        };
        point_t prev;
      };
      union
      {
        struct
        {
          std::int16_t base_x;
          std::int16_t base_y;
        };
        point_t base;
      };

      std::uint32_t base_msec;
      touch_state_t state = touch_state_t::none;

      inline int deltaX(void) const { return x - prev_x; }
      inline int deltaY(void) const { return y - prev_y; }
      inline int distanceX(void) const { return x - base_x; }
      inline int distanceY(void) const { return y - base_y; }
      inline bool isPressed(void) const { return state & touch_state_t::mask_touch; };
      inline bool wasPressed(void) const { return state == touch_state_t::touch_begin; };
      inline bool wasClicked(void) const { return state == touch_state_t::touch_end; };
      inline bool isReleased(void) const { return !(state & touch_state_t::mask_touch); };
      inline bool wasReleased(void) const { return (state & (touch_state_t::mask_touch | touch_state_t::mask_change)) == touch_state_t::mask_change; };
      inline bool isHolding(void) const { return (state & (touch_state_t::mask_touch | touch_state_t::mask_holding)) == (touch_state_t::mask_touch | touch_state_t::mask_holding); }
      inline bool wasHold(void) const { return state == touch_state_t::hold_begin; }
    };

    /// Get the current number of touchpoints.
    /// @return number of touchpoints.
    inline std::uint8_t getCount(void) const { return _touch_count; }

    ///
    inline const touch_detail_t& getDetail(std::size_t index = 0) const { return _touch_detail[_touch_raw[index].id < TOUCH_MAX_POINTS ? _touch_raw[index].id : 0]; }


    inline const lgfx::touch_point_t& getTouchPointRaw(std::size_t index = 0) const { return _touch_raw[index < _touch_count ? index : 0]; }

    void setHoldThresh(std::uint16_t msec) { _msecHold = msec; }

    void setFlickThresh(std::uint16_t distance) { _flickThresh = distance; }

    bool isEnabled(void) const { return _gfx; }

    void begin(lgfx::LGFX_Device* gfx) { _gfx = gfx; }
    void update(std::uint32_t msec);

  protected:
    std::int32_t _flickThresh = 8;
    std::int32_t _msecHold = 500;
    lgfx::LGFX_Device* _gfx = nullptr;
    touch_detail_t _touch_detail[TOUCH_MAX_POINTS];
    lgfx::touch_point_t _touch_raw[TOUCH_MAX_POINTS];
    std::uint8_t _touch_count;

    bool update_detail(touch_detail_t* dt, std::uint32_t msec, bool pressed, lgfx::touch_point_t* tp);
  };
}
