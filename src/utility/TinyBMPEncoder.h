#ifndef __TINY_BMP_ENCODER_H
#define __TINY_BMP_ENCODER_H

#include "M5Display.h"

class BMP_Encoder {

  public:

    void init( M5Display *tft, fs::FS &fileSystem  );
    bool encodeToFile( const char* filename, const int imageW, const int imageH );

  private:

    uint8_t *bmpBuffer = NULL;
    M5Display *_tft;
    fs::FS * _fileSystem;

};

#endif // __TINY_BMP_ENCODER_H
