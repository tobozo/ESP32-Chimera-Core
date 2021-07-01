#include "I2CUtil.h"
//#include <stdint.h>
#include "../../../ESP32-Chimera-Core.h"

extern M5Stack M5;

I2CUtil::I2CUtil() {
}


bool I2CUtil::begin( signed char sdaPin, signed char sclPin )
{
  return Wire.begin( sdaPin, sclPin );
}



bool I2CUtil::writeCommand( unsigned char i2c_addr, unsigned char reg )
{
  bool res = false;

  Wire.beginTransmission(i2c_addr);
  Wire.write(reg);
  res = (Wire.endTransmission() == 0);

  log_d("send to 0x%02x [0x%02x] result:%s", i2c_addr, reg, res ? "OK" : "NG");

  return res;
}



bool I2CUtil::writeByte( unsigned char i2c_addr, unsigned char reg, unsigned char data )
{
  bool res = false;

  Wire.beginTransmission(i2c_addr);
  Wire.write(reg);
  Wire.write(data);
  res = (Wire.endTransmission() == 0);

  log_d("send to 0x%02x [0x%2x] data=0x%02x result:%s", i2c_addr, reg, data, res ? "OK" : "NG");

  return res;
}



bool I2CUtil::writeBytes( unsigned char i2c_addr, unsigned char reg, unsigned char *data, unsigned char length )
{
  bool res = false;

  Wire.beginTransmission(i2c_addr);
  Wire.write(reg);

  for(int i = 0; i < length; i++) {
    Wire.write(*(data+i));
    log_v("sent 1 byte (0x%2x) to 0x%02x [0x%02x]", *(data+i), i2c_addr, reg);
  }
  res = Wire.endTransmission() == 0;

  log_d("sent to 0x%02x [0x%02x], result:%s", i2c_addr,reg, res ? "OK" : "NG");

  return res;
}



bool I2CUtil::readByte( unsigned char i2c_addr, unsigned char *buf )
{
  log_v("read 1 byte from 0x%02x receive=", i2c_addr);

  if ( Wire.requestFrom(i2c_addr, (unsigned char)1)) {
    *buf = Wire.read();

    log_v("read 1 byte from 0x%02x received: 0x%02x", i2c_addr, buf);

    return true;
  }

  log_d("read 1 byte from 0x%02x received none", i2c_addr);

  return false;
}



bool I2CUtil::readByte( unsigned char i2c_addr, unsigned char reg,unsigned char *buf )
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg);

  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(i2c_addr, (unsigned char)1)) {
    *buf = Wire.read();

    log_v("read 1 byte from 0x%02x [0x%02x] received: 0x%02x", i2c_addr, reg, *buf);

    return true;
  }

  log_d("read 1 byte from 0x%02x [0x%02x] received none", i2c_addr);

  return false;
}



bool I2CUtil::readBytes( unsigned char i2c_addr, unsigned char reg, unsigned char count,unsigned char *dest )
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg);

  unsigned char i = 0;

  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(i2c_addr, (unsigned char)count)) {
    while (Wire.available()) {
      dest[i++] = Wire.read();
      log_v("%02x ", dest[i-1]);
    }

    log_v("read from 0x%02x [0x%02x] (%d bytes) received %d bytes", i2c_addr, reg, count, i);

    return true;
  }

  log_d("read from 0x%02x [0x%02x] (%d bytes) received none", i2c_addr, reg, count);

  return false;
}



bool I2CUtil::readBytes( unsigned char i2c_addr, unsigned char count,unsigned char * dest )
{
  unsigned char i = 0;

  if (Wire.requestFrom(i2c_addr, (unsigned char)count)) {
    while (Wire.available()) {
      dest[i++] = Wire.read();
    }
    return true;
  }

  return false;
}



void I2CUtil::scanID( bool *result )
{
  for(int i=0x00;i<=0x7f;i++) {
    *(result+i)=writeCommand(i,0x00);
  }
}


// I2C Scanner, scavengered from https://github.com/MartyMacGyver/Arduino_I2C_Scanner
void I2CUtil::scan()
{
  Serial.println();
  Serial.println("Scanning I2C Bus: ");
  Serial.println();
  Serial.print("   ");
  for (int i = 0; i < 0x10; i++) {
    Serial.printf(" %2x", i);
  }
  Serial.println();
  Serial.print("            ");
  for(unsigned char addr = 0x03; addr <= 0x77; addr++ ) {
    // Address the device
    Wire.beginTransmission(addr);
    // Check for ACK (detection of device), NACK or error
    unsigned char deviceStatus = Wire.endTransmission();

    if (!(addr % 0x10)) { // Start of a line
      Serial.printf("%02x:", addr / 0x10);
    }
    if (deviceStatus == 0) {
      Serial.printf(" %02x", addr);
    } else if (deviceStatus == 4) {
      Serial.printf(" %2s", "??");
    } else {
      Serial.printf(" %2s", "--");
    }
    if (!((addr+1) % 0x10) ) {
      Serial.println();
    }
  }
  Serial.println();
}
