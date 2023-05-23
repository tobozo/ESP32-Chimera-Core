#pragma once
#define I2CUtil_h

#include <Wire.h>

typedef void (*I2CPrintCb)( const char* format, ... );


class I2CUtil
{
  public:

    I2CUtil( TwoWire *i2cPort = &Wire, uint32_t clockspeed = 0 ) { _i2cPort = i2cPort; _clockspeed = clockspeed; }


    //I2CUtil() { };
    void init(TwoWire* wire, uint8_t address, int i2cSDA, int i2cSCL);
    bool available();
    void write1Byte(uint8_t subAddress, uint8_t data);
    void write1Byte(uint8_t address, uint8_t subAddress, uint8_t data);
    void write16Bit(uint8_t subAddress, uint8_t data_1, uint8_t data_2);
    uint8_t read8Bit(uint8_t subAddress);
    uint16_t read12Bit(uint8_t subAddress);
    uint16_t read13Bit(uint8_t subAddress);
    uint16_t read16Bit(uint8_t subAddress);
    uint16_t read16Bit_lowFirst(uint8_t subAddress);
    uint32_t read24Bit(uint8_t subAddress);
    uint32_t read32Bit(uint8_t subAddress);
    void readBuff(uint8_t subAddress, int size, uint8_t buff[]);


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
    int _sdaPin = -1, _sclPin = -1, _address = -1;
    bool _begun = false;

    void checkFreq( bool unset = false );

    I2CPrintCb printCb = nullptr;
    static void defaultPrintCb(const char* format, ...);

};

extern I2CUtil I2CUtil_Core; // I2C Scanner && Twatch I2C bus
