/*
 * Copyright (c) 2006-2021, Hollyland Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-21     luzhanghao       the first version
 */
#ifndef _HAL_INC_HL_HAL_GPIO_H_
#define _HAL_INC_HL_HAL_GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include "rtdef.h"

/* Exported types ------------------------------------------------------------*/
typedef enum _HL_GPIO_PORT_E
{
    GPIO_USB_CC = 0,  // high（device），low（host）
    GPIO_CHG_STAT,    // 充电状态检测
    GPIO_VBUS_DET,    // 是否插入充电器检测

    GPIO_TX1_OCB,  // 过流检测
    GPIO_TX2_OCB,
    GPIO_RX_OCB,

    GPIO_PWR_5V_EN,   // 升压控制
    GPIO_TX1_PWR_EN,  // 过流芯片控制
    GPIO_TX2_PWR_EN,
    GPIO_RX_PWR_EN,

    GPIO_PWR_1V8_EN,  // 1.8V电压输出
    GPIO_SN1_EN,      // 串口电平转换
    GPIO_SN2_EN,

    GPIO_USB_OTG,  // 充电芯片控制（host、device）
    GPIO_LED_R,
    GPIO_LED_G,
    GPIO_LED_B,

    GPIO_CHARGE_ENABLE,  // 充电控制

    GPIO_WAKEUP_TX1,  // TX1唤醒
    GPIO_WAKEUP_TX2,  // TX2唤醒
    GPIO_WAKEUP_RX,   // RX唤醒

    USER_GPIO_NUMBER,
} HL_GPIO_PORT_E;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// @brief This function init a gpio with a specified enum value.
rt_err_t hl_hal_gpio_init(HL_GPIO_PORT_E gpio_index);

// @brief This function deinit a gpio with a specified enum value.
rt_err_t hl_hal_gpio_deinit(HL_GPIO_PORT_E gpio_index);

// @brief This function set a gpio with a specified enum value.
void hl_hal_gpio_high(HL_GPIO_PORT_E gpio_index);

// @brief This function reset a gpio with a specified enum value.
void hl_hal_gpio_low(HL_GPIO_PORT_E gpio_index);

// @brief This function read a gpio with a specified enum value.
int hl_hal_gpio_read(HL_GPIO_PORT_E gpio_index);

#endif /* _HAL_INC_HL_HAL_GPIO_H_ */
