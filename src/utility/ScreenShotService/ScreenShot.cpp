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

#include "ScreenShot.hpp"


ScreenShotService::~ScreenShotService()
{
  if( _begun ) {
    if( screenShotBuffer) free( screenShotBuffer );
    _begun = false;
  }
}

void ScreenShotService::init()
{
  if( _inited ) return;
  setWindow(0, 0, _src->width(), _src->height() ); // default capture mode is full screen
  _inited = true;
}


void ScreenShotService::setWindow( uint32_t x, uint32_t y, uint32_t w, uint32_t h )
{

  if( x >= _src->width()-1 )  x = 0;
  if( y >= _src->height()-1 ) y = 0;
  if( x+w > _src->width() ) w = _src->width()-x;
  if( y+h > _src->height() )  h = _src->height()-y;
  _x = x; _y = y;
  _w = w; _h = h;
}


bool ScreenShotService::begin( bool ifPsram )
{
  if( !_inited ) {
    //return false;
    init();
  }
  if( _begun ) return true;
  if( !displayCanReadPixels() ) {
    log_i( "readPixel() test failed, screenshots are disabled" );
    return false;
  }
  _psram = ifPsram;
  _begun = true;
  return true;
}


bool ScreenShotService::displayCanReadPixels()
{
  uint16_t value_initial = _src->readPixel( 30,30 );
  uint8_t r = 64, g = 255, b = 128; // scan color
  uint16_t value_in = _src->color565(r, g, b);
  uint16_t value_out;
  __attribute__((unused)) byte testnum = 0;
  log_d( "Testing display readpixel" );
  //  log_w( "Testing display#%04x", TFT_DRIVER );
  _src->drawPixel( 30, 30, value_in ); //  <----- Test color
  value_out = _src->readPixel( 30,30 );
  log_d( "test#%d: readPixel(as rgb565), expected:0x%04x, got: 0x%04x", testnum++, value_in, value_out );
  if( value_in == value_out ) {
    readPixelSuccess = true;
    _src->drawPixel( 30, 30, value_initial );
    return true;
  }
  return false;
}


void ScreenShotService::snap( const char* name, bool displayAfter )
{
  if( readPixelSuccess == false ) {
    log_i( "[ERROR] This TFT is unsupported, or it hasn't been tested yet" );
    return;
  }
  if( jpegCapture ) { // enough ram allocated?
    snapJPG( name, displayAfter );
  } else { // PNG capture
    snapPNG( name, displayAfter );
  }
  return;
}



