/**
 * @file hl_hal_soft_spi.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
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
 * <tr><td>2022-10-17     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _HL_HAL_SPI_SOFT_H_
#define _HL_HAL_SPI_SOFT_H_
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stdint.h"
#include "n32l40x.h"
#include <rtthread.h>
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
#define SPI_GPIOX_ODR_OFFSET_Addr       0x14
#define SPI_GPIOX_IDR_OFFSET_Addr       0x10
#define SPI_GPIOX_BASE                  GPIOB_BASE

#define SPI_PIN_OUT(pin_count)      (*((unsigned int *)(0x42000000 + ((SPI_GPIOX_BASE + SPI_GPIOX_ODR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))
#define SPI_PIN_IN(pin_count)       (*((unsigned int *)(0x42000000 + ((SPI_GPIOX_BASE + SPI_GPIOX_IDR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))

#define SPI_MOSI   SPI_PIN_OUT(15)
#define SPI_MISO   SPI_PIN_IN(14)
#define SPI_SCK    SPI_PIN_OUT(13)
#define SPI_CS     SPI_PIN_OUT(12)
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_hal_soft_spi_init(void);
uint8_t hl_hal_soft_spi_send_recv(uint8_t wdata);

#endif
/*
 * EOF
 */