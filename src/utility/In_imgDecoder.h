#ifndef _IN_IMG_ENCODER
#define _IN_IMG_ENCODER


/*\
 *
 * ESP32-Chimera-Core Img Decoders should all sit here !!
 *
\*/




///// JPEG DECODING
#include "utility/JPEGDecoder/JPEGDecoder.h" // ***altered version of https://github.com/Bodmer/JPEGDecoder


typedef bool (*jpegSketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *data);
__attribute__((unused)) static jpegSketchCallback JpgRenderCallback;


__attribute__((unused)) static void jpgCallBackSetter( jpegSketchCallback sketchCallback ) {
  JpgRenderCallback = sketchCallback;
}


__attribute__((unused)) static uint32_t jpegRender( int xpos, int ypos, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY ) {

  uint16_t jpgWidth  = JpegDec.width;// / (1 << (uint8_t)(JpegDec.scale));
  uint16_t jpgHeight = JpegDec.height;// / (1 << (uint8_t)(JpegDec.scale));

  if( maxWidth == 0 )  maxWidth  = jpgWidth;
  if( maxHeight == 0 ) maxHeight = jpgHeight;

  if (offX >= jpgWidth || offY >= jpgHeight) {
    log_e("Offset Outside of JPEG size");
    return 0;
  }

  size_t jpgMaxWidth = jpgWidth - offX;
  size_t jpgMaxHeight = jpgHeight - offY;

  JpegDec.outWidth  = (jpgMaxWidth > maxWidth)   ? maxWidth  : jpgMaxWidth;
  JpegDec.outHeight = (jpgMaxHeight > maxHeight) ? maxHeight : jpgMaxHeight;

  log_v("Jpeg metrics:\n\twidth=%d\n\theight=%d\n\tjpgMaxWidth=%d\n\tjpgMaxHeight=%d\n\toutWidth=%d\n\toutHeight=%d\n\tmaxWidth=%d\n\tmaxHeight=%d\n\toffX=%d\n\toffY=%d\n",
        jpgWidth, jpgHeight,
        jpgMaxWidth, jpgMaxHeight,
        JpegDec.outWidth, JpegDec.outHeight,
        maxWidth, maxHeight,
        offX, offY
  );

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint16_t max_x = JpegDec.width;
  uint16_t max_y = JpegDec.height;

  int recleft   = offX;
  int rectop    = offY;
  int recright  = offX+JpegDec.outWidth;
  int recbottom = offY+JpegDec.outHeight;
  //uint16_t oL = 0, oR = 0;
  if (recright < offX) {
    return 1;
  }
  if (recleft >= (offX + JpegDec.outWidth)) {
    return 1;
  }
  if (recbottom < offY) {
    return 1;
  }
  if (rectop >= (offY + JpegDec.outHeight)) {
    return 1;
  }

  if( maxWidth == 0 )  maxWidth = JpegDec.width;
  if( maxHeight == 0 ) maxHeight = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = std::min<int>( mcu_w, max_x % mcu_w );
  uint32_t min_h = std::min<int>( mcu_h, max_y % mcu_h );
  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();
  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;
  // Fetch data from the file, decode and display
  while ( JpegDec.read() ) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)
    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
    // check if the image block size needs to be changed for the right edge
    if ( mcu_x + mcu_w <= max_x ) win_w = mcu_w;
    else win_w = min_w;
    // check if the image block size needs to be changed for the bottom edge
    if ( mcu_y + mcu_h <= max_y ) win_h = mcu_h;
    else win_h = min_h;
    // copy pixels into a contiguous block
    if ( win_w != mcu_w ) {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for ( int h = 1; h < win_h; h++ ) {
        p += mcu_w;
        for ( int w = 0; w < win_w; w++ ) {
          *cImg = *( pImg + w + p );
          cImg++;
        }
      }
    }
    // calculate how many pixels must be drawn
    //uint32_t mcu_pixels = win_w * win_h;
    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x/*-offX*/ + win_w ) <= max_x/*maxWidth*/ && ( mcu_y/*-offY*/ + win_h ) <= max_y/*maxHeight*/ ) {

      if( mcu_x-offX < xpos || mcu_y-offY < ypos ) {
        continue;
      }

      if( mcu_x-offX+win_w > xpos+maxWidth || mcu_y-offY+win_h > ypos+maxHeight ) {
        continue;
      }
      if( JpgRenderCallback ) JpgRenderCallback( mcu_x-offX, mcu_y-offY, win_w, win_h, pImg );
    }
    else if ( (mcu_y + win_h) >= maxHeight )
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }
  return millis() - drawTime;
}


__attribute__((unused)) static void jpgRenderer( fs::FS &fs, const char* pFilename, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY ) {
  boolean decoded = JpegDec.decodeFsFile( fs, pFilename );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}

