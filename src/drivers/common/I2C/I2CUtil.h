#ifndef I2CUtil_h
#define I2CUtil_h

#include <Wire.h>

typedef void (*I2CPrintCb)( const char* format, ... );


class I2CUtil
{
  public:

    I2CUtil();

    bool begin( signed char sdaPin, signed char sclPin );

    bool writeCommand( unsigned char i2c_addr, unsigned char reg);

    bool writeByte( unsigned char i2c_addr, unsigned char reg, unsigned char data );
    bool writeBytes( unsigned char i2c_addr, unsigned char reg, unsigned char *data,unsigned char length );

    bool readByte( unsigned char i2c_addr, unsigned char *buf );
    bool readByte( unsigned char i2c_addr, unsigned char reg,unsigned char *buf );
    bool readBytes( unsigned char i2c_addr, unsigned char count,unsigned char * dest );
    bool readBytes( unsigned char i2c_addr, unsigned char reg, unsigned char count, unsigned char * dest );

    void setScanOutputCb( I2CPrintCb output ) { printCb = output; };
    void scanID(bool *result);
    void scan();

  private:

    I2CPrintCb printCb = nullptr;
    static void defaultPrintCb(const char* format, ...);

};

#endif
