/**
 * @file hl_drv_led.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-24
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
 * <tr><td>2022-12-24     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_HAL_LED_H__
#define __HL_HAL_LED_H__

/* Includes ------------------------------------------------------------------*/

#include "stdint.h"
#include "stdbool.h"
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/


typedef enum _hl_drv_led_op_cmd_e{
    HL_DRV_LED_BOOT_START_MODE,         //Bootloader开始模式
    HL_DRV_LED_BOOT_END_MODE,           //Bootloader结束模式
}hl_drv_led_op_cmd_e;

/* define --------------------------------------------------------------------*/

#define LED_FUNC_RET_ERR    1
#define LED_FUNC_RET_OK     0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_led_init(void);
uint8_t hl_drv_led_deinit(void);
uint8_t hl_drv_led_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size);

#endif
/*
 * EOF
 */