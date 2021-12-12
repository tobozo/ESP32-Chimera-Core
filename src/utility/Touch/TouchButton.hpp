#pragma once
#define _TOUCH_BUTTON_H

#include "../../M5Display.h"

// LGFX has native TouchButton support since version 1.0.2
#if defined LGFX_VERSION_MAJOR && defined LGFX_VERSION_MINOR && defined LGFX_VERSION_PATCH
  #if ((100*LGFX_VERSION_MAJOR) + (10*LGFX_VERSION_MINOR) + LGFX_VERSION_PATCH) >=102
    #define LGFX_HAS_TOUCHBUTTON
  #endif
#endif

#if defined LGFX_HAS_TOUCHBUTTON
  // inherit from LovyanGFX button support
  #define TouchButton LGFX_Button
#else
  #error "Older version of LovyanGFX detected, please update !!"
#endif // defined LGFX_HAS_TOUCHBUTTON

