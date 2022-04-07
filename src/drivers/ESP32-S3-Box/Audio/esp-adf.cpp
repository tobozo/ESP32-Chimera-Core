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
      //#define I2S_COMM_FORMAT I2S_COMM_FORMAT_STAND_I2S
    #else
      const i2s_comm_format_t COMM_FORMAT = I2S_COMM_FORMAT_I2S;
      //#define I2S_COMM_FORMAT I2S_COMM_FORMAT_I2S
    #endif
  #else
    const i2s_comm_format_t COMM_FORMAT = I2S_COMM_FORMAT_I2S;
    //#define I2S_COMM_FORMAT I2S_COMM_FORMAT_I2S
  #endif

  i2s_port_t Speak_I2S_NUMBER = I2S_NUM_0;
  //#define Speak_I2S_NUMBER I2S_NUM_0

  const int mode_mic = 0;
  const int mode_spk = 1;

  uint8_t MAX_SOUND = 12;

  uint32_t DATA_SIZE = 1024;
  uint32_t SAMPLE_RATE = 44100;
  int DMA_BUF_LEN = 64;
  int DMA_BUF_COUNT = 8;

  float globalVolume = 0.5f;

  i2s_pin_config_t tx_pin_config =
  {
    .bck_io_num   = I2S_BCK_PIN,
    .ws_io_num    = I2S_LRCK_PIN,
    .data_out_num = I2S_DATA_PIN,
    .data_in_num  = I2S_DATA_IN_PIN
  };

  i2s_config_t i2s_config =
  {
    .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate          = SAMPLE_RATE,
    .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format       = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = COMM_FORMAT,
    .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count        = DMA_BUF_COUNT,
    .dma_buf_len          = DMA_BUF_LEN
  };



};

namespace ESP32S3BoxAudio
{

  #ifdef __cplusplus
  extern "C" {
  #endif
    // load esp-adf components stolen from esp32-s3-box factorydemo
    #include "src/esp32_s3_box.h"

    // overwrite the weak function from esp-adf
    void mute_btn_handler(void *arg)
    {
      muted = gpio_get_level((gpio_num_t)BUTTON_A_PIN) == 1 ? false : true;

      if ( !muted ) {
        log_d("Mute Off");
        muted = false;
      } else {
        log_d("Mute On");
        muted = true;
      }
      if( onMuteBtn ) onMuteBtn(muted);
    }


  #ifdef __cplusplus
  }
  #endif

  SpeakerCfg cfg;
  i2w_writer_t i2sWriter;

  //void AudioLoop(void *pvParameters) { };
  void VuMeterLoop(void *pvParameters) { };

  void sendSoundData16( int16_t* dataI, size_t bytes_sent, size_t bytes_written )
  {
    if( muted ) return;
    i2s_write(cfg.Speak_I2S_NUMBER, (const unsigned char *)dataI, 2 * bytes_sent, &bytes_written, portMAX_DELAY);
    int16_t _soundLevel = 0;
    for( int i=0; i<bytes_sent; i++ ) {
      _soundLevel += dataI[i];
    }
    _soundLevel /=bytes_sent;
    soundLevel = _soundLevel;
  }

  bool init( AudioLoop_task_t audioloop )
  {
    log_d("Init audio");
    esp_err_t esp_err = ESP_OK;
    esp_err |= bsp_board_init();
    esp_err |= bsp_board_power_ctrl(POWER_MODULE_AUDIO, true);

    muted = gpio_get_level((gpio_num_t)BUTTON_A_PIN) == 1 ? false : true;

    if( esp_err == ESP_OK ) {

      setMode( cfg.mode_spk );
      if( !cfgInit() ) {
        return false;
      }

      if( audioloop != nullptr ) {
        log_d("Assigning  audio loop");
        i2sWriter.write = (is2WriterCb_t)sendSoundData16;
        xTaskCreateUniversal( audioloop, "audioLoop", 2048, (void*)&i2sWriter, 1, NULL, 0);
        vTaskDelay(100);
        disableCore0WDT();
      } else {
        log_w("No audio loop assigned");
      }
      //xTaskCreateUniversal( VuMeterLoop, "VuMeterLoop", 2048, NULL, 0, NULL, 0);


      log_i("I2S Started successfully");
      return true;
    } else {
      log_e("Unable to init I2S");
      return false;
    }
  }

/*
  bool InitI2SSpeakOrMic(int mode)
  {
    esp_err_t err = ESP_OK;
    i2s_config_t i2s_config = cfg.getSpeakerConfig(mode);
    i2s_pin_config_t tx_pin_config = cfg.getTxPinConfig();
    err += i2s_driver_uninstall(cfg.Speak_I2S_NUMBER);
    err += i2s_driver_install(cfg.Speak_I2S_NUMBER, &i2s_config, 0, NULL);
    err += i2s_set_pin(cfg.Speak_I2S_NUMBER, &tx_pin_config);
    return err == ESP_OK;
  }
*/

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

  bool cfgInit()
  {
    // TODO: check for audioloop task
    esp_err_t err = ESP_OK;
    err += i2s_driver_uninstall(cfg.Speak_I2S_NUMBER);
    err += i2s_driver_install(cfg.Speak_I2S_NUMBER, &cfg.i2s_config, 0, NULL);
    err += i2s_set_pin(cfg.Speak_I2S_NUMBER, &cfg.tx_pin_config);
    return err == ESP_OK;
  }



}


#endif // defined ARDUINO_ESP32_S3_BOX
