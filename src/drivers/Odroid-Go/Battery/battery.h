/*
 * battery.h
 *
 *  Created on: 5 de jul de 2018
 *      Author: mdrjr
 */

#pragma once
#define LIBRARIES_ODROID_GO_SRC_UTILITY_BATTERY_H_

#if __has_include(<esp_arduino_version.h>) // platformio has optional esp_arduino_version
  #include <esp_arduino_version.h>
#endif

#define HAS_ADC_CAL_SUPPORT

#if defined ESP_ARDUINO_VERSION_VAL
   #if ESP_ARDUINO_VERSION > ESP_ARDUINO_VERSION_VAL(2,0,14)
    #undef HAS_ADC_CAL_SUPPORT
  #endif
#endif


#include <driver/adc.h>
#include <esp_adc_cal.h>

#define BATTERY_RESISTANCE_NUM 2
#define BATTERY_SAMPLES 64
#define BATTERY_VMAX 420
#define BATTERY_VMIN 330
#define BATTERY_CUTOFF 325

class Battery
{
public:
  Battery(void);

  void begin();
  double getVoltage();
  int getPercentage();
  void setProtection(bool enable);
  void update();

private:
  #if defined HAS_ADC_CAL_SUPPORT
    esp_adc_cal_characteristics_t _adc_chars;
  #endif
  bool _enable_protection;
};

//   #endif
// #endif
