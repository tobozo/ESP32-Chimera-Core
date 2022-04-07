/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_i2s.h"
#include "bsp_codec.h"

//#include "button.h"
//#include "bsp_btn.h"

static const char *TAG = "board";

static const board_button_t g_btns[] = {
    {BOARD_BTN_ID_BOOT, 0,      GPIO_NUM_0,    0},
};
/*
static const pmod_pins_t g_pmod[2] = {
    {
        {(gpio_num_t)9,  (gpio_num_t)43, (gpio_num_t)44, (gpio_num_t)14},
        {(gpio_num_t)10, (gpio_num_t)11, (gpio_num_t)13, (gpio_num_t)12},
    },
    {
        {(gpio_num_t)38, (gpio_num_t)39, (gpio_num_t)40, (gpio_num_t)41},
        {(gpio_num_t)42, (gpio_num_t)21, (gpio_num_t)19, (gpio_num_t)20},
    },
};
*/
static const board_res_desc_t g_board_s3_box_res = {

    //.FUNC_I2C_EN =     (1),
    .GPIO_I2C_SCL =    (GPIO_NUM_18),
    .GPIO_I2C_SDA =    (GPIO_NUM_8),

    //.FUNC_I2S_EN =         (1),
    .GPIO_I2S_LRCK =       (GPIO_NUM_47),
    .GPIO_I2S_MCLK =       (GPIO_NUM_2),
    .GPIO_I2S_SCLK =       (GPIO_NUM_17),
    .GPIO_I2S_SDIN =       (GPIO_NUM_16),
    .GPIO_I2S_DOUT =       (GPIO_NUM_15),
    .CODEC_I2C_ADDR = 0,
    .AUDIO_ADC_I2C_ADDR = 0,

    .IMU_I2C_ADDR = 0,

    .FUNC_PWR_CTRL =       (1),
    .GPIO_PWR_CTRL =       (GPIO_NUM_46),
    .GPIO_PWR_ON_LEVEL =   (1),

    .GPIO_MUTE_NUM =   GPIO_NUM_1,
    .GPIO_MUTE_LEVEL = 1,

    //.PMOD1 = &g_pmod[0],
    //.PMOD2 = &g_pmod[1],
};



__attribute__((weak)) void mute_btn_handler(void *arg)
{
    if (g_board_s3_box_res.GPIO_MUTE_LEVEL == gpio_get_level((gpio_num_t)g_board_s3_box_res.GPIO_MUTE_NUM)) {
        ets_printf(DRAM_STR("Mute Off"));
    } else {
        ets_printf(DRAM_STR("Mute On"));
    }
}

esp_err_t bsp_board_s3_box_init(void)
{
//     /*!< Mute_Button on ESP32-S3-Box */
//     gpio_config_t io_conf_key =
//     {
//       .intr_type = GPIO_INTR_ANYEDGE,
//       .mode = GPIO_MODE_INPUT,
//       .pin_bit_mask = 1ULL << g_board_s3_box_res.GPIO_MUTE_NUM,
//       .pull_down_en = GPIO_PULLDOWN_DISABLE,
//       .pull_up_en = GPIO_PULLUP_ENABLE
//     };
//     ESP_ERROR_CHECK(gpio_config(&io_conf_key));
//
//     /* Install GPIO ISR service to enable GPIO ISR callback */
//     gpio_install_isr_service(0);
//     ESP_ERROR_CHECK(gpio_isr_handler_add((gpio_num_t)g_board_s3_box_res.GPIO_MUTE_NUM, mute_btn_handler, NULL));

    //bsp_btn_init_default();

    /**
     * @brief Initialize I2S and audio codec
     *
     * @note Actually the sampling rate can be reconfigured.
     *       `MP3GetLastFrameInfo` can fill the `MP3FrameInfo`, which includes `samprate`.
     *       So theoretically, the sampling rate can be dynamically changed according to the MP3 frame information.
     */
    ESP_ERROR_CHECK(bsp_i2s_init(I2S_NUM_0, 16000));
    ESP_ERROR_CHECK(bsp_codec_init(AUDIO_HAL_16K_SAMPLES));

    return ESP_OK;
}

esp_err_t bsp_board_s3_box_power_ctrl(power_module_t module, bool on)
{
    /* Config power control IO */
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

    /* Checko IO config result */
    if (ESP_OK != bsp_io_config_state) {
        ESP_LOGE(TAG, "Failed initialize power control IO");
        return bsp_io_config_state;
    }

    /* Control independent power domain */
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
