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
typedef struct {
    uint8_t spi_mosi_pin_num;
    uint8_t spi_miso_pin_num;
    uint8_t spi_sck_pin_num;
    uint8_t spi_cs_pin_num;
    uint16_t spi_mosi_pin;
    uint16_t spi_miso_pin;
    uint16_t spi_sck_pin;
    uint16_t spi_cs_pin;
    uint32_t gpiox_base;
    uint32_t rcc_periph_gpiox;
    GPIO_Module *gpiox;
}hl_hal_soft_spi_info;

/* define --------------------------------------------------------------------*/
#define SPI_GPIOX_ODR_OFFSET_Addr       0x14
#define SPI_GPIOX_IDR_OFFSET_Addr       0x10
#define SPI_GPIOX_BASE                  GPIOB_BASE

#define SPI_PIN_OUT(pin_count, gpiox_base)      (*((unsigned int *)(0x42000000 + ((gpiox_base + SPI_GPIOX_ODR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))
#define SPI_PIN_IN(pin_count, gpiox_base)       (*((unsigned int *)(0x42000000 + ((gpiox_base + SPI_GPIOX_IDR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))

#define SPI_MOSI(pin_count, gpiox_base)   SPI_PIN_OUT(pin_count, gpiox_base)
#define SPI_MISO(pin_count, gpiox_base)   SPI_PIN_IN(pin_count, gpiox_base)
#define SPI_SCK(pin_count, gpiox_base)    SPI_PIN_OUT(pin_count, gpiox_base)
#define SPI_CS(pin_count, gpiox_base)     SPI_PIN_OUT(pin_count, gpiox_base)

#define HL_HAL_SPI_CS_LOW(pin_count, gpiox_base)    (SPI_CS(pin_count, gpiox_base) = 0)
#define HL_HAL_SPI_CS_HIGH(pin_count, gpiox_base)   (SPI_CS(pin_count, gpiox_base) = 1) 

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_hal_soft_spi_init(hl_hal_soft_spi_info *spi_info);
uint8_t hl_hal_soft_spi_send_recv(hl_hal_soft_spi_info *spi_info, uint8_t wdata);
void hl_hal_soft_spi_deinit(hl_hal_soft_spi_info *spi_info);

#endif
/*
 * EOF
 */