#pragma once
#define __RTC_H__

#include <Wire.h>

/*\
 *
 * This RTC driver is useful for both M5Core2 and M5StickC
 * but is very commonly used so it has been modified to
 * allow custom I2C pins on RTC.begin(), choosing between
 * Wire and Wire1, and setting the frequency
 *
\*/


typedef struct
{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
} RTC_TimeTypeDef;


typedef struct
{
  uint8_t WeekDay;
  uint8_t Month;
  uint8_t Date;
  uint16_t Year;
} RTC_DateTypeDef;


class RTC_BM8563
{
  public:
    RTC_BM8563();

    // default uses Wire1
    void begin( int i2c_port=-1, int pin_sda=-1, int pin_scl=-1, int freq=-1 );
    void GetBm8563Time(void);

    void SetTime(RTC_TimeTypeDef* RTC_TimeStruct);
    void SetDate(RTC_DateTypeDef* RTC_DateStruct);
    void SetData(RTC_DateTypeDef* RTC_DateStruct) { SetDate( RTC_DateStruct ); } // aliasing for backwards compat with M5Core2.h and M5StickC.h

    void GetTime(RTC_TimeTypeDef* RTC_TimeStruct);
    void GetDate(RTC_DateTypeDef* RTC_DateStruct);
    void GetData(RTC_DateTypeDef* RTC_DateStruct) { GetDate( RTC_DateStruct ); } // aliasing for backwards compat with M5Core2.h and M5StickC.h

    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Week;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
    uint8_t DateString[9];
    uint8_t TimeString[9];
    uint8_t asc[14];

  private:

    int PIN_SDA = 21;
    int PIN_SCL = 22;
    int I2C_PORT = 1; // defaults to: Wire1
    uint8_t BM8563_ADDR = 0x51; // BM8563 I2C address

    uint8_t trdata[7];

    void Bcd2asc(void);
    void DataMask();
    void Str2Time(void);
    void WriteReg(uint8_t reg, uint8_t data);

    uint8_t ReadReg(uint8_t reg);
    uint8_t Bcd2ToByte(uint8_t Value);
    uint8_t ByteToBcd2(uint8_t Value);

};
