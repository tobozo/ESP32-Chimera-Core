#include "I2CUtil.h"

//I2CUtil() { };
void I2CUtil::init(TwoWire* wire, uint8_t address, int sdaPin, int sclPin)
{
  _address = address;
  begin( sdaPin, sclPin, wire );
}

bool I2CUtil::available()
{
  if(!_i2cPort) return false;
  _i2cPort->beginTransmission(_address);
  return (_i2cPort->endTransmission() ? true : false);
}


void I2CUtil::write1Byte(uint8_t subAddress, uint8_t data)
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(subAddress);
  _i2cPort->write(data);
  _i2cPort->endTransmission();
}

void I2CUtil::write1Byte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  _i2cPort->beginTransmission(address);
  _i2cPort->write(subAddress);
  _i2cPort->write(data);
  _i2cPort->endTransmission();
}

uint8_t I2CUtil::read8Bit(uint8_t subAddress)
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(subAddress);
  _i2cPort->endTransmission();
  _i2cPort->requestFrom(_address, (size_t)1);
  return _i2cPort->read();
}

uint16_t I2CUtil::read12Bit(uint8_t subAddress)
{
  uint8_t buff[2];
  readBuff(subAddress, 2, buff);
  return (buff[0] << 4) + buff[1];
}

uint16_t I2CUtil::read13Bit(uint8_t subAddress)
{
  uint8_t buff[2];
  readBuff(subAddress, 2, buff);
  return (buff[0] << 5) + buff[1];
}

uint16_t I2CUtil::read16Bit(uint8_t subAddress)
{
  uint8_t buff[2];
  readBuff(subAddress, 2, buff);
  return (buff[0] << 8) + buff[1];
}

uint32_t I2CUtil::read24Bit(uint8_t subAddress)
{
  uint8_t buff[4];
  readBuff(subAddress, 3, buff);
  return (buff[0] << 16) + (buff[1] << 8) + buff[2];
}

uint32_t I2CUtil::read32Bit(uint8_t subAddress)
{
  uint8_t buff[4];
  readBuff(subAddress, 4, buff);
  return (buff[0] << 24) + (buff[1] << 16) + (buff[2] << 8) + buff[3];
}

void I2CUtil::readBuff(uint8_t subAddress, int size, uint8_t buff[])
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(subAddress);
  _i2cPort->endTransmission();
  _i2cPort->requestFrom(_address, (size_t)size);
  for (int i = 0; i < size; i++) {
    buff[i] = _i2cPort->read();
  }
}

uint16_t I2CUtil::read16Bit_lowFirst(uint8_t subAddress)
{
  uint8_t buff[2];
  readBuff(subAddress, 2, buff);
  return buff[0] + (buff[1] << 8);
}

void I2CUtil::write16Bit(uint8_t subAddress, uint8_t data_1, uint8_t data_2)
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(subAddress);
  _i2cPort->write(data_1);
  _i2cPort->write(data_2);
  _i2cPort->endTransmission();
}



bool I2CUtil::begin( int sdaPin, int sclPin, TwoWire *i2cPort )
{
  if( sdaPin != _sdaPin || sclPin != _sclPin ) {
    log_d("New I2C pins Context (old=%d/%d, new=%d/%d)", _sdaPin, _sclPin, sdaPin, sclPin );
    _begun = false;
  }
  if( i2cPort != nullptr ) {
    if( _i2cPort != i2cPort ) {
      log_d("New I2C port Context");
      _begun = false;
      _i2cPort = i2cPort;
    }
  }
  if( _i2cPort == nullptr ) {
    log_e("Can't start I2CUtils without proper I2C Port");
    return false;
  }
  if( _begun ) {
    log_d("Skipping I2C start (already started) on %s with SDA/SCL: %d/%d", i2cPort == &Wire?"Wire":"Wire1", _sdaPin, _sclPin );
    return true;
  }
  _sdaPin = sdaPin;
  _sclPin = sclPin;
  bool ret = _i2cPort->begin( _sdaPin, _sclPin );
  uint32_t currentClockSpeed = _i2cPort->getClock();
  bool wire_started = currentClockSpeed > 0;

  if( !wire_started ) {
    log_e("Failed to get freq from Wire, aborting");
    return false;
  }

  _lastclockspeed = currentClockSpeed;
  if( _clockspeed == 0 ) {
    _clockspeed = currentClockSpeed; // inherit clock speed
  }

  _i2cPort->setClock( _clockspeed );

  log_d("Begin I2C %s on %s with SDA/SCL: %d/%d at %d Hz", ret?"SUCCESS":"FAIL", _i2cPort == &Wire?"Wire":"Wire1", _sdaPin, _sclPin, _clockspeed );
  if( ret ) _begun = true;
  return ret;
}


void I2CUtil::checkFreq( bool unset )
{
  uint32_t currentClockSpeed = _i2cPort->getClock();
  if( unset ) { // restore speed
    if( currentClockSpeed == _lastclockspeed ) return;
    //log_d("Restoring freq from %d to %d", currentClockSpeed, _lastclockspeed );
    _i2cPort->setClock( _lastclockspeed );
  } else {
    if( currentClockSpeed == _clockspeed ) return;
    log_v("Setting freq to %d (was: %d)", _clockspeed, currentClockSpeed );
    _lastclockspeed = currentClockSpeed;
    _i2cPort->setClock( _clockspeed );
  }
}


bool I2CUtil::writeCommand( unsigned char i2c_addr, unsigned char reg )
{
  bool res = false;
  checkFreq(false);
  _i2cPort->beginTransmission(i2c_addr);
  _i2cPort->write(reg);
  res = (_i2cPort->endTransmission() == 0);

  log_v("send to 0x%02x [0x%02x] result:%s", i2c_addr, reg, res ? "OK" : "NG");
  checkFreq(true);
  return res;
}