__attribute__((unused)) static void jpgRenderer( Stream *dataSource, uint32_t data_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY ) {
  boolean decoded = JpegDec.decodeStream( dataSource, data_len );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}


__attribute__((unused)) static void jpgRenderer( const uint8_t * jpg_data, uint32_t jpg_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY ) {
  boolean decoded = JpegDec.decodeArray( jpg_data, jpg_len );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}






///// PNG DECODING
#include "utility/pngle.h" // original version of https://github.com/kikuchan/pngle

typedef bool (*pngSketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
__attribute__((unused)) static pngSketchCallback PngRenderCallback;
__attribute__((unused)) static void pngCallBackSetter( pngSketchCallback sketchCallback ) {
  PngRenderCallback = sketchCallback;
}

typedef struct png_draw_params {
  uint16_t x;
  uint16_t y;
  uint16_t maxWidth;
  uint16_t maxHeight;
  uint16_t offX;
  uint16_t offY;
  double scale;
  uint8_t alphaThreshold;
} png_decoder_t;

__attribute__((unused)) static pngle_t *pngle = nullptr;
__attribute__((unused)) static png_decoder_t png;//(png_decoder_t*){0,0,0,0,0,0,0.0,0};

#define pngleColor(c)                                                            \
  (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) |                             \
   ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) |                             \
   ((((uint8_t *)(c))[2] & 0xF8) >> 3))

__attribute__((unused)) static void pngRender(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  png_decoder_t *p = (png_decoder_t *)pngle_get_user_data(pngle);
  uint16_t color = pngleColor(rgba);

  if (x < p->offX || y < p->offY) return ;
  x -= p->offX;
  y -= p->offY;

  // An interlaced file with alpha channel causes disaster, so use 1 here for simplicity
  w = 1;
  h = 1;

  if (p->scale != 1.0) {
    x = (uint32_t)ceil(x * p->scale);
    y = (uint32_t)ceil(y * p->scale);
    w = (uint32_t)ceil(w * p->scale);
    h = (uint32_t)ceil(h * p->scale);
  }

  if (x >= p->maxWidth || y >= p->maxHeight) return ;
  if (x + w >= p->maxWidth) w = p->maxWidth - x;
  if (y + h >= p->maxHeight) h = p->maxHeight - y;

  x += p->x;
  y += p->y;

  if (rgba[3] >= p->alphaThreshold) {
    PngRenderCallback(x, y, w, h, color);
    //p->tft->fillRect(x, y, w, h, color);
  }
}

__attribute__((unused)) static void pngInitCb(pngle_t *pngle, uint32_t w, uint32_t h){
  (void)(pngle);
  if( png.maxWidth == 0 ) png.maxWidth  = w * png.scale;
  if( png.maxHeight== 0 ) png.maxHeight = h * png.scale;
}


__attribute__((unused)) static void pngInit( int x, int y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold ) {
  pngle = pngle_new();
  png.x = x;
  png.y = y;
  png.maxWidth = maxWidth;
  png.maxHeight = maxHeight;
  png.offX = offX;
  png.offY = offY;
  png.scale = scale;
  png.alphaThreshold = alphaThreshold;

  pngle_set_user_data(pngle, &png);
  pngle_set_init_callback(pngle, pngInitCb);
  pngle_set_draw_callback(pngle, pngRender);
  //pngle_set_done_callback(pngle, pngRendered);
}

__attribute__((unused)) static void pngRenderer( const uint8_t * png_data, uint32_t png_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold  ) {
  pngInit( x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold );
  //uint8_t buf[1024];
  //int remain = 0;
  //int len;
  int fed = pngle_feed(pngle, png_data, png_len);
  if (fed < 0) {
    log_e("[pngle error] %s", pngle_error(pngle));
  }
  pngle_destroy(pngle);
}

__attribute__((unused)) static void pngRenderer( Stream *dataSource, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold  ) {
  pngInit( x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold );
  uint8_t buf[1024];
  int remain = 0;
  int len;
  while ((len = dataSource->readBytes(buf + remain, sizeof(buf) - remain)) > 0) {
    int fed = pngle_feed(pngle, buf, remain + len);
    if (fed < 0) {
      log_e("[pngle error] %s", pngle_error(pngle));
      break;
    }
    remain = remain + len - fed;
    if (remain > 0) memmove(buf, buf + fed, remain);
  }

  pngle_destroy(pngle);
}

__attribute__((unused)) static void pngRenderer( fs::FS &fs, const char* pFilename, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold  ) {
  File pFile = fs.open( pFilename );
  if( !pFile ) {
    log_e( "[pngle error] can't open file %s", pFilename );
  }
  pngRenderer( &pFile, x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold  );
}


#endif
