/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/adc.h"
#include "audio_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BOARD_S3_BOX,
} boards_id_t;

typedef enum {
    BOARD_BTN_ID_BOOT  = 0, // The index is the same as the sequence number of the board_button_t array
    BOARD_BTN_ID_PREV  = 1,
    BOARD_BTN_ID_ENTER = 2,
    BOARD_BTN_ID_NEXT  = 3,
} board_btn_id_t;

typedef struct {
    board_btn_id_t id;
    uint32_t vol;      // for adc button
    gpio_num_t io_num; // for gpio button, Set GPIO_NUM_NC to indicate this button is adc button
    bool active_level; // for gpio button
} board_button_t;

typedef struct {
    gpio_num_t row1[4]; //The first row
    gpio_num_t row2[4];
} pmod_pins_t;

typedef struct {
    /**
     * @brief ESP-Dev-Board I2C GPIO defineation
     *
     */
    bool FUNC_I2C_EN     ;
    int GPIO_I2C_SCL    ;
    int GPIO_I2C_SDA    ;

    /**
     * @brief ESP-Dev-Board I2S GPIO defination
     *
     */
    bool FUNC_I2S_EN         ;
    int GPIO_I2S_LRCK       ;
    int GPIO_I2S_MCLK       ;
    int GPIO_I2S_SCLK       ;
    int GPIO_I2S_SDIN       ;
    int GPIO_I2S_DOUT       ;
    int CODEC_I2C_ADDR;
    int AUDIO_ADC_I2C_ADDR;

    int IMU_I2C_ADDR;

    /**
     * @brief ESP32-S3-HMI-DevKit power control IO
     *
     * @note Some power control pins might not be listed yet
     *
     */
    int FUNC_PWR_CTRL       ;
    int GPIO_PWR_CTRL       ;
    int GPIO_PWR_ON_LEVEL   ;

    int GPIO_MUTE_NUM   ;
    int GPIO_MUTE_LEVEL ;

    const pmod_pins_t *PMOD1;
    const pmod_pins_t *PMOD2;

} board_res_desc_t;

/**
 * @brief Power module of dev board. This can be expanded in the future.
 *
 */
typedef enum {
    POWER_MODULE_LCD = 1,       /*!< LCD power control */
    POWER_MODULE_AUDIO,         /*!< Audio PA power control */
    POWER_MODULE_ALL = 0xff,    /*!< All module power control */
} power_module_t;

typedef struct {
    boards_id_t id;
    const char *name;

    /**
     * @brief Special config for dev board
     *
     * @return
     *    - ESP_OK: Success
     *    - Others: Fail
     */
    esp_err_t (*board_init)(void);

    /**
    * @brief Control power of dev board
    *
    * @param module Refer to `power_module_t`
    * @param on Turn on or off specified power module. On if true
    * @return
    *    - ESP_OK: Success
    *    - Others: Fail
    */
    esp_err_t (*board_power_ctrl)(power_module_t module, bool on);

    /**
     * @brief Get board description
     *
     * @return pointer of board_res_desc_t
     */
    const board_res_desc_t *(*board_get_res_desc)(void);

} boards_info_t;

/**
 * @brief Special config for dev board
 *
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t bsp_board_init(void);

/**
 * @brief Get the board information
 *
 * @return pointer of boards_info_t
 */
const boards_info_t *bsp_board_get_info(void);

/**
 * @brief Get board description
 *
 * @return pointer of board_res_desc_t
 */
const board_res_desc_t *bsp_board_get_description(void);

/**
 * @brief Control power of dev board
 *
 * @param module Refer to `power_module_t`
 * @param on Turn on or off specified power module. On if true
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t bsp_board_power_ctrl(power_module_t module, bool on);

#ifdef __cplusplus
}
#endif
