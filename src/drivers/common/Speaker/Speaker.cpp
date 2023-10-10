//#define ECC_NO_PRAGMAS
#include "./Speaker.h"


void SPEAKER::begin()
{
  _begun = true;

  #if defined ESP_ARDUINO_VERSION_VAL
    #if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
      ledcSetup(_channel, 0, 13);
      ledcAttachPin(_pin, _channel);
    #else
      ledcAttach( _pin, 0, 13 ); // Note: auto channel ?
    #endif
  #else
    ledcSetup(_channel, 0, 13);
    ledcAttachPin(_pin, _channel);
  #endif

  #ifdef ARDUINO_ODROID_ESP32
    pinMode(25, OUTPUT);
    digitalWrite(25, HIGH);
  #endif
  setBeep(1000, 100);
}

void SPEAKER::end() {
  mute();
  #if defined ESP_ARDUINO_VERSION_VAL
    #if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
      ledcDetachPin(_pin);
    #else
      ledcDetach(_pin);
    #endif
  #else
    ledcDetachPin(_pin);
  #endif
  _begun = false;
}

void SPEAKER::tone(uint16_t frequency)
{
  if(!_begun) begin();
  ledcWriteTone(_channel, frequency);
  ledcWrite(_channel, 0x400 >> _volume);
}

void SPEAKER::tone(uint16_t frequency, uint32_t duration)
{
  tone(frequency);
  _count = millis() + duration;
  speaker_on = 1;
}

void SPEAKER::beep()
{
  if(!_begun) begin();
  tone(_beep_freq, _beep_duration);
}

void SPEAKER::setBeep(uint16_t frequency, uint16_t duration)
{
  _beep_freq = frequency;
  _beep_duration = duration;
}

void SPEAKER::setVolume(uint8_t volume)
{
  _volume = 11 - volume;
}

void SPEAKER::mute()
{
  ledcWriteTone(_channel, 0);
  digitalWrite(_pin, 0);
}

void SPEAKER::update()
{
  if(!_begun) return;
  if(speaker_on) {
    if(millis() > _count) {
      speaker_on = 0;
      mute();
    }
  }
}

void SPEAKER::write(uint8_t value)
{
  dacWrite(_pin, value);
}

void SPEAKER::playMusic(const uint8_t* music_data, uint16_t sample_rate)
{
  uint32_t length = strlen((char*)music_data);
  uint16_t delay_interval = ((uint32_t)1000000 / sample_rate);
  if(_volume != 11) {
    for(int i = 0; i < length; i++) {
      dacWrite(_pin, music_data[i] / _volume);
      delayMicroseconds(delay_interval);
    }

    for(int t = music_data[length - 1] / _volume; t >= 0; t--) {
      dacWrite(_pin, t);
      delay(2);
    }
  }
  // ledcSetup(_channel, 0, 13);
  #if defined ESP_ARDUINO_VERSION_VAL
    #if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
      ledcAttachPin(_pin, _channel);
    #else
      ledcAttach(_pin, 0, 13);
    #endif
  #else
    ledcAttachPin(_pin, _channel);
  #endif
}
