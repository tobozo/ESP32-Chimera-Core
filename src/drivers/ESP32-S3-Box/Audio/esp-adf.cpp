#include "esp-adf.hpp"

#if defined ARDUINO_ESP32_S3_BOX

#include <esp32-hal.h>
#include <esp_arduino_version.h>

struct SpeakerCfg
{
  // handle I2S migration across SDK versions
  #ifdef ESP_ARDUINO_VERSION_VAL
    #if defined ESP_ARDUINO_VERSION && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
      const i2s_comm_format_t COMM_FORMAT = I2S_COMM_FORMAT_STAND_I2S;
    #else
      const i2s_comm_format_t COMM_FORMAT = I2S_COMM_FORMAT_I2S;
    #endif
  #else
    const i2s_comm_format_t COMM_FORMAT = I2S_COMM_FORMAT_I2S;
  #endif

  const gpio_num_t mute_led_state = GPIO_NUM_1;

  i2s_port_t Speak_I2S_NUMBER = I2S_NUM_0;

  const int mode_mic = 0;
  const int mode_spk = 1;

  bool muted = false;

  uint32_t sample_rate = 44100;
  int dma_buf_len      = 64;
  int dma_buf_count    = 8;
  float globalVolume   = 0.5f;


  i2s_config_t i2s_config =
  {
    .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate          = sample_rate,
    .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format       = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = COMM_FORMAT,
    .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count        = dma_buf_count,
    .dma_buf_len          = dma_buf_len
  };


  i2s_pin_config_t tx_pin_config =
  {
    .bck_io_num   = I2S_BCK_PIN,
    .ws_io_num    = I2S_LRCK_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num  = I2S_DATA_IN_PIN
  };


};


namespace ESP32S3BoxAudio
{

  SpeakerCfg cfg;
  i2s_writer_t _defaultI2sWriter = { sendSoundData8, sendSoundData16 };
  void *i2s_writer = nullptr;
  TaskHandle_t *loop_handle = nullptr;
  AudioLoop_task_t *loop_task = nullptr;

  extern "C" {

    // load esp-adf components stolen from esp32-s3-box factorydemo
    #include "src/esp32_s3_box.h"

    // override the ISR weak function from esp-adf layer
    void mute_btn_handler(void *arg)
    {
      static int volume;
      // unlike the mute led state, the mute button state can't be read
      cfg.muted = gpio_get_level( cfg.mute_led_state ) == 1 ? false : true;

      if ( !cfg.muted ) {
        log_i("Mute Off");
      } else {
        log_i("Mute On");
      }

      if( onMuteBtn ) onMuteBtn(cfg.muted);
    }

  }


  bool init( AudioLoop_task_t audioloop, i2s_writer_t *i2sWriter )
  {
    log_i("Init audio");
    esp_err_t esp_err = ESP_OK;
    esp_err |= bsp_board_init();
    esp_err |= bsp_board_power_ctrl(POWER_MODULE_AUDIO, true);

    cfg.muted = gpio_get_level( cfg.mute_led_state ) == 1 ? false : true;

    if( audioloop != nullptr ) {
      log_i("Attaching user audio loop task");
      loop_task = &audioloop;
    }

    if( i2sWriter ) {
      log_i("Attaching user i2sWriter");
      i2s_writer = (void*)i2sWriter;
    } else {
      log_i("Attaching default i2sWriter");
      i2s_writer = (void*)&_defaultI2sWriter;
    }

    if( esp_err == ESP_OK ) {

      setMode( cfg.mode_spk );

      if( !cfgInit() ) {
        log_e("Failed to init audio config, aborting");
        return false;
      }

      if( loop_task != nullptr ) {
        log_d("Starting audio loop task");
        xTaskCreateUniversal( *loop_task, "audioLoop", 2048, i2s_writer, 1, loop_handle, 0);
        vTaskDelay(100);
        disableCore0WDT();
        log_i("I2S Started successfully with audio loop task");
      } else {
        log_w("I2S Started successfully (no audio loop attached)");
      }

      return true;
    } else {
      log_e("Unable to init I2S");
      return false;
    }
  }


  bool cfgInit()
  {
    // TODO: check for audioloop task
    esp_err_t err = ESP_OK;
    err += i2s_driver_uninstall(cfg.Speak_I2S_NUMBER);
    err += i2s_driver_install(cfg.Speak_I2S_NUMBER, &cfg.i2s_config, 0, NULL);
    err += i2s_set_pin(cfg.Speak_I2S_NUMBER, &cfg.tx_pin_config);
    return err == ESP_OK;
  }


  size_t sendSoundData8( uint8_t* dataI, size_t bytes_sent )
  {
    int16_t _soundLevel = 0;
    size_t bytes_written;
    int16_t *DataPtr16 = (int16_t *)dataI;
    for( int i=0; i<bytes_sent/2; i++ ) {
      _soundLevel += DataPtr16[i];
    }
    _soundLevel /= bytes_sent/2;
    soundLevel = _soundLevel;
    if( cfg.muted ) return bytes_sent;
    i2s_write(cfg.Speak_I2S_NUMBER, dataI, bytes_sent, &bytes_written, portMAX_DELAY);
    return bytes_written;
  }

  size_t sendSoundData16( int16_t* dataI, size_t bytes_sent )
  {
    int16_t _soundLevel = 0;
    size_t bytes_written;
    for( int i=0; i<bytes_sent; i++ ) {
      _soundLevel += dataI[i];
    }
    _soundLevel /=bytes_sent;
    soundLevel = _soundLevel;
    if( cfg.muted ) return 2*bytes_sent;
    i2s_write(cfg.Speak_I2S_NUMBER, (const unsigned char *)dataI, 2 * bytes_sent, &bytes_written, portMAX_DELAY);
    return bytes_written;
  }


  void setMode( const int mode )
  {
    if (mode == cfg.mode_mic) {
      cfg.i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    } else {
      cfg.i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
      cfg.i2s_config.use_apll = false;
      cfg.i2s_config.tx_desc_auto_clear = true;
    }
  }


  void setSampleRate( uint32_t sample_rate )
  {
    cfg.i2s_config.sample_rate = sample_rate;
  }


  void setDMABufCount( int dma_buf_count )
  {
    cfg.i2s_config.dma_buf_count = dma_buf_count;
  }


  void setDMABufLen( int dma_buf_len )
  {
    cfg.i2s_config.dma_buf_len = dma_buf_len;
  }


  void setTxPinConfig( i2s_pin_config_t tx_pin_config )
  {
    cfg.tx_pin_config = tx_pin_config;
  }


  i2s_pin_config_t getTxPinConfig()
  {
    //if( I2S_MCLK_PIN >=0 ) tx_pin_config.mck_io_num = I2S_MCLK_PIN;
    return cfg.tx_pin_config;
  }


  void setI2sConfig( i2s_config_t i2s_config )
  {
    cfg.i2s_config = i2s_config;
  }


  i2s_config_t getI2SConfig()
  {
    return cfg.i2s_config;
  }






}


#endif // defined ARDUINO_ESP32_S3_BOX
