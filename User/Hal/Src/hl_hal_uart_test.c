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

static void _uart3_callback(uint8_t data)
{
    rt_kprintf("uart3:%c\n", data);
}

static void _lpuart_callback(uint8_t data)
{
    rt_kprintf("lpuart:%c\n", data);
}

/* Exported functions --------------------------------------------------------*/
static void hl_hal_uart_test(int argc, char* argv[])  //首先需要把PA8引脚拉高，给电平转换芯片供电。
{
    rt_kprintf("init uart3, Tx:PB10, Rx:PB11\n");
    hl_hal_uart_init(UART_PORT3, 115200);
    hl_hal_uart_receive_reg(UART_PORT3, _uart3_callback);

    rt_kprintf("init lpuart, Tx:PA1, Rx:PA0\n");
    hl_hal_uart_init(LPUART_PORT, 115200);
    hl_hal_uart_receive_reg(LPUART_PORT, _lpuart_callback);
}

MSH_CMD_EXPORT(hl_hal_uart_test, hal uart test);
/*
 * EOF
 */