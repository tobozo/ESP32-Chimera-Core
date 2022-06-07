#pragma once
#define I2CUtil_h

#include <Wire.h>

typedef void (*I2CPrintCb)( const char* format, ... );


class I2CUtil
{
  public:

    I2CUtil( TwoWire *i2cPort = &Wire, uint32_t clockspeed = 0 ) { _i2cPort = i2cPort; _clockspeed = clockspeed; }

    //bool begin( signed char sdaPin, signed char sclPin );
    bool begin( int sdaPin, int sclPin, TwoWire *i2cPort = nullptr );
    bool begun() { return _begun; };

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

    TwoWire *_i2cPort;
    uint32_t _clockspeed = 0;
    uint32_t _lastclockspeed = 0;
    int _sdaPin = -1, _sclPin = -1;
    bool _begun = false;

    void checkFreq( bool unset = false );

    I2CPrintCb printCb = nullptr;
    static void defaultPrintCb(const char* format, ...);

};

//extern I2CUtil I2CUtil_Core; // I2C Scanner && Twatch I2C bus
