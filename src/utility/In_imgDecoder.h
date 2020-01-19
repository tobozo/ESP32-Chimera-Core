#ifndef _IN_IMG_ENCODER
#define _IN_IMG_ENCODER

// generalistic aliases for all decoders

typedef int32_t (*getWidthCallBack)(void);
typedef int32_t (*getHeightCallBack)(void);
typedef uint16_t (*getRgb565CallBack)(uint8_t r, uint8_t g, uint8_t b);
typedef void (*pushColorCallBack)(uint32_t color, uint16_t len);
typedef void (*pushColorArrayCallBack)(uint16_t* buf, uint16_t len);
typedef void (*setWindowCallBack)(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
typedef void (*setWriteColorCallBack)(uint16_t color);

typedef void (*transationStartCallBack)(void);
typedef void (*transactionEndCallBack)(void);

__attribute__((unused)) static getWidthCallBack  GetWidthCallBack;
__attribute__((unused)) static getHeightCallBack GetHeightCallBack;
__attribute__((unused)) static getRgb565CallBack GetRgb565CallBack;
__attribute__((unused)) static pushColorArrayCallBack PushColorArrayCallBack;
__attribute__((unused)) static setWindowCallBack SetWindowCallBack;
__attribute__((unused)) static transationStartCallBack TransationStartCallBack;
__attribute__((unused)) static transactionEndCallBack TransactionEndCallBack;
__attribute__((unused)) static void setWidthGetter( getWidthCallBack cb ) { GetWidthCallBack = cb; }
__attribute__((unused)) static void setHeightGetter( getHeightCallBack cb ) { GetHeightCallBack = cb; }
__attribute__((unused)) static void setRgb565Converter( getRgb565CallBack cb ) { GetRgb565CallBack = cb; }
__attribute__((unused)) static void setColorWriterArray( pushColorArrayCallBack cb ) { PushColorArrayCallBack = cb; }
__attribute__((unused)) static void setWindowSetter( setWindowCallBack cb ) { SetWindowCallBack = cb; }
__attribute__((unused)) static void setTransactionStarter( transationStartCallBack cb ) { TransationStartCallBack = cb; }
__attribute__((unused)) static void setTransactionEnder( transactionEndCallBack cb ) { TransactionEndCallBack = cb; }

__attribute__((unused)) static bool islegacyJpegDecoder = false;


/*\
 *
 * ESP32-Chimera-Core Img Decoders should all sit here !!
 *
\*/






////////////////////////////////////////////////
///// JPEG DECODING (Legacy version) ///////////
////////////////////////////////////////////////
#include "rom/tjpgd.h"

typedef bool (*jpegLegacySketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *data);
__attribute__((unused)) static jpegLegacySketchCallback JpgLegacyRenderCallback;
__attribute__((unused)) static void jpgLegacyCallBackSetter( jpegLegacySketchCallback sketchCallback ) { JpgLegacyRenderCallback = sketchCallback; }

#define jpgColor(c)                                                            \
  (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) |                             \
   ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) |                             \
   ((((uint8_t *)(c))[2] & 0xF8) >> 3))


typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t maxWidth;
  uint16_t maxHeight;
  uint16_t offX;
  uint16_t offY;
  jpeg_div_t scale;
  const void *src;
  size_t len;
  size_t index;
  //M5Display *tft;
  uint16_t outWidth;
  uint16_t outHeight;
} jpg_file_decoder_t;

static const char *jd_errors/* = {"Succeeded",
                           "Interrupted by output function",
                           "Device error or wrong termination of input stream",
                           "Insufficient memory pool for the image",
                           "Insufficient stream input buffer",
                           "Parameter error",
                           "Data format error",
                           "Right format but not supported",
                           "Not supported JPEG standard"}*/;