#if defined HAS_JPEGENC

  // for direct to disk (no jpeg buffer) image encoding
  struct JPGENC_IMG_Proxy_t
  {
    static fs::FS* fs;
    static void* open(const char *filename) { static auto f = fs->open(filename, FILE_WRITE); return (void *)&f; }
    static void  close(JPEGFILE *p) { File *f = (File *)p->fHandle; if (f) f->close(); }
    static int32_t read(JPEGFILE *p, uint8_t *buffer, int32_t length) { File *f = (File *)p->fHandle; return f->read(buffer, length); }
    static int32_t write(JPEGFILE *p, uint8_t *buffer, int32_t length) { File *f = (File *)p->fHandle; return f->write(buffer, length); }
    static int32_t seek(JPEGFILE *p, int32_t position) { File *f = (File *)p->fHandle; return f->seek(position); }
  };

  fs::FS* JPGENC_IMG_Proxy_t::fs = nullptr;


  void ScreenShotService::snapJPG( const char* name, bool displayAfter )
  {
    if( !_begun || !_inited ) return;
    assert( name );
    assert( _fileSystem );

    JPEG jpg;
    JPEGENCODE jpe;
    JPGENC_IMG_Proxy_t::fs = _fileSystem;
    JPGENC_IMG_Proxy_t IMG_Proxy = JPGENC_IMG_Proxy_t();
    bool use_buffer = true;
    auto color_depth = _src->getColorDepth();
    const int bytes_per_pixel = color_depth/8;
    uint8_t jpeg_pixel_format = color_depth >= 24 ? JPEG_PIXEL_RGB888 : JPEG_PIXEL_RGB565;
    uint8_t ucMCU[64*bytes_per_pixel];
    int iMCUCount, rc, i;
    size_t iDataSize;
    [[maybe_unused]] uint32_t time_start = millis();

    genFileName( name, "jpg" ); // store computed file name in 'fileName' (char[255])
    screenShotBufSize = _w*_h*.5; // will use buffered writes (faster), estimated max jpeg bytes = (w*h)/2, i.e. max 4bits per pixel

    if( _psram && psramInit() ) {
      screenShotBuffer = (uint8_t*)ps_calloc( screenShotBufSize, sizeof( uint8_t ) );
    } else {
      screenShotBuffer = (uint8_t*)calloc( screenShotBufSize, sizeof( uint8_t ) );
      _psram = false;
    }
    if( screenShotBuffer ) {
      log_v( "ScreenShot Service can use JPG capture" );
    } else {
      log_i( "Not enough ram to use jpeg screenshot" );
      use_buffer = false;
    }

    if( use_buffer && screenShotBufSize > 0 ) {
      rc = jpg.open(screenShotBuffer, screenShotBufSize);
    } else {
      rc = jpg.open(fileName, IMG_Proxy.open, IMG_Proxy.close, IMG_Proxy.read, IMG_Proxy.write, IMG_Proxy.seek);
    }

    if (rc != JPEG_SUCCESS) {
      log_e("Failed to create jpeg object");
      goto _end;
    }

    rc = jpg.encodeBegin(&jpe, _w, _h, jpeg_pixel_format, JPEG_SUBSAMPLE_444, JPEG_Q_HIGH);

    if (rc != JPEG_SUCCESS) {
      log_e("Failed to initiate jpeg encoding");
      goto _end;
    }

    memset(ucMCU, 0, sizeof(ucMCU)); // zerofill MCU, not really required as we'll fill it soon with readRect()
    iMCUCount = ((_w + jpe.cx-1)/ jpe.cx) * ((_h + jpe.cy-1) / jpe.cy);

    for (i=0; i<iMCUCount && rc == JPEG_SUCCESS; i++) {
      if( color_depth >= 24 ) _src->readRectRGB( _x+jpe.x, _y+jpe.y, jpe.cx, jpe.cy, ucMCU );
      else _src->readRect( _x+jpe.x, _y+jpe.y, jpe.cx, jpe.cy, (lgfx::rgb565_t*)ucMCU );
      rc = jpg.addMCU(&jpe, ucMCU, 8*bytes_per_pixel);
    }

    iDataSize = jpg.close();

    if( use_buffer ) {
      auto file = _fileSystem->open(fileName, "w" );
      if( ! file ) {
        log_e("Can't open %d for writing", fileName );
        goto _end;
      }
      file.write( screenShotBuffer, iDataSize );
      file.close();
    }

    log_i( "[SUCCESS] Screenshot saved as %s (%d bytes) in %u ms", fileName, iDataSize, millis()-time_start);

    if( displayAfter ) {
      snapAnimation();
      _src->drawJpgFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }

    _end:

    if( screenShotBuffer ) free( screenShotBuffer );
  }



  void ScreenShotService::snapAVI( AVI_Params_t *params, bool finalize )
  {
    assert( params );
    assert( _fileSystem || params->fs );
    assert( _src ); // source display/sprite

    if( params->fs ) _fileSystem = params->fs;
    else params->fs = _fileSystem;

    if( !_begun || !_inited ) return;
    _psram = _psram && psramInit();

    auto color_depth = _is_sprite ? ((LGFX_Sprite*)_src)->getColorDepth() : ((LGFX*)_src)->getColorDepth();
    auto readRect    = _is_sprite ? defaultReadRect<LGFX_Sprite>          : defaultReadRect<LGFX>;

    if( params->size.w>0 && params->size.h > 0 ) {
      _w = params->size.w;
      _h = params->size.h;
    } else {
      _w = _is_sprite ? ((LGFX_Sprite*)_src)->width()  : ((LGFX*)_src)->width();
      _h = _is_sprite ? ((LGFX_Sprite*)_src)->height() : ((LGFX*)_src)->height();
      params->size.w = _w;
      params->size.h = _h;
    }

    assert( _w > 0 );
    assert( _h > 0 );

    //static fs::File aviFile;
    static AviMjpegEncoder* AVIEncoder = nullptr;
    static JPGENC_AVI_Proxy_t AVI_Proxy; // callbacks for JPEGENC

    const int bytes_per_pixel = color_depth/8;
    uint8_t jpeg_pixel_format = color_depth >= 24 ? JPEG_PIXEL_RGB888 : JPEG_PIXEL_RGB565;

    JPEG jpg;
    JPEGENCODE jpe;
    uint16_t MCUSize = 64*bytes_per_pixel;
    static uint8_t *ucMCU = nullptr;// (uint8_t*)calloc(MCUSize, sizeof(uint8_t));
    int iMCUCount, rc, i;
    char framename[32];

    if( AVIEncoder == nullptr ) {
      if( !_fileSystem ) {
        log_e("No file/stream provided");
        goto _end;
      }

      if( params->path == "" ) {
        genFileName( "out", "avi" );
        params->path = String( fileName );
      }

      AVIEncoder = new AviMjpegEncoder( params );

      if( !params->ready ) {
        log_e("Unable to create AVIEncoder instance");
        goto _end;
      }

      JPGENC_AVI_Proxy_t::aviEncoder = AVIEncoder;
      screenShotBufSize = params->use_buffer ? _w*_h*.5 : 0; // estimated max jpeg bytes = (w*h)/2, i.e. max 4bits per pixel
      if( params->use_buffer && screenShotBufSize > 0 ) { // allocate some memory for the buffer
        screenShotBuffer = _psram ? (uint8_t*)ps_calloc( screenShotBufSize, sizeof(uint8_t) ) : (uint8_t*)calloc( screenShotBufSize, sizeof(uint8_t) );
        if( ! screenShotBuffer ) {
          log_e("Failed to allocate jpeg buffer (%d bytes), JPEGENC will use unbuffered (slower) writes instead", screenShotBufSize);
          screenShotBufSize = 0;
          params->use_buffer = false;
          _psram = false;
        }
      }

      log_d("Capture params: %s[%dx%d]@%dbpp (jpg_bufsize=%d, %s)", _is_sprite?"sprite":"display", _w, _h, color_depth, screenShotBufSize, _psram?"PSRAM":"Heap" );

      ucMCU = (uint8_t*)calloc(MCUSize, sizeof(uint8_t));
      if( !ucMCU ) {
        log_e("Can't allocate %d byte for JPEG MCUs", MCUSize );
        goto _end;
      }
    } // end if( AVIEncoder == nullptr )

    if( finalize ) {
      if( AVIEncoder ) AVIEncoder->finalize();
      _end:
      if( ucMCU ) free( ucMCU );
      if( AVIEncoder ) {
        delete AVIEncoder;
        AVIEncoder = nullptr;
      }
      if( screenShotBuffer ) free( screenShotBuffer );
      params->ready = false;
      return;
    }

    if( params->use_buffer && screenShotBufSize > 0 ) {
      rc = jpg.open(screenShotBuffer, screenShotBufSize);
    } else {
      sprintf( framename, "frame-%d", AVIEncoder->framesCount()+1 );
      rc = jpg.open(framename, AVI_Proxy.open, AVI_Proxy.close, AVI_Proxy.read, AVI_Proxy.write, AVI_Proxy.seek);
    }

    if (rc != JPEG_SUCCESS) {
      log_e("Failed to create jpeg object");
      goto _end;
    }

    rc = jpg.encodeBegin(&jpe, _w, _h, jpeg_pixel_format, JPEG_SUBSAMPLE_444, JPEG_Q_BEST);

    if (rc != JPEG_SUCCESS) {
      log_e("Failed to initiate jpeg encoding");
      goto _end;
    }

    iMCUCount = ((_w + jpe.cx-1)/ jpe.cx) * ((_h + jpe.cy-1) / jpe.cy);

    for (i=0; i<iMCUCount && rc == JPEG_SUCCESS; i++) {
      readRect( _src, _x+jpe.x, _y+jpe.y, jpe.cx, jpe.cy, ucMCU );
      rc = jpg.addMCU(&jpe, ucMCU, 8*bytes_per_pixel);
    }

    size_t iDataSize = jpg.close();

    if( params->use_buffer && screenShotBufSize > 0 ) { // if using buffered writes, send the jpeg buffer
      AVIEncoder->addJpegFrame( screenShotBuffer, iDataSize );
    }
  }

