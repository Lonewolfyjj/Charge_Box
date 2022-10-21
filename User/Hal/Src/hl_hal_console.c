/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-06     luzhanghao       the first version
 */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "n32l40x.h"
#include "hl_hal_usb_cdc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static bool detach_flag = false;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_init(void)
{
    hl_hal_usb_cdc_init();
}

/**
 * @brief This function deinit rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_deinit(void)
{
}

/**
 * @brief This function is the console output interface
 */
void rt_hw_console_output(const char* str)
{
    if (detach_flag == true) {
        return;
    }

    rt_size_t i = 0, size = 0;
    char      a = '\r';
    uint32_t write_bytes;

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            hl_hal_usb_cdc_write(&a, 1, &write_bytes);
        }
        hl_hal_usb_cdc_write((uint8_t*)(str + i), 1, &write_bytes);
    }
}

/**
 * @brief This function is the console input interface
 */
char rt_hw_console_getchar(void)
{
    if (detach_flag == true) {
        rt_thread_mdelay(100);
        return -1;
    }

    char ch = -1;
    uint32_t read_bytes = 0;

    hl_hal_usb_cdc_read(&ch, 1, &read_bytes);
    if (read_bytes != 1) {
        rt_thread_mdelay(10);
        return -1;
    }

    return ch;
}

void hl_hal_console_detach(bool flag)
{
    detach_flag = flag;
}