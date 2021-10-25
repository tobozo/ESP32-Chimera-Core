#include "RTC_BM8563.h"


RTC_BM8563::RTC_BM8563()
{

}


void RTC_BM8563::begin( int i2c_port, int pin_sda, int pin_scl, int freq )
{
  // override defaults if necessary
  if( pin_sda  != -1 ) PIN_SDA = pin_sda;
  if( pin_scl  != -1 ) PIN_SCL = pin_scl;
  if( i2c_port != -1 ) I2C_PORT = i2c_port;

  auto &twowire = (I2C_PORT) ? Wire1 : Wire;

  if( twowire.begin( PIN_SDA, PIN_SCL ) ) {

    if( freq > 0 ) {
      log_d("Setting I2C clock to %d", freq );
      twowire.setClock(freq);
    }

    log_d("[RTC BM8563] Begin (port=Wire%s, SDA=%d, SCL=%d, freq=%s)", (I2C_PORT) ? "1" : "", PIN_SDA, PIN_SCL, String(twowire.getClock()).c_str() );

    // sent init signals to RTC module
    WriteReg(0x00,0x00);
    WriteReg(0x01,0x00);
    WriteReg(0x0D,0x00);

  } else {
    log_n("[RTC BM8563] Begin failed ! (port=Wire%s, SDA=%d, SCL=%d, freq=%s)", (I2C_PORT) ? "1" : "", PIN_SDA, PIN_SCL, String(twowire.getClock()).c_str() );
  }
}


void RTC_BM8563::WriteReg(uint8_t reg, uint8_t data)
{
  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(reg);
  twowire.write(data);
  twowire.endTransmission();
}


uint8_t RTC_BM8563::ReadReg(uint8_t reg)
{
  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(reg);
  twowire.endTransmission();
  twowire.requestFrom(BM8563_ADDR, (uint8_t)1);
  uint8_t buf = twowire.read();
  return buf;
}


void RTC_BM8563::GetBm8563Time(void)
{
  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(0x02);
  twowire.endTransmission();
  twowire.requestFrom(BM8563_ADDR,(uint8_t)7);
  while(twowire.available()){
    trdata[0] = twowire.read();
    trdata[1] = twowire.read();
    trdata[2] = twowire.read();
    trdata[3] = twowire.read();
    trdata[4] = twowire.read();
    trdata[5] = twowire.read();
    trdata[6] = twowire.read();
  }
  DataMask();
  Bcd2asc();
  Str2Time();
}


void RTC_BM8563::Str2Time(void)
{
  Second = (asc[0] - 0x30) * 10 + asc[1] - 0x30;
  Minute = (asc[2] - 0x30) * 10 + asc[3] - 0x30;
  Hour = (asc[4] - 0x30) * 10 + asc[5] - 0x30;
}


void RTC_BM8563::DataMask()
{
  trdata[0] = trdata[0]&0x7f;
  trdata[1] = trdata[1]&0x7f;
  trdata[2] = trdata[2]&0x3f;
  trdata[3] = trdata[3]&0x3f;
  trdata[4] = trdata[4]&0x07;
  trdata[5] = trdata[5]&0x1f;
  trdata[6] = trdata[6]&0xff;
}

/********************************************************************
 BCD to ASCII code conversion
***********************************************************************/
void RTC_BM8563::Bcd2asc(void)
{
  uint8_t i,j;
  for (j=0,i=0; i<7; i++){
    asc[j++] = (trdata[i]&0xf0)>>4|0x30;
    asc[j++] = (trdata[i]&0x0f)|0x30;
  }
}


uint8_t RTC_BM8563::Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}


uint8_t RTC_BM8563::ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;

  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }

  return  ((uint8_t)(bcdhigh << 4) | Value);
}


void RTC_BM8563::GetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint8_t buf[3] = {0};
  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(0x02);
  twowire.endTransmission();
  twowire.requestFrom(BM8563_ADDR,(uint8_t)3);

  while(twowire.available()){
    buf[0] = twowire.read();
    buf[1] = twowire.read();
    buf[2] = twowire.read();
  }

  RTC_TimeStruct->Seconds = Bcd2ToByte(buf[0]&0x7f);
  RTC_TimeStruct->Minutes = Bcd2ToByte(buf[1]&0x7f);
  RTC_TimeStruct->Hours   = Bcd2ToByte(buf[2]&0x3f);
}


void RTC_BM8563::SetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
  if(RTC_TimeStruct == NULL)
    return;

  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(0x02);
  twowire.write(ByteToBcd2(RTC_TimeStruct->Seconds));
  twowire.write(ByteToBcd2(RTC_TimeStruct->Minutes));
  twowire.write(ByteToBcd2(RTC_TimeStruct->Hours));
  twowire.endTransmission();
}


void RTC_BM8563::GetDate(RTC_DateTypeDef* RTC_DateStruct)
{
  uint8_t buf[4] = {0};

  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(0x05);
  twowire.endTransmission();
  twowire.requestFrom(BM8563_ADDR,(uint8_t)4);

  while(twowire.available()){
    buf[0] = twowire.read();
    buf[1] = twowire.read();
    buf[2] = twowire.read();
    buf[3] = twowire.read();
  }

  RTC_DateStruct->Date    = Bcd2ToByte(buf[0]&0x3f);
  RTC_DateStruct->WeekDay = Bcd2ToByte(buf[1]&0x07);
  RTC_DateStruct->Month   = Bcd2ToByte(buf[2]&0x1f);

  if(buf[2]&0x80){
    RTC_DateStruct ->Year = 1900 + Bcd2ToByte(buf[3]&0xff);
  } else {
    RTC_DateStruct ->Year = 2000  + Bcd2ToByte(buf[3]&0xff);
  }
}


void RTC_BM8563::SetDate(RTC_DateTypeDef* RTC_DateStruct)
{
  if(RTC_DateStruct == NULL)
    return;
  auto &twowire = (I2C_PORT) ? Wire1 : Wire;
  twowire.beginTransmission(BM8563_ADDR);
  twowire.write(0x05);
  twowire.write(ByteToBcd2(RTC_DateStruct->Date));
  twowire.write(ByteToBcd2(RTC_DateStruct->WeekDay));

  if(RTC_DateStruct->Year < 2000){
    twowire.write(ByteToBcd2(RTC_DateStruct->Month) | 0x80);
    twowire.write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year % 100)));
  } else {
    twowire.write(ByteToBcd2(RTC_DateStruct->Month) | 0x00);
    twowire.write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year %100)));
  }
  twowire.endTransmission();
}