__attribute__((unused))
static uint32_t jpgLegacyRead(JDEC *decoder, uint8_t *buf, uint32_t len) {
  jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
  if (buf) {
    memcpy(buf, (const uint8_t *)jpeg->src + jpeg->index, len);
  }
  jpeg->index += len;
  return len;
}

__attribute__((unused))
static uint32_t jpgLegacyReadFile(JDEC *decoder, uint8_t *buf, uint32_t len) {
  jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
  File *file = (File *)jpeg->src;
  if (buf) {
    return file->read(buf, len);
  } else {
    file->seek(len, SeekCur);
  }
  return len;
}

__attribute__((unused))
static uint32_t jpgLegacyReadStream(JDEC *decoder, uint8_t *buf, uint32_t len) {
  jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;

  Stream *stream = (Stream *)jpeg->src;
  return stream->readBytes(buf, len);

}
__attribute__((unused))
static uint32_t jpgLegacyColorPush(JDEC *decoder, void *bitmap, JRECT *rect) {
  jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
  uint16_t x = rect->left;
  uint16_t y = rect->top;
  uint16_t w = rect->right + 1 - x;
  uint16_t h = rect->bottom + 1 - y;
  uint16_t oL = 0, oR = 0;
  uint8_t *data = (uint8_t *)bitmap;

  if (rect->right < jpeg->offX) {
    return 1;
  }
  if (rect->left >= (jpeg->offX + jpeg->outWidth)) {
    return 1;
  }
  if (rect->bottom < jpeg->offY) {
    return 1;
  }
  if (rect->top >= (jpeg->offY + jpeg->outHeight)) {
    return 1;
  }
  if (rect->top < jpeg->offY) {
    uint16_t linesToSkip = jpeg->offY - rect->top;
    data += linesToSkip * w * 3;
    h -= linesToSkip;
    y += linesToSkip;
  }
  if (rect->bottom >= (jpeg->offY + jpeg->outHeight)) {
    uint16_t linesToSkip = (rect->bottom + 1) - (jpeg->offY + jpeg->outHeight);
    h -= linesToSkip;
  }
  if (rect->left < jpeg->offX) {
    oL = jpeg->offX - rect->left;
  }
  if (rect->right >= (jpeg->offX + jpeg->outWidth)) {
    oR = (rect->right + 1) - (jpeg->offX + jpeg->outWidth);
  }

  uint16_t pixBuf[32];
  uint8_t pixIndex = 0;
  uint16_t line;
  if( TransationStartCallBack ) TransationStartCallBack();
  SetWindowCallBack((int32_t)x - jpeg->offX + jpeg->x + oL,
                    (int32_t)y - jpeg->offY + jpeg->y,
                    (int32_t)x - jpeg->offX + jpeg->x + oL + w - (oL + oR) - 1,
                    (int32_t)y - jpeg->offY + jpeg->y + h - 1);

  log_w("Setwindow( [%d,%d], [%d,%d])",
    (int32_t)x - jpeg->offX + jpeg->x + oL,
    (int32_t)y - jpeg->offY + jpeg->y,
    (int32_t)x - jpeg->offX + jpeg->x + oL + w - (oL + oR) - 1,
    (int32_t)y - jpeg->offY + jpeg->y + h - 1
  );

  while (h--) {
    data += 3 * oL;
    line = w - (oL + oR);
    while (line--) {
      pixBuf[pixIndex++] = jpgColor(data);
      //pixBuf[pixIndex++] = GetRgb565CallBack(data[0], data[1], data[2]);
      data += 3;
      if (pixIndex == 32) {
        PushColorArrayCallBack( pixBuf, pixIndex );
        pixIndex = 0;
      }
    }
    data += 3 * oR;
  }
  if (pixIndex) {
    PushColorArrayCallBack( pixBuf, pixIndex );
  }
  if( TransactionEndCallBack ) TransactionEndCallBack();
  return 1;
}

