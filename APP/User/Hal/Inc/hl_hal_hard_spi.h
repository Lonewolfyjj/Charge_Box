/**
 * @file spi_flash.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-29
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
 * <tr><td>2022-11-29     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _HL_HAL_HARD_SPI_H_
#define _HL_HAL_HARD_SPI_H_

/* Includes ------------------------------------------------------------------*/

#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define FLASH_SPI                SPI2
#define FLASH_SPI_CLK            RCC_APB2_PERIPH_SPI2

//SCK引脚
#define FLASH_SPI_SCK_PIN        GPIO_PIN_13 
#define FLASH_SPI_SCK_GPIO_PORT  GPIOB      
#define FLASH_SPI_SCK_GPIO_CLK   RCC_APB2_PERIPH_GPIOB

//MISO引脚
#define FLASH_SPI_MISO_PIN       GPIO_PIN_14 
#define FLASH_SPI_MISO_GPIO_PORT GPIOB      
#define FLASH_SPI_MISO_GPIO_CLK  RCC_APB2_PERIPH_GPIOB

//MOSI引脚
#define FLASH_SPI_MOSI_PIN       GPIO_PIN_15 
#define FLASH_SPI_MOSI_GPIO_PORT GPIOB      
#define FLASH_SPI_MOSI_GPIO_CLK  RCC_APB2_PERIPH_GPIOB

//CS(NSS)引脚 片选选普通GPIO即可
#define FLASH_CS_PIN             GPIO_PIN_12 
#define FLASH_CS_GPIO_PORT       GPIOB      
#define FLASH_CS_GPIO_CLK        RCC_APB2_PERIPH_GPIOB

#define HARD_SPI_CS_LOW()     GPIO_ResetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)
#define HARD_SPI_CS_HIGH()    GPIO_SetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)

/* variables -----------------------------------------------------------------*/

#define HARD_SPI_WAIT_TIMEOUT   1000000
#define HARD_SPI_FUNC_RET_ERR   0xffff
#define HARD_SPI_FUNC_RET_OK    0

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_hal_hard_spi_init(void);
uint8_t hl_hal_hard_spi_send_recv(uint8_t byte);
void hl_hal_hard_spi_deinit(void);

#endif
/*
 * EOF
 */







