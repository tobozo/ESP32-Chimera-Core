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

#include "ScreenShot.h"

/*
ScreenShotService::ScreenShotService( M5Display *tft, fs::FS &fileSystem )
{
  // wut ?
}
*/
ScreenShotService::~ScreenShotService()
{
  if( _begun ) {
    free( rgbBuffer );
    _begun = false;
  }
}

void ScreenShotService::init()
{
  if( _inited ) return;
  // default capture mode is full screen
  setWindow(0, 0, _tft->width(), _tft->height() );
  _inited = true;
}


void ScreenShotService::setWindow( uint32_t x, uint32_t y, uint32_t w, uint32_t h )
{

  if( x >= _tft->width()-1 ) {
    x = 0;
  }
  if( y >= _tft->height()-1 ) {
    y = 0;
  }
  if( x+w > _tft->width() ) {
    w = _tft->width()-x;
  }
  if( y+h > _tft->height() ) {
    h = _tft->height()-y;
  }
  _x = x;
  _y = y;
  _w = w;
  _h = h;
}


bool ScreenShotService::begin( bool ifPsram )
{
  if( !_inited ) {
    //return false;
    init();
  }
  if( _begun ) return true;
  if( !displayCanReadPixels() ) {
    log_n( "readPixel() test failed, screenshots are disabled" );
    return false;
  }
  _psram = ifPsram;
  _begun = true;
  return true;
}


bool ScreenShotService::displayCanReadPixels()
{
  uint16_t value_initial = _tft->readPixel( 30,30 );
  uint8_t r = 64, g = 255, b = 128; // scan color
  uint16_t value_in = _tft->color565(r, g, b);
  uint16_t value_out;
  __attribute__((unused)) byte testnum = 0;
  log_d( "Testing display readpixel" );
  //  log_w( "Testing display#%04x", TFT_DRIVER );
  _tft->drawPixel( 30, 30, value_in ); //  <----- Test color
  value_out = _tft->readPixel( 30,30 );
  log_d( "test#%d: readPixel(as rgb565), expected:0x%04x, got: 0x%04x", testnum++, value_in, value_out );
  if( value_in == value_out ) {
    readPixelSuccess = true;
    _tft->drawPixel( 30, 30, value_initial );
    return true;
  }
  return false;
}


void ScreenShotService::snap( const char* name, bool displayAfter )
{
  if( readPixelSuccess == false ) {
    log_n( "[ERROR] This TFT is unsupported, or it hasn't been tested yet" );
    return;
  }
  if( jpegCapture ) { // enough ram allocated?
    snapJPG( name, displayAfter );
  } else { // BMP capture
    snapBMP( name, displayAfter );
  }
  return;
}

// this is lame
static uint32_t jpeg_encoder_xoffset = 0;
static uint32_t jpeg_encoder_yoffset = 0;
static uint32_t jpeg_encoder_w       = 0;

static void jpeg_encoder_callback(uint32_t y, uint32_t h, unsigned char* rgbBuffer, void* device)
{
  auto tft = (M5Display*)device;
  tft->readRectRGB( jpeg_encoder_xoffset, jpeg_encoder_yoffset+y, jpeg_encoder_w, h, rgbBuffer );
}


