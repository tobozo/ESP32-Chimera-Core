#pragma once
#define _SPEAKER_H_

#include <stdint.h>


#if __has_include(<esp_arduino_version.h>) // platformio has optional esp_arduino_version
  #include <esp_arduino_version.h>
#endif


#if defined ESP_ARDUINO_VERSION_VAL
  #if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
    #include <esp32-hal.h>
  #else // ESP_ARDUINO_VERSION_VAL >= 3.0.0
    #define SOC_DAC_SUPPORTED 1 // why ???
    #include <esp32-hal.h>
    #include <esp32-hal-ledc.h>
    #include <esp32-hal-dac.h>
  #endif
#else
  #include <esp32-hal-dac.h>
#endif


class SPEAKER
{
  public:

    SPEAKER(const int8_t pin=-1, const uint8_t channel=0) :  _pin(pin), _channel(channel), _volume(8), _begun(false) { };
    void begin();
    void end();
    void mute();
    void tone(uint16_t frequency);
    void tone(uint16_t frequency, uint32_t duration);
    void beep();
    void setBeep(uint16_t frequency, uint16_t duration);
    void update();

    void write(uint8_t value);
    void setVolume(uint8_t volume);
    void playMusic(const uint8_t *music_data, uint16_t sample_rate);

  private:
    int8_t _pin = -1;     // SPEAKER_PIN
    uint8_t _channel = 0; // TONE_PIN_CHANNEL
    uint32_t _count = 0;
    uint8_t _volume = 0;
    uint16_t _beep_duration = 0;
    uint16_t _beep_freq = 0;
    bool _begun = false;
    bool speaker_on = false;
};
