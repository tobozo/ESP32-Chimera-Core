#include <ESP32-Chimera-Core.h>

#if defined HAS_SDCARD
  #define USE_SDUPDATER
#endif


#define SDU_APP_NAME   "ESP32-S3-Box Test"      // app title for the sd-updater lobby screen
#define SDU_APP_PATH   "/S3BoxTest.bin" // app binary file name on the SD Card (also displayed on the sd-updater lobby screen)
#define SDU_APP_AUTHOR "@tobozo"      // app binary author name for the sd-updater lobby screen
#if defined USE_SDUPDATER
  #include <M5StackUpdater.h> // https://github.com/tobozo/M5Stack-SD-Updater (get it from your library manager)
#endif



void setup()
{

  M5.begin();

  checkSDUpdater( SD, MENU_BIN, 15000, TFCARD_CS_PIN );

}

void loop()
{
  /*
  if( digitalRead( GPIO_NUM_1 ) != lastbtnstate ) {
    lastbtnstate = 1-lastbtnstate;
    log_d("btnstate: %d", lastbtnstate );
  }*/
  //BtnMute->read();
  //if( BtnMute->wasPressed() ) log_e("BtnMute");

   //M5.update();
   //if( M5.BtnA.wasPressed() ) log_e("BtnA");
//   if( M5.BtnB.wasPressed() ) log_e("BtnB");
//   if( M5.BtnC.wasPressed() ) log_e("BtnC");

  //delay(100);

}