#endif





void ScreenShotService::snapBMP( const char* name, bool displayAfter )
{
  genFileName( name, "bmp" );
  [[maybe_unused]] uint32_t time_start = millis();
  BMP_Encoder *BMPEncoder = new BMP_Encoder( _src, _fileSystem );
  if( !BMPEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write BMP file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    log_i( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, outFile.size(), millis()-time_start);
    outFile.close();
    if( displayAfter ) {
      snapAnimation();
      _src->drawBmpFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }
  }
  delete BMPEncoder;
}


void ScreenShotService::snapPNG( const char* name, bool displayAfter )
{
  genFileName( name, "png" );
  [[maybe_unused]] uint32_t time_start = millis();
  PNG_Encoder* PNGEncoder = new PNG_Encoder( _src, _fileSystem );
  PNGEncoder->init();
  if( !PNGEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write PNG file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    log_i( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, outFile.size(), millis()-time_start);
    outFile.close();
    if( displayAfter ) {
      snapAnimation();
      _src->drawPngFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }
  }
  delete PNGEncoder;
}


void ScreenShotService::snapGIF( const char* name, bool displayAfter )
{
  genFileName( name, "gif" );
  [[maybe_unused]] uint32_t time_start = millis();
  GIF_Encoder* GIFEncoder  = new GIF_Encoder( _src, _fileSystem );
  if( !GIFEncoder->encodeToFile( fileName, _x, _y, _w, _h ) )  {
    log_e( "[ERROR] Could not write GIF file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    log_i( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, outFile.size(), millis()-time_start);
    outFile.close();
  }
  delete GIFEncoder;
}


size_t ScreenShotService::snapQOI( Stream* stream )
{
  assert( stream );
  [[maybe_unused]] uint32_t time_start = millis();
  QOI_Encoder* QOIEncoder = new QOI_Encoder( _src, _is_sprite );
  size_t qoi_size = QOIEncoder->encodeToStream( stream, _x, _y, _w, _h );
  if( qoi_size <= 0 )  {
    log_e( "[ERROR] Could not stream QOI data ([%d:%d]@[%dx%d] is sprite: %s)", _x, _y, _w, _h, _is_sprite?"true":"false" );
  } else {
    log_v( "[SUCCESS] Screenshot streamed (%d bytes). Total time %u ms", qoi_size, millis()-time_start);
  }
  delete QOIEncoder;
  return qoi_size;
}


size_t ScreenShotService::snapQOI( const char* name, bool displayAfter )
{
  genFileName( name, "qoi" );
  [[maybe_unused]] uint32_t time_start = millis();
  QOI_Encoder* QOIEncoder = new QOI_Encoder( _src, _is_sprite );
  QOIEncoder->setFileSystem(_fileSystem);
  size_t qoi_size = QOIEncoder->encodeToFile( fileName, _x, _y, _w, _h );
  if( qoi_size <= 0 )  {
    log_e( "[ERROR] Could not write QOI file to: %s", fileName );
  } else {
    fs::File outFile = _fileSystem->open( fileName );
    log_i( "[SUCCESS] Screenshot saved as %s (%d bytes). Total time %u ms", fileName, outFile.size(), millis()-time_start);
    outFile.close();
    if( displayAfter ) {
      snapAnimation();
      _src->drawQoiFile( *_fileSystem, fileName, _x, _y );
      delay(5000);
    }
  }
  delete QOIEncoder;
  return qoi_size;
}


void ScreenShotService::checkFolder( const char* path )
{
  assert(path);
  assert(_fileSystem);
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
  assert(name);
  assert(_fileSystem);
  bool isPrefix = name[0] !='/';
  *fileName = {0};
  if( isPrefix ) {
    checkFolder( extension );
    struct tm now;
    getLocalTime( &now, 0 );
    sprintf( fileName, "%s/%s-%04d-%02d-%02d_%02dh%02dm%02ds.%s", folderName, name, (now.tm_year)+1900,( now.tm_mon)+1, now.tm_mday,now.tm_hour , now.tm_min, now.tm_sec, extension );
  } else {
    sprintf( fileName, "%s", name );
  }
  log_v( "generated path: %s", fileName );
}


void ScreenShotService::snapAnimation()
{
  for( byte i = 0; i<16; i++ ) {
    _src->drawRect(0, 0, _src->width()-1, _src->height()-1, TFT_WHITE);
    delay(20);
    _src->drawRect(0, 0, _src->width()-1, _src->height()-1, TFT_BLACK);
    delay(20);
  }
  _src->clear();
  delay(150);
}
