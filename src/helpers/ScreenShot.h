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

#ifndef __SCREENSHOT_SERVICE_H_
#define __SCREENSHOT_SERVICE_H_


//#pragma message ("Screenshots support enabled !!")
#include "../M5Display.h"
#include "TinyJPEGEncoder.h"
#include "TinyBMPEncoder.h"

class ScreenShotService {

  public:

    ScreenShotService();
    ~ScreenShotService();

    void init( M5Display *tft, fs::FS &fileSystem );
    bool begin( bool ifPsram = true ); // ram test and allocation
    /*
     *  M5.ScreenShot.snap() => /jpg/screenshot-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("my-namespace") => /jpg/my-namespace-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("/path/to/my-file.jpg") => /path/to/my-file.jpg
     */
    void snap(    const char* name = "screenshot", bool displayAfter = false );
    void snapJPG( const char* name = "screenshot", bool displayAfter = false );
    void snapBMP( const char* name = "screenshot", bool displayAfter = false );

    bool readPixelSuccess  = false; // result of tft pixel read test
    bool jpegCapture       = true; // default yes until tested, BMP capture will be used if not enough ram is available

  private:

    bool        _begun         = false; // prevent begin() from being called more than once

    char        fileName[255]  = {0};
    char        folderName[32] = {0};

    M5Display* _tft;
    fs::FS *   _fileSystem;

    void        genFileName( const char* name, const char* extension );
    void        checkFolder( const char* path );
    void        snapAnimation();
    bool        displayCanReadPixels();

    JPEG_Encoder JPEGEncoder;
    BMP_Encoder  BMPEncoder;

    uint8_t*    rgbBuffer      = NULL; // used for jpeg only, bmp has his own


}; // end class

#endif // __SCREENSHOT_SERVICE_H_
