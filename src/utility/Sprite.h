/***************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the TFT_eSPI class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
***************************************************************************************/
#include <FS.h>
#include <SPI.h>
#include "In_eSPI.h"

#define truncateFP(V,S) ((V + (V < 0 ? -1<<(S-1) : 1<<(S-1)))>>S)

class TFT_eSprite : public TFT_eSPI {

 public:

  TFT_eSprite(TFT_eSPI *tft); // Constructor
  virtual ~TFT_eSprite();

           // Create a sprite of width x height pixels, return a pointer to the RAM area
           // Sketch can cast returned value to (uint16_t*) for 16 bit depth if needed
           // RAM required is:
           //  - 1 bit per pixel for 1 bit colour depth
           //  - 1 byte per pixel for 8 bit colour
           //  - 2 bytes per pixel for 16 bit color depth
  void*    createSprite(int16_t width, int16_t height, uint8_t frames = 1);

           // Delete the sprite to free up the RAM
  void     deleteSprite(void);

           // Select the frame buffer for graphics write (for 2 colour ePaper and DMA toggle buffer)
           // Returns a pointer to the Sprite frame buffer
  void*    frameBuffer(int8_t f);

           // Set or get the colour depth to 8 or 16 bits. Can be used to change depth an existing
           // sprite, but clears it to black, returns a new pointer if sprite is re-created.
  void*    setColorDepth(int8_t b);
  int8_t   getColorDepth(void);

           // Set foreground and background colours for 1 bit per pixel Sprite
  void     setBitmapColor(uint16_t fg, uint16_t bg);

  void     drawPixel(int32_t x, int32_t y, uint32_t color);

  void     drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t font),

           fillSprite(uint32_t color),

           // Define a window to push 16 bit colour pixels into in a raster order
            // Colours are converted to the set Sprite colour bit depth
           setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1),
           pushColor(uint32_t color),
           pushColor(uint32_t color, uint16_t len),
           // Push a pixel preformatted as a 8 or 16 bit colour (avoids conversion overhead)
           writeColor(uint16_t color),

           // Set the scroll zone, top left corner at x,y with defined width and height
           // The colour (optional, black is default) is used to fill the gap after the scroll
           setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color = TFT_BLACK),
           // Scroll the defined zone dx,dy pixels. Negative values left,up, positive right,down
           // dy is optional (default is then no up/down scroll).
           // The sprite coordinate frame does not move because pixels are moved
           scroll(int16_t dx, int16_t dy = 0),

           drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
           drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
           drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),

           drawGradientLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBColor colorstart, RGBColor colorend),
           drawGradientHLine(int32_t x, int32_t y, int32_t w, RGBColor colorstart, RGBColor colorend),
           drawGradientVLine( int32_t x, int32_t y, int32_t h, RGBColor colorstart, RGBColor colorend),

           fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

  RGBColor colorAt( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, RGBColor colorstart, RGBColor colorend );

           // Set the sprite text cursor position for print class (does not change the TFT screen cursor)
           //setCursor(int16_t x, int16_t y);

           // Set the rotation of the Sprite (for 1bpp Sprites only)
  void     setRotation(uint8_t rotation);
  uint8_t  getRotation(void);


  bool     pushRotated(int16_t angle, int32_t transp = -1);   // Using fixed point maths
  bool     pushRotatedHP(int16_t angle, int32_t transp = -1); // Using higher precision floating point maths
  bool     pushRotated(TFT_eSprite *spr, int16_t angle, int32_t transp = -1);   // Using fixed point maths
  bool     pushRotatedHP(TFT_eSprite *spr, int16_t angle, int32_t transp = -1); // Using  higher precision floating point

           // Set and get the pivot point for this Sprite
  void     setPivot(int16_t x, int16_t y);
  int16_t  getPivotX(void),
           getPivotY(void);

           // Get the bounding box for a rotated copy of this Sprite
  void     getRotatedBounds(float sina, float cosa, int16_t w, int16_t h, int16_t xp, int16_t yp,
                                int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);

           // Read the colour of a pixel at x,y and return value in 565 format
  uint16_t readPixel(int32_t x0, int32_t y0);

           // Write an image (colour bitmap) to the sprite
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, int32_t transparent=-1);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data, int32_t transparent=-1);
  //void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);

  bool     setupImgDecoder( int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0 );

  void drawJpgFile( fs::FS &fs, const char *path, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE );
  void drawJpgFile( Stream *dataSource, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE );
  void drawJpg( const uint8_t *jpg_data, uint32_t jpg_len, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE );

  // PNG implementation by https://github.com/kikuchan
  void drawPngFile( fs::FS &fs, const char *path, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  void drawPngFile( Stream &readSource, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  void drawPng( const uint8_t *png_data, size_t png_len, int32_t x=0, int32_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );
  __attribute__((deprecated)) void drawPngUrl( const char *url, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, double scale=1.0, uint8_t alphaThreshold=127, uint16_t bgcolor=TFT_BLACK  );

           // Swap the byte order for pushImage() - corrects different image endianness
  void     setSwapBytes(bool swap);
  bool     getSwapBytes(void);

           // Push the sprite to the TFT screen, this fn calls pushImage() in the TFT class.
           // Optionally a "transparent" colour can be defined, pixels of that colour will not be rendered
  void     pushSprite(int32_t x, int32_t y);
  void     pushSprite(int32_t x, int32_t y, uint16_t transparent);

  int16_t  drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font),
           drawChar(uint16_t uniCode, int32_t x, int32_t y);

           // Return the width and height of the sprite
  int16_t  width(void),
           height(void);

           // Used by print class to print text to cursor position
  size_t   write(uint8_t);

           // Functions associated with anti-aliased fonts
  void     drawGlyph(uint16_t code);
  void     printToSprite(String string);
  void     printToSprite(char *cbuffer, uint16_t len);
  int16_t  printToSprite(int16_t x, int16_t y, uint16_t index);
  void     setPsram( bool enabled );

 private:

  TFT_eSPI *_tft;

           // Reserve memory for the Sprite and return a pointer
  void*    callocSprite(int16_t width, int16_t height, uint8_t frames = 1);

 protected:

  uint8_t  _bpp;     // bits per pixel (1, 8 or 16)
  uint16_t *_img;    // pointer to 16 bit sprite
  uint8_t  *_img8;   // pointer to  8 bit sprite
  uint8_t  *_img8_1; // pointer to  frame 1
  uint8_t  *_img8_2; // pointer to  frame 2

  int16_t _xpivot;   // x pivot point coordinate
  int16_t _ypivot;   // y pivot point coordinate

  bool     _created;    // A Sprite has been created and memory reserved
  bool     _gFont = false;
  bool     _usePsram = true; // use Psram if available

//  int32_t  _icursor_x, _icursor_y;
  uint8_t  _rotation = 0;
  int32_t  _xs, _ys, _xe, _ye, _xptr, _yptr; // for setWindow
  int32_t  _sx, _sy; // x,y for scroll zone
  uint32_t _sw, _sh; // w,h for scroll zone
  uint32_t _scolor;  // gap fill colour for scroll zone

  bool     _iswapBytes; // Swap the byte order for Sprite pushImage()

  int32_t  _iwidth, _iheight; // Sprite memory image bit width and height (swapped during rotations)
  int32_t  _dwidth, _dheight; // Real display width and height (for <8bpp Sprites)
  int32_t  _bitwidth;         // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)

};
