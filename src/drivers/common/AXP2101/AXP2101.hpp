#pragma once

#define _AXP2101_H_

//#include "utility/I2C_PORT.h"
#include "../I2C/I2CUtil.h"

/* AXP173 device address */
#define AXP2101_ADDR      0x34
#define AW9523_ADDR       0x58
#define AXP173_ADDR_READ  0x69
#define AXP173_ADDR_WRITE 0x68

class AXP2101 : public I2CUtil {
   private:
    uint16_t _getMin(uint16_t a, uint16_t b);
    uint16_t _getMax(uint16_t a, uint16_t b);
    uint16_t _getMid(uint16_t input, uint16_t min, uint16_t max);

   public:
    enum OUTPUT_CHANNEL {
        OP_DLDO1 = 0x99,
    };
    enum ADC_CHANNEL {
        ADC_TS = 0,
        ADC_APS_V,
        ADC_VBUS_C,
        ADC_VBUS_V,
        ADC_ACIN_C,
        ADC_ACIN_V,
        ADC_BAT_C,
        ADC_BAT_V,
    };
    enum CHARGE_CURRENT {
        CHG_100mA = 0,
        CHG_190mA,
        CHG_280mA,
        CHG_360mA,
        CHG_450mA,
        CHG_550mA,
        CHG_630mA,
        CHG_700mA,
        CHG_780mA,
        CHG_880mA,
        CHG_960mA,
        CHG_1000mA,
        CHG_1080mA,
        CHG_1160mA,
        CHG_1240mA,
        CHG_1320mA,
    };
    enum COULOMETER_CTRL {
        COULOMETER_RESET = 5,
        COULOMETER_PAUSE,
        COULOMETER_ENABLE,
    };
    /* Init */
    bool begin(TwoWire* wire = &Wire1);
    /* Power input state */
    bool isACINExist();
    bool isACINAvl();
    bool isVBUSExist();
    bool isVBUSAvl();
    bool getBatCurrentDir();
    bool isAXP173OverTemp();
    bool isCharging();
    bool isBatExist();
    bool isChargeCsmaller();
    /* Power output control */
    void setOutputEnable(OUTPUT_CHANNEL channel, bool state);
    void setOutputVoltage(OUTPUT_CHANNEL channel, uint16_t voltage);
    /* Basic control */
    void powerOFF();  ////
    /* Charge control */
    void setChargeEnable(bool state);
    void setChargeCurrent(CHARGE_CURRENT current);
    /* ADC control */
    void setADCEnable(ADC_CHANNEL channel, bool state);  //
    void setAllADC(bool state);                          //

    /* Coulometer control */
    void setCoulometer(COULOMETER_CTRL option, bool state);
    /* Coulometer data */
    uint32_t getCoulometerChargeData();
    uint32_t getCoulometerDischargeData();
    float getCoulometerData();
    /* BAT data */
    float getBatVoltage();
    float getBatCurrent();
    float getBatLevel();
    float getBatPower();
    /* VBUS data */
    float getVBUSVoltage();
    float getVBUSCurrent();
    /* Temperature data */
    float getAXP173Temp();
    float getTSTemp();
    // -- sleep
    void PrepareToSleep(void);
    void RestoreFromLightSleep(void);
    void DeepSleep(uint64_t time_in_us = 0);
    void LightSleep(uint64_t time_in_us = 0);

    void setBacklight(bool state);

    void coreS3_init();  ////
    void coreS3_AW9523_init();
    void coreS3_VBUS_boost(bool state);
};