void ScreenShotService::snapJPG( const char* name, bool displayAfter )
{
  if( !jpegCapture ) return;
  jpeg_encoder_xoffset = _x;
  jpeg_encoder_yoffset = _y;
  jpeg_encoder_w = _w;
  genFileName( name, "jpg" );
  uint32_t time_start = millis();

  if( _psram && psramInit() ) {
    log_v("Will attempt to allocate psram for screenshots");
    rgbBuffer = (uint8_t*)ps_calloc( (_w*8*3)+1, sizeof( uint8_t ) );
  } else {
    log_v("Will attempt to allocate ram for screenshots");
    rgbBuffer = (uint8_t*)calloc( (_w*8*3)+1, sizeof( uint8_t ) );
    _psram = false;
  }
  if( rgbBuffer != NULL ) {
    log_v( "ScreenShot Service can use JPG capture" );
  } else {
    log_n( "Not enough ram to use jpeg screenshot" );
    jpegCapture = false;
    return;
  }

  JPEGEncoder = new JPEG_Encoder( _fileSystem );
  JPEGEncoder->begin( _psram );

  if ( !JPEGEncoder->encodeToFile( fileName, _w, _h, 3 /*3=RGB,4=RGBA*/, rgbBuffer, &jpeg_encoder_callback, _tft ) ) {
    log_n( "[ERROR] Could not write JPG file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    size_t fileSize = outFile.size();
    outFile.close();
    log_n( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, fileSize, millis()-time_start);
    if( displayAfter ) {
      snapAnimation();
      _tft->drawJpgFile( *_fileSystem, fileName, _x, _y, _w, _h, 0, 0 );
      delay(5000);
    }
  }
  delete JPEGEncoder;
  free( rgbBuffer );
}


void ScreenShotService::snapBMP( const char* name, bool displayAfter )
{
  genFileName( name, "bmp" );
  uint32_t time_start = millis();
  BMPEncoder = new BMP_Encoder( _tft, _fileSystem );
  if( !BMPEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write BMP file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    size_t fileSize = outFile.size();
    outFile.close();
    log_n( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, fileSize, millis()-time_start);
    if( displayAfter ) {
      snapAnimation();
      _tft->drawBmpFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }
  }
  delete BMPEncoder;
}


void ScreenShotService::snapPNG( const char* name, bool displayAfter )
{
  genFileName( name, "png" );
  uint32_t time_start = millis();
  PNGEncoder = new PNG_Encoder( _tft, _fileSystem );
  PNGEncoder->init();
  if( !PNGEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write PNG file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    size_t fileSize = outFile.size();
    outFile.close();
    log_n( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, fileSize, millis()-time_start);
    if( displayAfter ) {
      snapAnimation();
      _tft->drawPngFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }
  }
  delete PNGEncoder;
}


void ScreenShotService::snapGIF( const char* name, bool displayAfter )
{
  genFileName( name, "gif" );
  uint32_t time_start = millis();
  GIFEncoder  = new GIF_Encoder( _tft, _fileSystem );
  if( !GIFEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write GIF file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    size_t fileSize = outFile.size();
    outFile.close();
    log_n( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, fileSize, millis()-time_start);
    //if( displayAfter ) {
    //  snapAnimation();
    //  _tft->drawGifFile( *_fileSystem, fileName, 0, 0 );
    //  delay(5000);
    //}
  }
  delete GIFEncoder;
}


void ScreenShotService::checkFolder( const char* path )
{
  *folderName = {0};
  sprintf( folderName, "%s%s", path[0] =='/'?"":"/", path );
  if( ! _fileSystem->exists( folderName ) ) {
    log_v( "Creating path %s", folderName );
    _fileSystem->mkdir( folderName );
  } else {
    log_v( "Path %s exists", folderName );
  }
}


void ScreenShotService::genFileName( const char* name, const char* extension )
{
  bool isPrefix = name[0] !='/';
  *fileName = {0};
  if( isPrefix ) {
    checkFolder( extension );
    struct tm now;
    getLocalTime( &now, 0 );
    sprintf( fileName, "%s/%s-%04d-%02d-%02d_%02dh%02dm%02ds.%s", folderName, name, (now.tm_year)+1900,( now.tm_mon)+1, now.tm_mday,now.tm_hour , now.tm_min, now.tm_sec, extension );
    log_v( "has prefix: %s, has folder:%s, has extension: /%s, got fileName: %s", name, folderName, extension, fileName );
  } else {
    log_v( "has path: %s", name );
    sprintf( fileName, "%s", name );
  }
}


void ScreenShotService::snapAnimation()
{
  for( byte i = 0; i<16; i++ ) {
    _tft->drawRect(0, 0, _tft->width()-1, _tft->height()-1, TFT_WHITE);
    delay(20);
    _tft->drawRect(0, 0, _tft->width()-1, _tft->height()-1, TFT_BLACK);
    delay(20);
  }
  _tft->clear();
  delay(150);
}