__attribute__((unused))
static bool jpgLegacyDecode(jpg_file_decoder_t *jpeg, uint32_t (*reader)(JDEC *, uint8_t *, uint32_t)) {
  static uint8_t work[3100];
  JDEC decoder;

  JRESULT jres = jd_prepare(&decoder, reader, work, 3100, jpeg);
  if (jres != JDR_OK) {
    log_e("jd_prepare failed! %s", jd_errors[jres]);
    return false;
  }

  uint16_t jpgWidth = decoder.width / (1 << (uint8_t)(jpeg->scale));
  uint16_t jpgHeight = decoder.height / (1 << (uint8_t)(jpeg->scale));

  if (jpeg->offX >= jpgWidth || jpeg->offY >= jpgHeight) {
    log_e("Offset Outside of JPEG size");
    return false;
  }

  size_t jpgMaxWidth = jpgWidth - jpeg->offX;
  size_t jpgMaxHeight = jpgHeight - jpeg->offY;

  jpeg->outWidth =
      (jpgMaxWidth > jpeg->maxWidth) ? jpeg->maxWidth : jpgMaxWidth;
  jpeg->outHeight =
      (jpgMaxHeight > jpeg->maxHeight) ? jpeg->maxHeight : jpgMaxHeight;



  jres = jd_decomp(&decoder, jpgLegacyColorPush, (uint8_t)jpeg->scale);


  if (jres != JDR_OK) {
    log_e("jd_decomp failed! %s", jd_errors[jres]);
    return false;
  }

  return true;
}



