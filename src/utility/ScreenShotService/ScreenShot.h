/*
 *
 * ScreenShotService
 *
 * Copyright 2019 tobozo http://github.com/tobozo
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files ("ScreenShotService"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once
#define __SCREENSHOT_SERVICE_H_


//#pragma message ("Screenshots support enabled !!")
#include "../../M5Display.h"
#include "./JPG/TinyJPEGEncoder.h"
#include "./BMP/TinyBMPEncoder.h"
#include "./PNG/FatPNGEncoder.h" // requires miniz.c patched with TDEFL_LESS_MEMORY=1
#include "./GIF/TinyGIFEncoder.h"

class ScreenShotService {

  public:

    ScreenShotService( M5Display *tft, fs::FS *fileSystem ) : _tft(tft), _fileSystem(fileSystem) { };
    ~ScreenShotService();

    void init();
    bool begin( bool ifPsram = true ); // ram test and allocation
    /*
     *  M5.ScreenShot.snap() => /jpg/screenshot-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("my-namespace") => /jpg/my-namespace-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("/path/to/my-file.jpg") => /path/to/my-file.jpg
     */
    void snap(    const char* name = "screenshot", bool displayAfter = false );
    void snapJPG( const char* name = "screenshot", bool displayAfter = false );
    void snapBMP( const char* name = "screenshot", bool displayAfter = false );
    void snapPNG( const char* name = "screenshot", bool displayAfter = false );
    void snapGIF( const char* name = "screenshot", bool displayAfter = false );
    void setWindow( uint32_t x=0, uint32_t y=0, uint32_t w=0, uint32_t h=0 );
    bool readPixelSuccess  = false; // result of tft pixel read test
    bool jpegCapture       = true; // default yes until tested, BMP capture will be used if not enough ram is available

  private:

    uint32_t _x=0, _y=0, _w=0, _h=0;

    bool        _begun         = false; // prevent begin() from being called more than once
    bool        _inited        = false; // prevent memory to be allocated more than once
    bool        _psram         = false;

    char        fileName[255]  = {0};
    char        folderName[32] = {0};

    M5Display* _tft;
    fs::FS*    _fileSystem;

    void        genFileName( const char* name, const char* extension );
    void        checkFolder( const char* path );
    void        snapAnimation();
    bool        displayCanReadPixels();

    JPEG_Encoder *JPEGEncoder = nullptr;
    BMP_Encoder  *BMPEncoder  = nullptr;
    PNG_Encoder  *PNGEncoder  = nullptr;
    GIF_Encoder  *GIFEncoder  = nullptr;

    uint8_t*    rgbBuffer      = NULL; // used for jpeg only, bmp has his own


}; // end class

