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


#include <FS.h>
#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "./AVI/AviMjpegEncoder.hpp"


template <typename GFX> // GFX can be either LGFX_Sprite or LGFX_Device
static void defaultReadRect( void* src, int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t* buf )
{
  assert(src);
  GFX* gfx = (GFX*)src;
  if( gfx->getColorDepth() >= 24 ) gfx->readRectRGB( x, y, w, h, buf );
  else gfx->readRect( x, y, w, h, (lgfx::rgb565_t*)buf );
};




class ScreenShotService
{

  public:

    ScreenShotService( LGFX *tft,           fs::FS *fileSystem ) : _fileSystem(fileSystem) { setSource(tft); };
    ScreenShotService( LGFX_Sprite *sprite, fs::FS *fileSystem ) : _fileSystem(fileSystem) { setSource(sprite); };
    ~ScreenShotService();

    void init();
    bool begin( bool ifPsram = true ); // ram test and allocation
    /*\
     *  M5.ScreenShot.snap() => /jpg/screenshot-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("my-namespace") => /jpg/my-namespace-YYYY-MM-DD_HH-MM-SS.jpg
     *  M5.ScreenShot.snap("/path/to/my-file.jpg") => /path/to/my-file.jpg
    \*/
    void snap(    const char* name = "screenshot", bool displayAfter = false );
    void snapBMP( const char* name = "screenshot", bool displayAfter = false );
    void snapPNG( const char* name = "screenshot", bool displayAfter = false );
    void snapGIF( const char* name = "screenshot", bool displayAfter = false );
    size_t snapQOI( const char* name = "screenshot", bool displayAfter = false );
    size_t snapQOI( Stream* stream );

    #if defined HAS_JPEGENC
      /*\
       *  AVI_Params_t aviparams = new AVI_Params_t( &SD, "/out.avi", fps, use_buffer, use_index_file );
       *  while( capturing ) {
       *    animate( &canvas );                                          // animate canvas
       *    if( params->ready ) M5.ScreenShot.snapAVI( aviparams );      // add frame
       *  }
       *  if( params->ready ) M5.ScreenShot.snapAVI( aviparams, true );  // finalize
      \*/
      void snapAVI( AVI_Params_t *params, bool finalize=false );
      void snapJPG( const char* name = "screenshot", bool displayAfter = false );
    #else
      #define TinyJPEG_DEPRECATE_MSG "AVI/JPG capture is disabled, include @bitbanks2's <JPEGENC.h> to enable it"
      [[deprecated(TinyJPEG_DEPRECATE_MSG)]]
      inline void snapAVI( void *params, bool finalize=false ) { log_n(TinyJPEG_DEPRECATE_MSG); };
      [[deprecated(TinyJPEG_DEPRECATE_MSG)]]
      void snapJPG( const char* name = "screenshot", bool displayAfter = false ) { log_n(TinyJPEG_DEPRECATE_MSG); };
    #endif

    template <typename GFX>
    void setSource( GFX *src )
    {
      _is_sprite = std::is_same<GFX,LGFX_Sprite>::value;
      _src = (LGFX*)src;
      _w = src->width();
      _h = src->height();
    };
    void setFs( fs::FS *fileSystem ) { _fileSystem = fileSystem; };
    void setWindow( uint32_t x=0, uint32_t y=0, uint32_t w=0, uint32_t h=0 );
    bool readPixelSuccess  = false; // result of tft pixel read test
    bool jpegCapture       = false; // default no (needs JPEGENC library, PNG capture is default
    const char* getFileName() { return fileName; }

  private:

    uint32_t _x=0, _y=0, _w=0, _h=0;    // capture window

    bool        _begun         = false; // prevent begin() from being called more than once
    bool        _inited        = false; // prevent memory to be allocated more than once
    bool        _psram         = false;
    bool        _is_sprite     = false;

    LGFX*       _src;
    fs::FS*     _fileSystem;

    char        fileName[255]  = {0};
    char        folderName[32] = {0};

    void        genFileName( const char* name, const char* extension );
    void        checkFolder( const char* path );
    void        snapAnimation();
    bool        displayCanReadPixels();

    uint8_t*    screenShotBuffer = NULL; // used by AVI and JPG encoders
    uint32_t    screenShotBufSize = 0;


}; // end class

#include "./BMP/TinyBMPEncoder.hpp"
#include "./PNG/FatPNGEncoder.hpp"
#include "./GIF/TinyGIFEncoder.hpp"
#include "./QOI/QOIEncoder.hpp"

