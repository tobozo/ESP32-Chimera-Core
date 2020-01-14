#include "M5Display.h"

#define BLK_PWM_CHANNEL 7 // LEDC_CHANNEL_7

M5Display::M5Display() : TFT_eSPI() {}

void M5Display::begin() {
  TFT_eSPI::begin();
#ifdef ARDUINO_ODROID_ESP32
  setRotation(0);
#else
  setRotation(1);
#endif
  fillScreen(0);
#ifdef TFT_BL
  // Init the back-light LED PWM
  ledcSetup(BLK_PWM_CHANNEL, 44100, 8);
  ledcAttachPin(TFT_BL, BLK_PWM_CHANNEL);
  ledcWrite(BLK_PWM_CHANNEL, 80);
#endif
}

void M5Display::sleep() {
  startWrite();
  writecommand(ILI9341_SLPIN); // Software reset
  endWrite();
}

void M5Display::wakeup() {
  startWrite();
  writecommand(ILI9341_SLPOUT);
  endWrite();
}

void M5Display::setBrightness(uint8_t brightness) {
  #ifdef ARDUINO_ESP32_DEV
    // WROVER_KIT Lcd has inverted brightness values
    brightness = 255 - brightness;
  #endif
  ledcWrite(BLK_PWM_CHANNEL, brightness);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint16_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t *data, uint16_t transparent) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, data, transparent);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, (const uint16_t*)data);
  setSwapBytes(swap);
}

void M5Display::drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, uint8_t *data) {
  bool swap = getSwapBytes();
  setSwapBytes(true);
  pushImage((int32_t)x0, (int32_t)y0, (uint32_t)w, (uint32_t)h, (uint16_t*)data);
  setSwapBytes(swap);
}

void M5Display::progressBar(int x, int y, int w, int h, uint8_t val) {
  drawRect(x, y, w, h, 0x09F1);
  fillRect(x + 1, y + 1, w * (((float)val) / 100.0), h - 1, 0x09F1);
}

#include "utility/qrcode.h"
void M5Display::qrcode(const char *string, uint16_t x, uint16_t y, uint8_t width, uint8_t version) {

  // Create the QR code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, 0, string);

  // Top quiet zone
  uint8_t thickness = width / qrcode.size;
  uint16_t lineLength = qrcode.size * thickness;
  uint8_t xOffset = x + (width-lineLength)/2;
  uint8_t yOffset = y + (width-lineLength)/2;
  fillRect(x, y, width, width, TFT_WHITE);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      uint8_t q = qrcode_getModule(&qrcode, x, y);
      if (q) fillRect(x * thickness + xOffset, y * thickness + yOffset, thickness, thickness, TFT_BLACK);
    }
  }
}

void M5Display::qrcode(const String &string, uint16_t x, uint16_t y, uint8_t width, uint8_t version) {
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  qrcode(buffer, x, y, width, version);
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Bodmers BMP image rendering function
void M5Display::drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y) {
  if ((x >= width()) || (y >= height())) return;

  // Open requested file on SD card
  File bmpFS = fs.open(path, "r");

  if (!bmpFS) {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
      y += h - 1;

      setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// void M5Display::drawBmp(fs::FS &fs, const char *path, uint16_t x, uint16_t y) {
//   drawBmpFile(fs, path, x, y);
// }


/*
 * JPEG
 */
static M5Display *jpegDisplay;
static bool fast_jpg_tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if ( y >= jpegDisplay->height() ) return 0;
  bool swap = jpegDisplay->getSwapBytes();
  jpegDisplay->setSwapBytes(true);
  jpegDisplay->pushImage(x, y, w, h, bitmap);
  jpegDisplay->setSwapBytes(swap);
  return 1;
}
void M5Display::drawJpg( const uint8_t * jpg_data, uint32_t jpg_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale ) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  jpegDisplay = this;
  if( setJpegRenderCallBack ) setJpegRenderCallBack( fast_jpg_tft_output );
  if( jpgFlashRenderFunc )    jpgFlashRenderFunc(jpg_data, jpg_len, x, y, maxWidth, maxHeight, offX, offY );
}
void M5Display::drawJpgFile( fs::FS &fs, const char *path, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale ) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  jpegDisplay = this;
  if( setJpegRenderCallBack ) setJpegRenderCallBack( fast_jpg_tft_output );
  if( jpgFSRenderFunc )       jpgFSRenderFunc(fs, path, x, y, maxWidth, maxHeight, offX, offY );
}
void M5Display::drawJpgFile( Stream *dataSource, uint32_t data_len, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale ) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  jpegDisplay = this;
  if( setJpegRenderCallBack ) setJpegRenderCallBack( fast_jpg_tft_output );
  if( jpgStreamRenderFunc )   jpgStreamRenderFunc( dataSource, data_len, x, y, maxWidth, maxHeight, offX, offY );
}

/*
 * PNG
 */
static M5Display *pngDisplay;
static bool fast_png_tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  if ( y >= pngDisplay->height() ) return 0;
  pngDisplay->fillRect(x, y, w, h, color);
  return 1;
}
void M5Display::drawPngFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  pngDisplay = this;
  if( setPngRenderCallBack ) setPngRenderCallBack( fast_png_tft_output );
  if( pngFSRenderFunc )      pngFSRenderFunc(fs, path, x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold );
}
void M5Display::drawPngFile(Stream &readSource, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  pngDisplay = this;
  if( setPngRenderCallBack ) setPngRenderCallBack( fast_png_tft_output );
  if( pngStreamRenderFunc )  pngStreamRenderFunc( &readSource, x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold );
}
void M5Display::drawPng(const uint8_t *png_data, size_t png_len, int32_t x, int32_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, double scale, uint8_t alphaThreshold ) {
  if ((x + maxWidth) > width() || (y + maxHeight) > height()) { log_e("Bad dimensions given"); return; }
  pngDisplay = this;
  if( setPngRenderCallBack ) setPngRenderCallBack( fast_png_tft_output );
  if( pngFlashRenderFunc )   pngFlashRenderFunc(png_data, png_len, x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold );
}
