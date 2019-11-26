#ifndef __SCREENSHOT_SERVICE_H_
#define __SCREENSHOT_SERVICE_H_


//#pragma message ("Screenshots support enabled !!")
#include "M5Display.h"
#include "TinyJPEGEncoder.h"
#include "TinyBMPEncoder.h"

class ScreenShotService {

  public:

    ScreenShotService();
    ~ScreenShotService();

    void init( M5Display *tft, fs::FS &fileSystem );
    bool begin(); // ram test and allocation
    /*
     *  M5.ScreenShot.snap() => /jpg/screenshot-YYYY-MM-DD_HH-MM-SS.jpg 
     *  M5.ScreenShot.snap("my-namespace") => /jpg/my-namespace-YYYY-MM-DD_HH-MM-SS.jpg 
     *  M5.ScreenShot.snap("/path/to/my-file.jpg") => /path/to/my-file.jpg
     */
    void snap(    const char* name = "screenshot" );
    void snapJPG( const char* name = "screenshot" );
    void snapBMP( const char* name = "screenshot" );  

    bool readPixelSuccess  = false; // result of tft pixel read test
    bool jpegCapture       = true; // default yes until tested, BMP capture will be used if not enough ram is available

  private:

    bool        _begun         = false; // prevent begin() from being called more than once
    uint8_t*    rgbBuffer      = NULL; // used for jpeg only, bmp has his own
    char        fileName[255]  = {0};
    char        folderName[32] = {0};

    M5Display* _tft;
    fs::FS *   _fileSystem;


    JPEG_Encoder JPEGEncoder;
    BMP_Encoder  BMPEncoder;

    void        genFileName( const char* name, const char* extension );
    void        checkFolder( const char* path );
    bool        displayCanReadPixels();


}; // end class

#endif // __SCREENSHOT_SERVICE_H_
