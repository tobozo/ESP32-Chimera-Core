/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "bsp_codec.h"
#include "bsp_i2c.h"
#include "esp_err.h"
#include "es8311.h"
#include "esp_log.h"

static const char *TAG = "codec";

typedef struct {
    codec_dev_t dev;
    uint8_t dev_addr;
    char *dev_name;
    esp_err_t (*init)(void);
} codec_desc_t;

static uint32_t s_codec_detect_flag = 0;

/**
 * @brief Default init function declaration for aduio CODEC
 */
static esp_err_t es8311_init_default(void);

static codec_desc_t codec_dev =  {CODEC_DEV_ES8311, 0x18, (char*)"ES8311", es8311_init_default };


static esp_err_t es8311_init_default(void)
{
    esp_err_t ret_val = ESP_OK;
    audio_hal_codec_config_t cfg;
    cfg.codec_mode = AUDIO_HAL_CODEC_MODE_DECODE;
    cfg.dac_output = AUDIO_HAL_DAC_OUTPUT_LINE1;

    cfg.i2s_iface.bits    = AUDIO_HAL_BIT_LENGTH_16BITS;
    cfg.i2s_iface.fmt     = AUDIO_HAL_I2S_NORMAL;
    cfg.i2s_iface.mode    = AUDIO_HAL_MODE_SLAVE;
    cfg.i2s_iface.samples = AUDIO_HAL_16K_SAMPLES;

    ret_val |= es8311_codec_init(&cfg);
    ret_val |= es8311_set_bits_per_sample(cfg.i2s_iface.bits);
    ret_val |= es8311_config_fmt((es_i2s_fmt_t)cfg.i2s_iface.fmt);
    ret_val |= es8311_codec_set_voice_volume(75);
    ret_val |= es8311_codec_ctrl_state(cfg.codec_mode, AUDIO_HAL_CTRL_START);

    if (ESP_OK != ret_val) {
        ESP_LOGE(TAG, "Failed initialize codec");
    }

    return ret_val;
}

bool bsp_codec_has_dev(codec_dev_t dev)
{
    if (s_codec_detect_flag & dev) {
        return true;
    } return false;
}

esp_err_t bsp_codec_detect(uint32_t *devices)
{
    if (ESP_OK == bsp_i2c_probe_addr(codec_dev.dev_addr)) {
        *devices |= codec_dev.dev;
        ESP_LOGI(TAG, "Detected codec at 0x%02X. Name : %s",
                  codec_dev.dev_addr, codec_dev.dev_name);
    }
    return 0 == *devices ? ESP_ERR_NOT_FOUND : ESP_OK;
}

esp_err_t bsp_codec_init(audio_hal_iface_samples_t sample_rate)
{
    esp_err_t ret_val = ESP_OK;

    ret_val = bsp_codec_detect(&s_codec_detect_flag);
    if (ESP_OK == ret_val) {
        if (codec_dev.dev & s_codec_detect_flag) {
            ret_val |= codec_dev.init();
            ESP_LOGI(TAG, "init %s", codec_dev.dev_name);
        }
    }
    return ret_val;
}

esp_err_t bsp_codec_set_voice_volume(uint8_t volume)
{
    if (bsp_codec_has_dev(CODEC_DEV_ES8311)) {
        // reduce volume for S3-BOX board
        float v = volume;
        v *= 0.6f;
        v = -0.01f * (v * v) + 2.0f * v;
        return es8311_codec_set_voice_volume((int)v);
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t bsp_codec_set_fmt(audio_hal_iface_format_t fmt)
{
    if (bsp_codec_has_dev(CODEC_DEV_ES8311)) {
        return es8311_config_fmt((es_i2s_fmt_t)fmt);
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t bsp_codec_set_voice_gain(uint8_t channel_mask, uint8_t volume)
{
    return ESP_ERR_NOT_FOUND;
}
