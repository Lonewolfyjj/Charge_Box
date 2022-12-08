/**
 * @file hl_hal_uart_test.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-17
 * 
 * ██╗  ██╗ ██████╗ ██╗     ██╗  ██╗   ██╗██╗      █████╗ ███╗   ██╗██████╗ 
 * ██║  ██║██╔═══██╗██║     ██║  ╚██╗ ██╔╝██║     ██╔══██╗████╗  ██║██╔══██╗
 * ███████║██║   ██║██║     ██║   ╚████╔╝ ██║     ███████║██╔██╗ ██║██║  ██║
 * ██╔══██║██║   ██║██║     ██║    ╚██╔╝  ██║     ██╔══██║██║╚██╗██║██║  ██║
 * ██║  ██║╚██████╔╝███████╗███████╗██║   ███████╗██║  ██║██║ ╚████║██████╔╝
 * ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝
 * @copyright Copyright (c) 2022 hollyland
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date           <th>Version  <th>Author         <th>Description
 * <tr><td>2022-10-17     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_uart.h"
#include "rtthread.h"
#include "stdlib.h"

#include "hl_hal_gpio.h"
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

static void _uart1_callback(uint8_t data)
{
    rt_kprintf("tx1:%c\n", data);
}

static void _uart2_callback(uint8_t data)
{
    rt_kprintf("tx2:%c\n", data);
}

static void _uart3_callback(uint8_t data)
{
    rt_kprintf("rx:%c\n", data);
}

/* Exported functions --------------------------------------------------------*/
static void hl_hal_uart_test(int argc, char* argv[])  //首先需要把PA8引脚拉高，给电平转换芯片供电。
{
    rt_kprintf("init tx1, Tx:PB6, Rx:PB7\n");
    hl_hal_uart_init(HL_HAL_UART_NUMB_1, atoi(argv[1]));
    hl_hal_uart_receive_reg(HL_HAL_UART_NUMB_1, _uart1_callback);

    rt_kprintf("init tx2, Tx:PA1, Rx:PA0\n");
    hl_hal_uart_init(HL_HAL_UART_NUMB_2, atoi(argv[1]));
    hl_hal_uart_receive_reg(HL_HAL_UART_NUMB_2, _uart2_callback);

    rt_kprintf("init rx, Tx:PB10, Rx:PB11\n");
    hl_hal_uart_init(HL_HAL_UART_NUMB_3, atoi(argv[1]));
    hl_hal_uart_receive_reg(HL_HAL_UART_NUMB_3, _uart3_callback);
}

static void hl_hal_uart_test_send_loop(int argc, char* argv[])  //首先需要把PA8引脚拉高，给电平转换芯片供电。
{
    while (1) {
        hl_hal_uart_send(HL_HAL_UART_NUMB_1, argv[1], strlen(argv[1]));
        hl_hal_uart_send(HL_HAL_UART_NUMB_2, argv[1], strlen(argv[1]));
        hl_hal_uart_send(HL_HAL_UART_NUMB_3, argv[1], strlen(argv[1]));
        rt_thread_mdelay(1000);
    }
}

MSH_CMD_EXPORT(hl_hal_uart_test, hal uart test);

MSH_CMD_EXPORT(hl_hal_uart_test_send_loop, hal uart test send to self loop);
/*
 * EOF
 */