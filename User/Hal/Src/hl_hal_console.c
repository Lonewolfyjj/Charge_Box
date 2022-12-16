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
#include "hl_hal_uart.h"
#include "hl_util_fifo.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static bool detach_flag = false;

static hl_util_fifo_t fifo;
static char           fifo_buf[64];

/* Private functions ---------------------------------------------------------*/

static void _console_uart_callback(uint8_t data)
{
    hl_util_fifo_write(&fifo, &data, 1);
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_init(void)
{
    hl_hal_usb_cdc_init();
    hl_hal_uart_init(HL_HAL_UART_NUMB_5, 115200);
    hl_util_fifo_init(&fifo, fifo_buf, sizeof(fifo_buf));
    hl_hal_uart_receive_reg(HL_HAL_UART_NUMB_5, _console_uart_callback);
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
    uint32_t  write_bytes;

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            hl_hal_usb_cdc_write(&a, 1, &write_bytes);
            hl_hal_uart_send(HL_HAL_UART_NUMB_5, &a, 1);
        }
        hl_hal_usb_cdc_write((uint8_t*)(str + i), 1, &write_bytes);
        hl_hal_uart_send(HL_HAL_UART_NUMB_5, (uint8_t*)(str + i), 1);
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

    char     ch         = -1;
    uint32_t read_bytes = 0;

    read_bytes = hl_util_fifo_read(&fifo, &ch, 1);
    if (read_bytes == 1) {
        return ch;
    }

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