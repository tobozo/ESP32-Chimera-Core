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


ScreenShotService::ScreenShotService() {
  // wut ?
}

ScreenShotService::~ScreenShotService() {
  if( _begun ) {
    free( rgbBuffer );
    _begun = false;
  }
}

void ScreenShotService::init( M5Display *tft, fs::FS &fileSystem ) {
  _tft = tft;
  _fileSystem = &fileSystem;
  BMPEncoder.init( tft, fileSystem );
  JPEGEncoder.init( fileSystem );
}


bool ScreenShotService::begin( bool ifPsram ) {
  if( _begun ) return true;
  if( !displayCanReadPixels() ) {
    log_e( "readPixel() test failed, screenshots are disabled" );
    return false;
  }
  if( ifPsram && psramInit() ) {
    log_w("Will attempt to allocate psram for screenshots");
    rgbBuffer = (uint8_t*)ps_calloc( (_tft->width()*_tft->height()*3)+1, sizeof( uint8_t ) );
  } else {
    log_w("Will attempt to allocate ram for screenshots");
    // attempt to allocate anyway, and use BMP stream on failure
    rgbBuffer = (uint8_t*)calloc( (_tft->width()*_tft->height()*3)+1, sizeof( uint8_t ) );
  }
  if( rgbBuffer != NULL ) {
    log_w( "ScreenShot Service can use JPG capture" );
    JPEGEncoder.begin( ifPsram );
  } else {
    log_w( "ScreenShot Service can use BMP capture" );
    jpegCapture = false;
  }
  _begun = true;
  return true;
}




bool ScreenShotService::displayCanReadPixels() {
  uint16_t value_initial = _tft->readPixel( 30,30 );
  uint8_t r = 64, g = 255, b = 128; // scan color
  uint16_t value_in = _tft->color565(r, g, b);
  uint16_t value_out;
  byte testnum = 0;

  log_w( "Testing display#%04x", TFT_DRIVER );

  _tft->drawPixel( 30, 30, value_in ); //  <----- Test color
  value_out = _tft->readPixel( 30,30 );
  log_w( "test#%d: readPixel(as rgb565), expected:0x%04x, got: 0x%04x", testnum++, value_in, value_out );
  if( value_in == value_out ) {
    readPixelSuccess = true;
    _tft->drawPixel( 30, 30, value_initial );
    return true;
  }
  return false;
}


void ScreenShotService::snap( const char* name, bool displayAfter ) {
  if( readPixelSuccess == false ) {
    log_e( "This TFT is unsupported, or it hasn't been tested yet" );
    return;
  }
  if( jpegCapture ) { // enough ram allocated?
    snapJPG( name, displayAfter );
  } else { // BMP capture
    snapBMP( name, displayAfter );
  }
  return;
}


void ScreenShotService::snapJPG( const char* name, bool displayAfter ) {
  if( !jpegCapture ) return;

  genFileName( name, "jpg" );

  _tft->readRectRGB( 0, 0, _tft->width(), _tft->height(), rgbBuffer );

  if ( !JPEGEncoder.encodeToFile( fileName, _tft->width(), _tft->height(), 3 /*3=RGB,4=RGBA*/, rgbBuffer ) ) {
    log_e( "[ERROR] Could not write JPG file to: %s", fileName );
  } else {
    Serial.printf( "Screenshot saved as %s\n", fileName );
    if( displayAfter ) {
      snapAnimation();
      _tft->drawJpgFile( *_fileSystem, fileName, 0, 0, _tft->width(), _tft->height(), 0, 0, JPEG_DIV_NONE );
      delay(5000);
    }
  }
}


void ScreenShotService::snapBMP( const char* name, bool displayAfter ) {
  genFileName( name, "bmp" );
  if( !BMPEncoder.encodeToFile( fileName, _tft->width(), _tft->height() ) )  {
    log_e( "[ERROR] Could not write BMP file to: %s", fileName );
  } else {
    Serial.printf( "Screenshot saved as %s\n", fileName );
    if( displayAfter ) {
      snapAnimation();
      _tft->drawBmpFile( *_fileSystem, fileName, 0, 0 );
      delay(5000);
    }
  }
}


void ScreenShotService::checkFolder( const char* path ) {
  *folderName = {0};
  if( path[0] =='/' ) {
    sprintf( folderName, "%s", path );
  } else {
    sprintf( folderName, "/%s", path );
  }
  if( ! _fileSystem->exists( folderName ) ) {
    log_d( "Creating path %s", folderName );
    _fileSystem->mkdir( folderName );
  } else {
    log_d( "Path %s exists", folderName );
  }
}


void ScreenShotService::genFileName( const char* name, const char* extension ) {
  bool isPrefix = name[0] !='/';
  *fileName = {0};
  if( isPrefix ) {
    checkFolder( extension );
    struct tm now;
    getLocalTime( &now, 0 );
    sprintf( fileName, "%s/%s-%04d-%02d-%02d_%02dh%02dm%02ds.%s", folderName, name, (now.tm_year)+1900,( now.tm_mon)+1, now.tm_mday,now.tm_hour , now.tm_min, now.tm_sec, extension );
    log_d( "has prefix: %s, has folder:%s, has extension: /%s, got fileName: %s", name, folderName, extension, fileName );
  } else {
    log_d( "has path: %s", name );
    sprintf( fileName, "%s", name );
  }
}


void ScreenShotService::snapAnimation() {
  for( byte i = 0; i<16; i++ ) {
    _tft->drawRect(0, 0, _tft->width()-1, _tft->height()-1, WHITE);
    delay(20);
    _tft->drawRect(0, 0, _tft->width()-1, _tft->height()-1, BLACK);
    delay(20);
  }
  _tft->clear();
  delay(150);
}