bool I2CUtil::writeByte( unsigned char i2c_addr, unsigned char reg, unsigned char data )
{
  bool res = false;
   checkFreq(false);
  _i2cPort->beginTransmission(i2c_addr);
  _i2cPort->write(reg);
  _i2cPort->write(data);
  res = (_i2cPort->endTransmission() == 0);
  checkFreq(true);
  log_v("send to 0x%02x [0x%2x] data=0x%02x result:%s", i2c_addr, reg, data, res ? "OK" : "NG");

  return res;
}



bool I2CUtil::writeBytes( unsigned char i2c_addr, unsigned char reg, unsigned char *data, unsigned char length )
{
  bool res = false;
  checkFreq(false);
  _i2cPort->beginTransmission(i2c_addr);
  _i2cPort->write(reg);

  for(int i = 0; i < length; i++) {
    _i2cPort->write(*(data+i));
    log_v("sent 1 byte (0x%2x) to 0x%02x [0x%02x]", *(data+i), i2c_addr, reg);
  }
  res = _i2cPort->endTransmission() == 0;

  log_v("sent to 0x%02x [0x%02x], result:%s", i2c_addr,reg, res ? "OK" : "NG");
  checkFreq(true);
  return res;
}



bool I2CUtil::readByte( unsigned char i2c_addr, unsigned char *buf )
{
  log_v("read 1 byte from 0x%02x receive=", i2c_addr);
  checkFreq(false);
  if ( _i2cPort->requestFrom(i2c_addr, (unsigned char)1)) {
    *buf = _i2cPort->read();

    log_v("read 1 byte from 0x%02x received: 0x%02x", i2c_addr, buf);
    checkFreq(true);
    return true;
  }

  log_v("tried to read 1 byte from i2c addr 0x%02x and received none", i2c_addr);
  checkFreq(true);
  return false;
}



bool I2CUtil::readByte( unsigned char i2c_addr, unsigned char reg, unsigned char *buf )
{
  checkFreq(false);
  _i2cPort->beginTransmission(i2c_addr);
  _i2cPort->write(reg);

  if (_i2cPort->endTransmission(false) == 0 && _i2cPort->requestFrom(i2c_addr, (unsigned char)1)) {
    *buf = _i2cPort->read();

    log_v("read 1 byte from 0x%02x [0x%02x] received: 0x%02x", i2c_addr, reg, *buf);
    checkFreq(true);
    return true;
  }

  log_v("tried to read 1 byte from i2c addr 0x%02x / reg 0x%02x and received none", i2c_addr, reg);
  checkFreq(true);
  return false;
}



bool I2CUtil::readBytes( unsigned char i2c_addr, unsigned char reg, unsigned char count,unsigned char *dest )
{
  checkFreq(false);
  _i2cPort->beginTransmission(i2c_addr);
  _i2cPort->write(reg);

  unsigned char i = 0;

  if (_i2cPort->endTransmission(false) == 0 && _i2cPort->requestFrom(i2c_addr, (unsigned char)count)) {
    while (_i2cPort->available()) {
      dest[i++] = _i2cPort->read();
      log_v("%02x ", dest[i-1]);
    }

    log_v("read from 0x%02x [0x%02x] (%d bytes) received %d bytes", i2c_addr, reg, count, i);
    checkFreq(true);
    return true;
  }

  log_v("tried to read %d bytes from i2c addr 0x%02x / reg 0x%02x and received none", count, i2c_addr, reg);
  checkFreq(true);
  return false;
}



bool I2CUtil::readBytes( unsigned char i2c_addr, unsigned char count,unsigned char * dest )
{
  unsigned char i = 0;
  checkFreq(false);
  if (_i2cPort->requestFrom(i2c_addr, (unsigned char)count)) {
    while (_i2cPort->available()) {
      dest[i++] = _i2cPort->read();
    }
    checkFreq(true);
    return true;
  }

  log_v("tried to read %d bytes from i2c addr 0x%02x and received none", count, i2c_addr);
  checkFreq(true);
  return false;
}



void I2CUtil::scanID( bool *result )
{
  for(int i=0x00;i<=0x7f;i++) {
    *(result+i)=writeCommand(i,0x00);
  }
}


void I2CUtil::defaultPrintCb(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}


// I2C Scanner, scavengered from https://github.com/MartyMacGyver/Arduino_I2C_Scanner
void I2CUtil::scan()
{
  if( !_begun ) {
    log_d("Call I2C.begin(sda,scl) first!");
    return;
  }
  if( printCb == nullptr ) {
    printCb = defaultPrintCb;
  }
  printCb("\n");
  printCb("\n");
  printCb("Scanning I2C Bus: \n");
  printCb("\n");
  printCb("   ");
  for (int i = 0; i < 0x10; i++) {
    printCb(" %2x", i);
  }
  printCb("\n");
  printCb("            ");
  for(unsigned char addr = 0x03; addr <= 0x77; addr++ ) {
    // Address the device
    _i2cPort->beginTransmission(addr);
    // Check for ACK (detection of device), NACK or error
    unsigned char deviceStatus = _i2cPort->endTransmission();

    if (!(addr % 0x10)) { // Start of a line
      printCb("%02x:", addr / 0x10);
    }
    if (deviceStatus == 0) {
      printCb(" %02x", addr);
    } else if (deviceStatus == 4) {
      printCb(" %2s", "??");
    } else {
      printCb(" %2s", "--");
    }
    if (!((addr+1) % 0x10) ) {
      printCb("\n");
    }
  }
  printCb("\n");
}


I2CUtil I2CUtil_Core;
