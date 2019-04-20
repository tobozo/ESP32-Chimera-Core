/*
 * battery.cpp
 *
 *  Created on: 5 de jul de 2018
 *      Author: mdrjr
 */

#include "battery.h"

Battery::Battery() {
	this->_enable_protection = false;
}

void Battery::begin() {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_EFUSE_TP, &_adc_chars);
}

double Battery::getVoltage() {
	uint32_t adc_reading = 0;

	for (int i = 0; i < BATTERY_SAMPLES; i++) {
		adc_reading += adc1_get_raw((adc1_channel_t) ADC1_CHANNEL_0);
	}

	adc_reading /= BATTERY_SAMPLES;

	return (double) esp_adc_cal_raw_to_voltage(adc_reading, &_adc_chars) * BATTERY_RESISTANCE_NUM / 1000;
}

int Battery::getPercentage() {

	int res = 101 - (101 / pow(1 + pow(1.33 * ((int)(getVoltage() * 100) - BATTERY_VMIN)/(BATTERY_VMAX - BATTERY_VMIN), 4.5), 3));

	if(res >= 100)
		res = 100;

	return res;
}

void Battery::setProtection(bool enable) {
	this->_enable_protection = enable;
}

void Battery::update() {
	if(this->_enable_protection == true) {
		int curr_voltage = (int)(getVoltage() * 100);

		if(curr_voltage <= BATTERY_CUTOFF) {
			esp_deep_sleep(30 * 60 * 1000000);
			esp_deep_sleep_start();
		}

	}
}

