#pragma once


#include "../../../Config.h"
#include <driver/i2s.h>

typedef void (*AudioLoop_task_t)(void *pvParameters);
typedef void (*is2WriterCb_t)( int16_t* data, size_t data_size, size_t bytes_written );
struct i2w_writer_t { is2WriterCb_t write; };

namespace ESP32S3BoxAudio
{
  static bool soundInited = false;
  static uint32_t audioProcessTime = 0;
  static int16_t soundLevel;

  bool init( AudioLoop_task_t audioloop = nullptr  );
  bool cfgInit();
  void setMode( const int mode );
  void setSampleRate( uint32_t sample_rate );
  void setDMABufCount( int dma_buf_count );
  void setDMABufLen( int dma_buf_len );
  void setTxPinConfig( i2s_pin_config_t cfg );
  void setI2sConfig( i2s_config_t cfg );
  i2s_pin_config_t getTxPinConfig();
  i2s_config_t getI2SConfig();

}
