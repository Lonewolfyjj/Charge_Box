/**
 * @file hl_hal_usb_cdc.h
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-20
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
 * <tr><td>2022-10-20     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_HAL_USB_CDC_H__
#define __HL_HAL_USB_CDC_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define HL_HAL_USB_CDC_FUNC_OK 0
#define HL_HAL_USB_CDC_FUNC_ERR 1

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int hl_hal_usb_cdc_init();

int hl_hal_usb_cdc_deinit();

int hl_hal_usb_cdc_read(uint8_t *buf, uint32_t len, uint32_t *p_size);

int hl_hal_usb_cdc_write(uint8_t *buf, uint32_t len, uint32_t *p_size);
#endif
/*
 * EOF
 */