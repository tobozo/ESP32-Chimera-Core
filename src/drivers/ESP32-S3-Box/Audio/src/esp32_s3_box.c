/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_i2s.h"
#include "bsp_codec.h"
#include "es8311.h"

static const char *TAG = "board";

static const board_res_desc_t g_board_s3_box_res =
{
    //.FUNC_I2C_EN =         (1),
    .GPIO_I2C_SCL =        ((gpio_num_t)18),
    .GPIO_I2C_SDA =        ((gpio_num_t)8),

    //.FUNC_I2S_EN =         (1),
    .GPIO_I2S_LRCK =       ((gpio_num_t)47),
    .GPIO_I2S_MCLK =       ((gpio_num_t)2),
    .GPIO_I2S_SCLK =       ((gpio_num_t)17),
    .GPIO_I2S_SDIN =       ((gpio_num_t)16),
    .GPIO_I2S_DOUT =       ((gpio_num_t)15),
    .CODEC_I2C_ADDR =      0,
    .AUDIO_ADC_I2C_ADDR =  0,

    .IMU_I2C_ADDR =        0,

    .FUNC_PWR_CTRL =       (1),
    .GPIO_PWR_CTRL =       ((gpio_num_t)46),
    .GPIO_PWR_ON_LEVEL =   (1),

    .GPIO_MUTE_NUM =       (gpio_num_t)1,
    .GPIO_MUTE_LEVEL =     1,
};



__attribute__((weak)) void mute_btn_handler(void *arg)
{
    if (g_board_s3_box_res.GPIO_MUTE_LEVEL == gpio_get_level((gpio_num_t)g_board_s3_box_res.GPIO_MUTE_NUM)) {
        ets_printf(DRAM_STR("Mute Off!\n"));
    } else {
        ets_printf(DRAM_STR("Mute On!\n"));
    }
}

esp_err_t bsp_board_s3_box_init(void)
{
    // Mute_Button on ESP32-S3-Box
    gpio_config_t io_conf_key =
    {
      .intr_type    = GPIO_INTR_ANYEDGE,
      .mode         = GPIO_MODE_INPUT,
      .pin_bit_mask = 1ULL << g_board_s3_box_res.GPIO_MUTE_NUM,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .pull_up_en   = GPIO_PULLUP_ENABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_key));

    // Install GPIO ISR service to enable GPIO ISR callback
    gpio_install_isr_service(0);
    ESP_ERROR_CHECK(gpio_isr_handler_add((gpio_num_t)g_board_s3_box_res.GPIO_MUTE_NUM, mute_btn_handler, NULL));
    ESP_ERROR_CHECK(bsp_i2s_init(I2S_NUM_0, 16000));
    ESP_ERROR_CHECK(bsp_codec_init(AUDIO_HAL_16K_SAMPLES));

    return ESP_OK;
}

esp_err_t bsp_board_s3_box_power_ctrl(power_module_t module, bool on)
{
    // Config power control IO
    static esp_err_t bsp_io_config_state = ESP_FAIL;
    if (ESP_OK != bsp_io_config_state) {
        gpio_config_t io_conf =
        {
          .intr_type = GPIO_INTR_DISABLE,
          .mode = GPIO_MODE_OUTPUT,
          .pin_bit_mask = 1ULL << g_board_s3_box_res.GPIO_PWR_CTRL,
          .pull_down_en = GPIO_PULLDOWN_DISABLE,
          .pull_up_en = GPIO_PULLUP_DISABLE
        };
        bsp_io_config_state = gpio_config(&io_conf);
    }

    // Checko IO config result
    if (ESP_OK != bsp_io_config_state) {
        ESP_LOGE(TAG, "Failed initialize power control IO");
        return bsp_io_config_state;
    }

    // Control independent power domain
    switch (module) {

    case POWER_MODULE_AUDIO:
    case POWER_MODULE_ALL:
        gpio_set_level((gpio_num_t)g_board_s3_box_res.GPIO_PWR_CTRL, on ? (g_board_s3_box_res.GPIO_PWR_ON_LEVEL) : (!g_board_s3_box_res.GPIO_PWR_ON_LEVEL));
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

const board_res_desc_t *bsp_board_s3_box_get_res_desc(void)
{
    return &g_board_s3_box_res;
}
