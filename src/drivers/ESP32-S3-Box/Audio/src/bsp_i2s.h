/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#if __has_include(<esp_arduino_version.h>)
  #include <esp_arduino_version.h>
#endif

#if defined ESP_ARDUINO_VERSION_VAL
  #if ESP_ARDUINO_VERSION <= ESP_ARDUINO_VERSION_VAL(2,0,14)
    #include "driver/i2s.h"
  #else
    #include "driver/i2s_std.h"
  #endif
#else
  #include "driver/i2s.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init I2S bus
 *
 * @param i2s_num I2S port num
 * @param sample_rate Audio sample rate. For I2S signal, it refers to LRCK/WS frequency
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t bsp_i2s_init(i2s_port_t i2s_num, uint32_t sample_rate);

/**
 * @brief Deinit I2S bus
 *
 * @param i2s_num I2S port num
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t bsp_i2s_deinit(i2s_port_t i2s_num);


#ifdef __cplusplus
}
#endif
