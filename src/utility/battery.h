/*
 * battery.h
 *
 *  Created on: 5 de jul de 2018
 *      Author: mdrjr
 */

#ifndef LIBRARIES_ODROID_GO_SRC_UTILITY_BATTERY_H_
#define LIBRARIES_ODROID_GO_SRC_UTILITY_BATTERY_H_

#include "Arduino.h"
#include "Config.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define BATTERY_RESISTANCE_NUM 2
#define BATTERY_SAMPLES 64
#define BATTERY_VMAX 420
#define BATTERY_VMIN 330
#define BATTERY_CUTOFF 325

class Battery {
public:
	Battery(void);

	void begin();
	double getVoltage();
	int getPercentage();
	void setProtection(bool enable);
	void update();

private:
	esp_adc_cal_characteristics_t _adc_chars;
	bool _enable_protection;
};

#endif /* LIBRARIES_ODROID_GO_SRC_UTILITY_BATTERY_H_ */

