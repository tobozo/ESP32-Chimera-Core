
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


bool ScreenShotService::begin() {
  if( _begun ) return true;
  if( !displayCanReadPixels() ) {
    log_e( "readPixel() test failed, screenshots are disabled" );
    return false;
  }
  if( psramInit() ) {
    rgbBuffer = (uint8_t*)ps_calloc( _tft->width()*_tft->height()*3, sizeof( uint8_t ) );
  } else {
    // attempt to allocate anyway, and use BMP stream on failure
    rgbBuffer = (uint8_t*)calloc( _tft->width()*_tft->height()*3, sizeof( uint8_t ) );
  }
  if( rgbBuffer != NULL ) {
    log_i( "ScreenShot Service can use JPG capture" );
    JPEGEncoder.begin();
  } else {
    log_i( "ScreenShot Service can use BMP capture" );
    jpegCapture = false;
  }
  _begun = true;
  return true;
}


bool ScreenShotService::displayCanReadPixels() {
  uint16_t value_in = TFT_BLUE;
  _tft->fillRect( 10, 10, 50, 50, value_in ); //  <----- Test color
  uint16_t value_out = _tft->readPixel( 30,30 );
  _tft->clear();
  log_i( "readPixel() test, expected:0x%02x, got: 0x%02x", value_in, value_out );
  if( value_in == value_out ) {
    readPixelSuccess = true;
    return true;
  }
  return false;
}


void ScreenShotService::snap( const char* name ) {
  if( readPixelSuccess == false ) {
    log_e( "This TFT is unsupported, or it hasn't been tested yet" );
    return;
  }
  if( jpegCapture ) { // enough ram allocated?
    snapJPG( name );
  } else { // BMP capture
    snapBMP( name );
  }
  return;
}


void ScreenShotService::snapJPG( const char* name ) {
  if( !jpegCapture ) return;
  _tft->readRectRGB( 0, 0, _tft->width(), _tft->height(), rgbBuffer );
  genFileName( name, "jpg" );
  if ( !JPEGEncoder.encodeToFile( fileName, _tft->width(), _tft->height(), 3 /*3=RGB,4=RGBA*/, rgbBuffer ) ) {
    log_e( "[ERROR] Could not write JPG file to: %s", fileName );
  } else {
    Serial.printf( "Screenshot saved as %s\n", fileName );
  }
}


void ScreenShotService::snapBMP( const char* name ) {
  genFileName( name, "bmp" );
  if( !BMPEncoder.encodeToFile( fileName, _tft->width(), _tft->height() ) )  {
    log_e( "[ERROR] Could not write BMP file to: %s", fileName );
  } else {
    Serial.printf( "Screenshot saved as %s\n", fileName );
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





