#if !defined _BUTTON_TOUCH_READER_H_
#define _BUTTON_TOUCH_READER_H_

#include "../../../Config.h"
#include "../../../M5Display.h"
#include "../../common/Button/Button.h"

// M5Core2 'Physical' Button emulation using off-display touch zone
class ButtonTouchReader
{
  public:

    ButtonTouchReader( M5Display* gfx, Button* btns[3] ){
      _gfx = gfx;
      _btns[0] = btns[0];
      _btns[1] = btns[1];
      _btns[2] = btns[2];
      button_zone_width = ((_gfx->width()+1)/3); // 1/3rd of the screen per button
      button_marginleft = 15; // dead space in pixels before and after each button to prevent overlap
      button_marginright = button_zone_width-button_marginleft;
      log_d("Touch zone set to w(%d), ml(%d), mr(%d)", button_zone_width, button_marginleft, button_marginright );
    };

    uint16_t number = 0;
    uint16_t button_zone_width = ((320+1)/3); // 1/3rd of the screen per button
    uint16_t button_marginleft = 15; // dead space in pixels before and after each button to prevent overlap
    uint16_t button_marginright = button_zone_width-button_marginleft;

    lgfx::touch_point_t tp; // touch coordinates will be stored there

    void read() {
      int button_num = -1;
      number = _gfx->getTouch(&tp, 1);

      if( number>0 ) {
        if( tp.y >= _gfx->height() ) {
          uint16_t tpxmod = tp.x%button_zone_width; // convert touch position to button relative coords
          // assign a button number only if touch position is between button margins
          if( tpxmod > button_marginleft && tpxmod < button_marginright ) {
            // button press !!
            button_num = tp.x / button_zone_width;
          }
        } else {
          // _gfx->fillCircle(tp.x, tp.y, 5, TFT_WHITE);
        }
      }
      toggleButtons( button_num );
    }

    void toggleButtons( uint8_t btnEnabled ) {
      for( uint8_t i=0; i<3; i++ ) {
        if( i == btnEnabled ) _btns[i]->setState( 0xff );
        else  _btns[i]->setState( 0x00 );
      }
    }

  private:
    M5Display* _gfx;
    Button* _btns[3];

};

#endif
