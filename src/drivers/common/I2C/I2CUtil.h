#ifndef I2CUtil_h
#define I2CUtil_h

//#include <stdint.h>
#include <Wire.h>

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

    void scanID(bool *result);
    void scan();
};

#endif
