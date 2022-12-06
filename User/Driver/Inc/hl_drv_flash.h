/**
 * @file hl_drv_flash.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-25
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
 * <tr><td>2022-10-25     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_DRV_FLASH_H__
#define __HL_DRV_FLASH_H__

/* Includes ------------------------------------------------------------------*/

#include "hl_hal_soft_spi.h"
#include "hl_hal_hard_spi.h"

/* typedef -------------------------------------------------------------------*/

/* define --------------------------------------------------------------------*/

/* FLASH的驱动类型，有软件模拟SPI驱动（HL_DRV_FLASH_TYPE = 0）| 硬件SPI驱动（HL_DRV_FLASH_TYPE = 1） */
#define HL_DRV_FLASH_TYPE       1

//#define W25XXX_ID               0xEF3015    // W25X16
//#define W25XXX_ID               0xEF4015    // W25Q16
//#define W25XXX_ID               0xEF4018    // W25Q128
#define W25XXX_ID               0xef4016    //W25Q32

#define debug_printf            rt_kprintf
#define FLASH_RET_OK            0
#define FLASH_RET_ERR           1

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_drv_flash_init(void);
int hl_drv_flash_deinit(void);
int hl_drv_flash_read(uint32_t addr, uint8_t *r_data, uint32_t len);
int hl_drv_flash_write(uint32_t addr, uint8_t* w_data, uint32_t len);
uint8_t hl_drv_flash_erase_sector(uint32_t addr);
void hl_drv_flash_erase_chip(void);

#endif
/*
 * EOF
 */
