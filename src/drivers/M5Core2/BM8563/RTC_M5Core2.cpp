#include "RTC_M5Core2.h"


RTC_M5Core2::RTC_M5Core2()
{

}


void RTC_M5Core2::begin(void)
{
  log_d("[RTC] Begin");
  Wire1.begin(21,22);
  WriteReg(0x00,0x00);
  WriteReg(0x01,0x00);
  WriteReg(0x0D,0x00);
}


void RTC_M5Core2::WriteReg(uint8_t reg, uint8_t data)
{
  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(reg);
  Wire1.write(data);
  Wire1.endTransmission();
}


uint8_t RTC_M5Core2::ReadReg(uint8_t reg)
{
  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(reg);
  Wire1.endTransmission();
  Wire1.requestFrom(BM8563_ADDR, (uint8_t)1);
  uint8_t buf = Wire1.read();
  return buf;
}


bool RTC_M5Core2::isrunning(void)
{
  log_d("[RTC] BM8563 Reg Read: Reg00=%02X Reg01=%02X Reg02=%02X",
    ReadReg(0x00), // should return 0xa8
    ReadReg(0x01), // should return 0x0c
    ReadReg(0x02)  // should return 0x45
  );
  return true;
}


void RTC_M5Core2::GetBm8563Time(void)
{
  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(0x02);
  Wire1.endTransmission();
  Wire1.requestFrom(BM8563_ADDR,(uint8_t)7);
  while(Wire1.available()){
    trdata[0] = Wire1.read();
    trdata[1] = Wire1.read();
    trdata[2] = Wire1.read();
    trdata[3] = Wire1.read();
    trdata[4] = Wire1.read();
    trdata[5] = Wire1.read();
    trdata[6] = Wire1.read();
  }
  DataMask();
  Bcd2asc();
  Str2Time();
}


void RTC_M5Core2::Str2Time(void)
{
  Second = (asc[0] - 0x30) * 10 + asc[1] - 0x30;
  Minute = (asc[2] - 0x30) * 10 + asc[3] - 0x30;
  Hour = (asc[4] - 0x30) * 10 + asc[5] - 0x30;
  /*
  uint8_t Hour;
  uint8_t Week;
  uint8_t Day;
  uint8_t Month;
  uint8_t  Year;
  */
}


void RTC_M5Core2::DataMask()
{
  trdata[0] = trdata[0]&0x7f;    //秒
  trdata[1] = trdata[1]&0x7f;    //分
  trdata[2] = trdata[2]&0x3f;    //时
  trdata[3] = trdata[3]&0x3f;    //日
  trdata[4] = trdata[4]&0x07;    //星期
  trdata[5] = trdata[5]&0x1f;    //月
  trdata[6] = trdata[6]&0xff;    //年
}
/********************************************************************
函 数 名： void Bcd2asc(void)
功 能： bcd 码转换成 asc 码，供Lcd显示用
说 明：
调 用：
入口参数：
返 回 值：无
***********************************************************************/
void RTC_M5Core2::Bcd2asc(void)
{
  uint8_t i,j;
  for (j=0,i=0; i<7; i++){
    asc[j++] =(trdata[i]&0xf0)>>4|0x30 ;/*格式为: 秒 分 时 日 月 星期 年 */
    asc[j++] =(trdata[i]&0x0f)|0x30;
  }
}


uint8_t RTC_M5Core2::Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}

uint8_t RTC_M5Core2::ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;

  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }

  return  ((uint8_t)(bcdhigh << 4) | Value);
}


void RTC_M5Core2::GetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint8_t buf[3] = {0};

  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(0x02);
  Wire1.endTransmission();
  Wire1.requestFrom(BM8563_ADDR,(uint8_t)3);

  while(Wire1.available()){
    buf[0] = Wire1.read();
    buf[1] = Wire1.read();
    buf[2] = Wire1.read();
  }

  RTC_TimeStruct->Seconds = Bcd2ToByte(buf[0]&0x7f);    //秒
  RTC_TimeStruct->Minutes = Bcd2ToByte(buf[1]&0x7f);    //分
  RTC_TimeStruct->Hours   = Bcd2ToByte(buf[2]&0x3f);    //时
}


void RTC_M5Core2::SetTime(RTC_TimeTypeDef* RTC_TimeStruct)
{

  if(RTC_TimeStruct == NULL)
    return;

  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(0x02);
  Wire1.write(ByteToBcd2(RTC_TimeStruct->Seconds));
  Wire1.write(ByteToBcd2(RTC_TimeStruct->Minutes));
  Wire1.write(ByteToBcd2(RTC_TimeStruct->Hours));
  Wire1.endTransmission();

}


void RTC_M5Core2::GetDate(RTC_DateTypeDef* RTC_DateStruct)
{

  uint8_t buf[4] = {0};

  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(0x05);
  Wire1.endTransmission();
  Wire1.requestFrom(BM8563_ADDR,(uint8_t)4);

  while(Wire1.available()){
    buf[0] = Wire1.read();
    buf[1] = Wire1.read();
    buf[2] = Wire1.read();
    buf[3] = Wire1.read();
  }

  RTC_DateStruct->Date    = Bcd2ToByte(buf[0]&0x3f);
  RTC_DateStruct->WeekDay = Bcd2ToByte(buf[1]&0x07);
  RTC_DateStruct->Month   = Bcd2ToByte(buf[2]&0x1f);

  if(buf[2]&0x80){
    RTC_DateStruct ->Year = 1900 + Bcd2ToByte(buf[3]&0xff);
  }else{
    RTC_DateStruct ->Year = 2000  + Bcd2ToByte(buf[3]&0xff);
  }

}


void RTC_M5Core2::SetDate(RTC_DateTypeDef* RTC_DateStruct)
{

  if(RTC_DateStruct == NULL)
    return;
  Wire1.beginTransmission(BM8563_ADDR);
  Wire1.write(0x05);
  Wire1.write(ByteToBcd2(RTC_DateStruct->Date));
  Wire1.write(ByteToBcd2(RTC_DateStruct->WeekDay));

  if(RTC_DateStruct->Year < 2000){
    Wire1.write(ByteToBcd2(RTC_DateStruct->Month) | 0x80);
    Wire1.write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year % 100)));
  } else {
    Wire1.write(ByteToBcd2(RTC_DateStruct->Month) | 0x00);
    Wire1.write(ByteToBcd2((uint8_t)(RTC_DateStruct->Year %100)));
  }
  Wire1.endTransmission();

}

