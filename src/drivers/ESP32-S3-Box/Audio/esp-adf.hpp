#pragma once

#if defined ARDUINO_ESP32_S3_BOX

#pragma GCC diagnostic ignored "-Wunused-variable"

#include "../../../Config.h"
#include <driver/i2s.h>

typedef void (*AudioLoop_task_t)(void *pvParameters);
typedef size_t (*is2Writer16Cb_t)( int16_t* data, size_t data_size );
typedef size_t (*is2Writer8Cb_t)( uint8_t* data, size_t data_size );
struct i2s_writer_t { is2Writer8Cb_t write8; is2Writer16Cb_t write16; };

namespace ESP32S3BoxAudio
{

  static bool soundInited = false;
  static uint32_t audioProcessTime = 0;
  static int16_t soundLevel; // average for FFT

  bool init( AudioLoop_task_t audioloop = nullptr, i2s_writer_t *i2sWriter = nullptr );
  bool cfgInit();
  void setMode( const int mode );
  void setSampleRate( uint32_t sample_rate );
  void setDMABufCount( int dma_buf_count );
  void setDMABufLen( int dma_buf_len );
  void setTxPinConfig( i2s_pin_config_t cfg );
  void setI2sConfig( i2s_config_t cfg );
  i2s_pin_config_t getTxPinConfig();
  i2s_config_t getI2SConfig();

  size_t sendSoundData8( uint8_t* dataI, size_t bytes_sent );
  size_t sendSoundData16( int16_t* dataI, size_t bytes_sent );

  static void (*onMuteBtn)(bool muted) = nullptr;

}

#pragma GCC diagnostic pop

#endif
