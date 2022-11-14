/**
 * @file hl_hal_pwm.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-11
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
 * <tr><td>2022-11-11     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_HAL_LOWPOWER_H__
#define __HL_HAL_LOWPOWER_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

/**
 * @brief 定时器通道枚举参数，四个通道，对应一个字节的四个位
 * @date 2022-11-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-14      <td>yijiujun     <td>新建
 * </table>
 */
typedef enum _hl_hal_pwm_tim_channel_num_e {
    HL_HAL_PWM_TIM_CHANNEL_1 = 0x01,
    HL_HAL_PWM_TIM_CHANNEL_2 = 0x02,
    HL_HAL_PWM_TIM_CHANNEL_3 = 0x04,
    HL_HAL_PWM_TIM_CHANNEL_4 = 0x08
}hl_hal_pwm_tim_channel_e;

/**
 * @brief 设置一个定时器输出PWM所需要的参数
 * @date 2022-11-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-14      <td>yijiujun     <td>新建
 * </table>
 */
typedef struct _hl_hal_pwm_timx_info_st{
    hl_hal_pwm_tim_channel_e tim_channel;
    uint16_t max_ccr_val;
    TIM_Module *timx;   
}hl_hal_pwm_timx_info_st;
/* define --------------------------------------------------------------------*/

#define dbg_printf  rt_kprintf
#define PWM_FUNC_RET_ERR     1
#define PWM_FUNC_RET_OK      0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions ----------------------------------------rt_kprintf----------------*/

/**
 * @brief 初始化一个定时器，定时器通道配置使能，PWM输出使能
 * @param [in] tim_info 
 * @return uint8_t 
 * @date 2022-11-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-14      <td>yijiujun     <td>新建
 * </table>
 */
uint8_t hl_hal_pwm_init(hl_hal_pwm_timx_info_st *tim_info);

/**
 * @brief 定时器去初始化
 * @param [in] tim_info 
 * @date 2022-11-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-14      <td>yijiujun     <td>新建
 * </table>
 */
void hl_hal_pwm_deinit(hl_hal_pwm_timx_info_st *tim_info);

/**
 * @brief 设置每一个通道的CCR值，即占空比大小
 * @param [in] tim_info 
 * @param [in] ccr_val 
 * @return uint8_t 
 * @date 2022-11-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-14      <td>yijiujun     <td>新建
 * </table>
 */
uint8_t hl_hal_pwm_set_ccr_val(hl_hal_pwm_timx_info_st *tim_info, uint16_t ccr_val);

#endif

/*
 * EOF
 */