__attribute__((unused))
static void jpgLegacyRenderer( const uint8_t * jpg_data, uint32_t jpg_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {

  if ((x + maxWidth) > GetWidthCallBack() || (y + maxHeight) > GetHeightCallBack()) {
    log_e("Bad dimensions given");
    return;
  }

  jpg_file_decoder_t jpeg;

  if (!maxWidth) {
    maxWidth = GetWidthCallBack() - x;
    log_w("Adjusted maxWidth to %d", maxWidth );
  }
  if (!maxHeight) {
    maxHeight = GetHeightCallBack() - y;
    log_w("Adjusted maxHeight to %d", maxWidth );
  }

  jpeg.src = jpg_data;
  jpeg.len = jpg_len;
  jpeg.index = 0;
  jpeg.x = x;
  jpeg.y = y;
  jpeg.maxWidth = maxWidth;
  jpeg.maxHeight = maxHeight;
  jpeg.offX = offX;
  jpeg.offY = offY;
  jpeg.scale = scale;

  jpgLegacyDecode(&jpeg, jpgLegacyRead);
}

__attribute__((unused))
static void jpgLegacyRenderer( Stream *dataSource, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {
  if ((x + maxWidth) > GetWidthCallBack() || (y + maxHeight) > GetHeightCallBack()) {
    log_e("Bad dimensions given");
    return;
  }

  jpg_file_decoder_t jpeg;

  if (!maxWidth) {
    maxWidth = GetWidthCallBack() - x;
  }
  if (!maxHeight) {
    maxHeight = GetHeightCallBack() - y;
  }

  jpeg.src = dataSource;
  jpeg.index = 0;
  jpeg.x = x;
  jpeg.y = y;
  jpeg.maxWidth = maxWidth;
  jpeg.maxHeight = maxHeight;
  jpeg.offX = offX;
  jpeg.offY = offY;
  jpeg.scale = scale;
  jpgLegacyDecode(&jpeg, jpgLegacyReadStream);

}
__attribute__((unused))
static void jpgLegacyRenderer(fs::FS &fs, const char* pFilename, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {
  if ((x + maxWidth) > GetWidthCallBack() || (y + maxHeight) > GetHeightCallBack()) {
    log_e("Bad dimensions given");
    return;
  }

  File file = fs.open(pFilename);
  if (!file) {
    log_e("Failed to open file for reading");
    return;
  }

  jpg_file_decoder_t jpeg;

  if (!maxWidth) {
    maxWidth = GetWidthCallBack() - x;
  }
  if (!maxHeight) {
    maxHeight = GetHeightCallBack() - y;
  }

  jpeg.src = &file;
  jpeg.len = file.size();
  jpeg.index = 0;
  jpeg.x = x;
  jpeg.y = y;
  jpeg.maxWidth = maxWidth;
  jpeg.maxHeight = maxHeight;
  jpeg.offX = offX;
  jpeg.offY = offY;
  jpeg.scale = scale;
  //jpeg.tft = this;

  jpgLegacyDecode(&jpeg, jpgLegacyReadFile);

  file.close();
}




////////////////////////////////////////////////
///// JPEG DECODING (Bodmer's version) /////////
////////////////////////////////////////////////
#include "utility/JPEGDecoder/JPEGDecoder.h" // ***altered version of https://github.com/Bodmer/JPEGDecoder


typedef bool (*jpegSketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *data);
__attribute__((unused)) static jpegSketchCallback JpgRenderCallback;
__attribute__((unused))
static void jpgCallBackSetter( jpegSketchCallback sketchCallback ) {
  JpgRenderCallback = sketchCallback;
}


__attribute__((unused))
static uint32_t jpegRender( int xpos, int ypos, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale ) {

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


__attribute__((unused))
static void jpgRenderer( fs::FS &fs, const char* pFilename, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {
  boolean decoded = JpegDec.decodeFsFile( fs, pFilename );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY, scale );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}

__attribute__((unused))
static void jpgRenderer( Stream *dataSource, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {
  boolean decoded = JpegDec.decodeStream( dataSource, 0 );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY, scale );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}


__attribute__((unused))
static void jpgRenderer( const uint8_t * jpg_data, uint32_t jpg_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale  ) {
  boolean decoded = JpegDec.decodeArray( jpg_data, jpg_len );
  if ( decoded ) {
    jpegRender( x, y, maxWidth, maxHeight, offX, offY, scale );
  } else {
    Serial.println( "Jpeg file format not supported!" );
  }
}





////////////////////////////////////////////////
///// PNG DECODING (Kikuchan's version) ////////
////////////////////////////////////////////////
#include "utility/pngle.h" // original version of https://github.com/kikuchan/pngle

typedef bool (*pngSketchCallback)(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
__attribute__((unused)) static pngSketchCallback PngRenderCallback;
__attribute__((unused))
static void pngCallBackSetter( pngSketchCallback sketchCallback ) {
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
  uint16_t bgcolor;
  uint8_t bgcolor8[3];
  uint32_t zonecount;
} png_decoder_t;

__attribute__((unused)) static pngle_t *pngle = nullptr;
__attribute__((unused)) static png_decoder_t png;//(png_decoder_t*){0,0,0,0,0,0,0.0,0};

#define pngleColor(c)                                                            \
  (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) |                             \
   ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) |                             \
   ((((uint8_t *)(c))[2] & 0xF8) >> 3))


#define R 0
#define G 1
#define B 2

__attribute__((unused))
static void pngRender(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  png_decoder_t *p = (png_decoder_t *)pngle_get_user_data(pngle);

  if( rgba[3] < p->alphaThreshold ) {
    if( rgba[3] == 0 ) {
      rgba[R] = p->bgcolor8[R];
      rgba[G] = p->bgcolor8[G];
      rgba[B] = p->bgcolor8[B];
    } else {
      // alpha color blend with background
      rgba[R] = (p->bgcolor8[R] * p->alphaThreshold + rgba[R] * (255 - p->alphaThreshold)) / 255;
      rgba[G] = (p->bgcolor8[G] * p->alphaThreshold + rgba[G] * (255 - p->alphaThreshold)) / 255;
      rgba[B] = (p->bgcolor8[B] * p->alphaThreshold + rgba[B] * (255 - p->alphaThreshold)) / 255;
    }
  }

  uint16_t color = pngleColor(rgba);

  if (x < p->offX || y < p->offY) return;
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

  //if (rgba[3] >= p->alphaThreshold) {
    PngRenderCallback(x, y, w, h, color);
    //p->tft->fillRect(x, y, w, h, color);
  //}
}

__attribute__((unused))
static void pngInitCb(pngle_t *pngle, uint32_t w, uint32_t h){
  (void)(pngle);
  if( png.maxWidth == 0 ) png.maxWidth  = w * png.scale;
  if( png.maxHeight== 0 ) png.maxHeight = h * png.scale;
}


__attribute__((unused))
static void pngInit( int x, int y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold, uint16_t bgcolor ) {
  pngle = pngle_new();
  png.x = x;
  png.y = y;
  png.maxWidth = maxWidth;
  png.maxHeight = maxHeight;
  png.offX = offX;
  png.offY = offY;
  png.scale = scale;
  png.alphaThreshold = alphaThreshold;
  png.bgcolor = bgcolor;

/*
  png.bgcolor8[R] = (bgcolor & 0xF800) >> 8;  // r: >> 11 << 3
  png.bgcolor8[G] = (bgcolor & 0x001F) << 3;  // g: >> 0 << 3
  png.bgcolor8[B] = (bgcolor & 0x07E0) >> 3;  // b: >> 5 << 2

  log_w("alpha1 [%d, %d, %d]", png.bgcolor8[R], png.bgcolor8[G], png.bgcolor8[B] );
*/
  png.bgcolor8[B] = (((bgcolor & 0x1f)* 527) + 23) >> 6;
  png.bgcolor8[G] = ((((bgcolor >> 5) & 0x3f)* 259) + 33) >> 6;
  png.bgcolor8[R] = ((((bgcolor >> 11) & 0x1f)* 527) + 23) >> 6;

  log_w("background rgb color: [%d, %d, %d]", png.bgcolor8[R], png.bgcolor8[G], png.bgcolor8[B] );

/*
  uint8_t VH = bgcolor & 0xFF;;
  uint8_t VL = bgcolor >> 8;

  png.bgcolor8[B] = (((VH & 0x1F) * 527) + 23) >> 6;
  //b = 0;
  png.bgcolor8[R] = ((((VL >> 3) & 0x1F) * 527) + 23) >> 6;
  //r = 0;
  png.bgcolor8[G] = ((((((VH & 0xF0) >> 5) | ((VL & 0x0F) << 3)) & 0x3F) * 259) + 33) >> 6;

  log_w("alpha3 [%d, %d, %d]", png.bgcolor8[R], png.bgcolor8[G], png.bgcolor8[B] );
*/

  png.zonecount = 0;

  pngle_set_user_data(pngle, &png);
  pngle_set_init_callback(pngle, pngInitCb);
  pngle_set_draw_callback(pngle, pngRender);
  //pngle_set_done_callback(pngle, pngRendered);
}

__attribute__((unused))
static void pngRenderer( const uint8_t * png_data, uint32_t png_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold, uint16_t bgcolor ) {
  pngInit( x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold, bgcolor );
  //uint8_t buf[1024];
  //int remain = 0;
  //int len;
  int fed = pngle_feed(pngle, png_data, png_len);
  if (fed < 0) {
    log_e("[pngle error] %s", pngle_error(pngle));
  }
  pngle_destroy(pngle);
}

__attribute__((unused))
static void pngRenderer( Stream *dataSource, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold, uint16_t bgcolor  ) {
  pngInit( x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold, bgcolor );
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

__attribute__((unused))
static void pngRenderer( fs::FS &fs, const char* pFilename, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold, uint16_t bgcolor ) {
  File pFile = fs.open( pFilename );
  if( !pFile ) {
    log_e( "[pngle error] can't open file %s", pFilename );
  }
  pngRenderer( &pFile, x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold, bgcolor  );
}


#endif
