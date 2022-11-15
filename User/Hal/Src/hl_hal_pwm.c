/**
 * @file hl_hal_pwm.c
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
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_pwm.h"
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

// the Capture Compare Register of  initial val 

#define TIM_CCR_INIT_VAL    0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

static uint8_t _hl_hal_pwm_tim_config(TIM_Module* timx, uint16_t period_ccr_val)
{
    uint16_t prescaler_val = 0;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;

    /* PCLK1 = HCLK/2 */
    RCC_ConfigPclk1(RCC_HCLK_DIV2);

    /* TIM1 and TIM8 clock enable */
    if (timx == TIM1) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_TIM1, ENABLE);
    } else if (timx == TIM2) {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
    } else if (timx == TIM3) {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM3, ENABLE);
    } else if (timx == TIM4) {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM4, ENABLE);
    } else if (timx == TIM5) {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM5, ENABLE);
    } else if (timx == TIM8) {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_TIM8, ENABLE);
    } else if (timx == TIM9) {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM9, ENABLE);
    } else {
        dbg_printf("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
        return PWM_FUNC_RET_ERR;
    }

    /* Compute the prescaler value, SystemCoreClock 48000000*/
    prescaler_val = (uint16_t)(SystemCoreClock / 12000000) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.Period = period_ccr_val - 1;            //定时器重装载值，也就是CCR的最大值
    TIM_TimeBaseStructure.Prescaler = prescaler_val - 1;          //1微秒产生一个脉冲
    TIM_TimeBaseStructure.ClkDiv = 0;
    TIM_TimeBaseStructure.CntMode = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(timx, &TIM_TimeBaseStructure);
    return PWM_FUNC_RET_OK;
}

static void _hl_hal_pwm_channelx_config(TIM_Module *timx, hl_hal_pwm_tim_channel_e channel_num)
{
    OCInitType tim_oc_init_struct;

    tim_oc_init_struct.OcMode      = TIM_OCMODE_PWM1;
    tim_oc_init_struct.OutputState = TIM_OUTPUT_STATE_ENABLE;

    tim_oc_init_struct.Pulse = TIM_CCR_INIT_VAL;                  //CCR的初始值
    tim_oc_init_struct.OcPolarity  = TIM_OC_POLARITY_LOW;

    if (channel_num & HL_HAL_PWM_TIM_CHANNEL_1) {
        TIM_InitOc1(timx, &tim_oc_init_struct);
        TIM_ConfigOc1Preload(timx, TIM_OC_PRE_LOAD_ENABLE);
    }

    if (channel_num & HL_HAL_PWM_TIM_CHANNEL_2) {
        TIM_InitOc2(timx, &tim_oc_init_struct);
        TIM_ConfigOc2Preload(timx, TIM_OC_PRE_LOAD_ENABLE);
    }

    if (channel_num & HL_HAL_PWM_TIM_CHANNEL_3) {
        TIM_InitOc3(timx, &tim_oc_init_struct);
        TIM_ConfigOc3Preload(timx, TIM_OC_PRE_LOAD_ENABLE);
    }

    if (channel_num & HL_HAL_PWM_TIM_CHANNEL_4) {
        TIM_InitOc4(timx, &tim_oc_init_struct);
        TIM_ConfigOc4Preload(timx, TIM_OC_PRE_LOAD_ENABLE);
    }
}

/* Exported functions --------------------------------------------------------*/

uint8_t hl_hal_pwm_init(hl_hal_pwm_timx_info_st *tim_info)
{
    uint8_t ret;
    ret = _hl_hal_pwm_tim_config(tim_info->timx, tim_info->max_ccr_val);
    if (ret == PWM_FUNC_RET_ERR) {
        return PWM_FUNC_RET_ERR;
    }
    _hl_hal_pwm_channelx_config(tim_info->timx, tim_info->tim_channel);
    
    TIM_ConfigArPreload(tim_info->timx, ENABLE);

    TIM_Enable(tim_info->timx, ENABLE);

    return PWM_FUNC_RET_OK;
}

uint8_t hl_hal_pwm_deinit(hl_hal_pwm_timx_info_st *tim_info)
{
    if (tim_info == RT_NULL) {
        dbg_printf("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
        return PWM_FUNC_RET_ERR;
    }

    TIM_ConfigArPreload(tim_info->timx, DISABLE);
    TIM_Enable(tim_info->timx, DISABLE);
    return PWM_FUNC_RET_OK;
}

uint8_t hl_hal_pwm_set_ccr_val(hl_hal_pwm_timx_info_st *tim_info, uint16_t ccr_val)
{
    if (tim_info == RT_NULL) {
        dbg_printf("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
        return PWM_FUNC_RET_ERR;
    }

    if (tim_info->tim_channel & HL_HAL_PWM_TIM_CHANNEL_1) {
        TIM_SetCmp1(tim_info->timx, ccr_val);
    }

    if (tim_info->tim_channel & HL_HAL_PWM_TIM_CHANNEL_2) {
        TIM_SetCmp2(tim_info->timx, ccr_val);
    }

    if (tim_info->tim_channel & HL_HAL_PWM_TIM_CHANNEL_3) {
        TIM_SetCmp3(tim_info->timx, ccr_val);
    }

    if (tim_info->tim_channel & HL_HAL_PWM_TIM_CHANNEL_4) {
        TIM_SetCmp4(tim_info->timx, ccr_val);
    }

    return PWM_FUNC_RET_OK;
}

uint8_t hl_hal_pwm_enbale(hl_hal_pwm_timx_info_st *tim_info, uint8_t enable_state)
{
    if (tim_info == RT_NULL) {
        dbg_printf("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
        return PWM_FUNC_RET_ERR;
    }

    if (enable_state == PWM_ENABLE) {
        TIM_ConfigArPreload(tim_info->timx, ENABLE);
        TIM_Enable(tim_info->timx, ENABLE);
        
    } else {
        TIM_ConfigArPreload(tim_info->timx, DISABLE);
        TIM_Enable(tim_info->timx, DISABLE);
    }
    return PWM_FUNC_RET_OK;
}
/*
 * EOF
 */