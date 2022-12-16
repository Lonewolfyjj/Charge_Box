/**
 * @file hl_drv_pwm_led.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-14
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
 * <tr><td>2022-11-14     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_HAL_PWM_LED_H__
#define __HL_HAL_PWM_LED_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_drv_pwm_led_type_e {
    HL_DRV_PWM_LED_BOX1,      
    HL_DRV_PWM_LED_BOX2,        
    HL_DRV_PWM_LED_BOX3,
    HL_DRV_PWM_LED_BOX4,
    HL_DRV_PWM_LED_BOX5, //红灯

    HL_DRV_PWM_LED_RX,     
    HL_DRV_PWM_LED_TX2,       
    HL_DRV_PWM_LED_TX1,        
}hl_drv_pwm_led_type_e;

typedef enum _hl_drv_pwm_led_op_cmd_e{
    HL_DRV_PWM_SET_BREATH_MODE,         //呼吸灯模式
    HL_DRV_PWM_SET_BRIGHT_MODE,         //常亮模式
    HL_DRV_PWM_SET_CLOSE_MODE,          //熄灭模式
    HL_DRV_PWM_SET_BREATH_MAX_VAL,      //设置呼吸的最大亮度值
    HL_DRV_PWM_LED_SLEEP_MODE,          //睡眠模式
    HL_DRV_PWM_LED_ACTIVE_MODE          //工作活跃模式
}hl_drv_pwm_led_op_cmd_e;

/* define --------------------------------------------------------------------*/

#define PWM_LED_FUNC_RET_ERR    1
#define PWM_LED_FUNC_RET_OK     0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_pwm_led_init();
uint8_t hl_drv_pwm_led_deinit();
uint8_t hl_drv_pwm_led_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size);

#endif
/*
 * EOF
 */