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

#if defined ESP_ARDUINO_VERSION_VAL && ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
  #include <driver/adc.h>
  #include <esp_adc_cal.h>

  // adc1_config_width
  // adc1_config_channel_atten
  // esp_adc_cal_characterize
  // adc1_get_raw
  // esp_adc_cal_raw_to_voltage
  // esp_adc_cal_characteristics_t

#else
  #include <esp_adc/adc_continuous.h>
  #include <esp_adc/adc_oneshot.h>
  #include <esp_adc/adc_cali_scheme.h>
#endif


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
  #if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3,0,0)
    esp_adc_cal_characteristics_t _adc_chars;
  #endif
  bool _enable_protection;
};


