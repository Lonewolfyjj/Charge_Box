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
#include "hl_util_fifo.h"
#include "hl_hal_uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONSOLE_UART "uart1"

#define FIFO_BUFSZ 128

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct rt_semaphore shell_rx_sem; /* defined console rx semaphore*/

static hl_util_fifo_t hl_console_fifo;

static char fifo_buf[FIFO_BUFSZ] = { 0 };

/* Private functions ---------------------------------------------------------*/
//#ifdef RT_USING_CONSOLE

static void hl_hal_console_callback(uint8_t data)
{
    if (1 == hl_util_fifo_write(&hl_console_fifo, &data, 1)) {
        rt_sem_release(&shell_rx_sem);
    }
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_init(void)
{
    hl_util_fifo_init(&hl_console_fifo, fifo_buf, sizeof(fifo_buf));

    /* 初始化串口接收数据的信号量 */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);

    hl_hal_uart_init(HL_HAL_UART_NUMB_1, 115200);

    hl_hal_uart_receive_reg(HL_HAL_UART_NUMB_1, hl_hal_console_callback);
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
    rt_size_t i = 0, size = 0;
    char      a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            hl_hal_uart_send(HL_HAL_UART_NUMB_1, &a, 1);
        }
        hl_hal_uart_send(HL_HAL_UART_NUMB_1, (uint8_t*)(str + i), 1);
    }
}

/**
 * @brief This function is the console input interface
 */
char rt_hw_console_getchar(void)
{
    char ch = -1;

    while (1 != hl_util_fifo_read(&hl_console_fifo, (uint8_t*)&ch, 1)) {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    }

    return ch;
}