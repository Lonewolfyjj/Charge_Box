/**
 * @file hl_app_pt.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 产测应用源文件
 * @version 1.0
 * @date 2022-10-28
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
 * <tr><td>2022-10-28     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

#include "n32l40x.h"
#include "hl_util_config.h"
#include "hl_mod_extcom.h"

#define DBG_SECTION_NAME "app_pt"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

extern bool hl_iwdog_feed_flag;

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

static void hl_app_pt_enter_boot(void)
{
    // hl_note: 使用窗口看门狗进行系统重启来进boot dfu ，喂狗时只喂一个数字1，这样狗很快就饿死了，然后看门狗复位，开机进boot dfu
    RCC_ClocksType temp;
    // DBG_ConfigPeriph(DBG_WWDG_STOP,ENABLE);

    /* WWDG configuration */
    /* Enable WWDG clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_WWDG, ENABLE);
    RCC_GetClocksFreqValue(&temp);
    /* WWDG clock counter = (PCLK1(16MHz)/4096)/8 = 488 Hz (~2049 us)  */
    WWDG_SetPrescalerDiv(WWDG_PRESCALER_DIV8);

    /* Set Window value to 80; WWDG counter should be refreshed only when the counter
    is below 80 (and greater than 64) otherwise a reset will be generated */
    WWDG_SetWValue(80);
    /*
    Enable WWDG and set counter value to 127, WWDG timeout = ~2049 us * (127-63) = 131.136 ms
    In this case the refresh window is: ~2049 us * (127-80) = 96.3 ms < refresh window < ~2049us * 64 = 131.136 ms
  */
    WWDG_Enable(127);

    return;
}

static void hl_app_pt_enter_lowpower(void)
{
    hl_iwdog_feed_flag = false;
}

static void hl_app_pt_enter_upgrade(void)
{
    bool flag = true;

    hl_mod_extcom_ctrl(HL_MOD_EXTCOM_ENTER_UPGRADE, &flag, sizeof(flag));
}

MSH_CMD_EXPORT(hl_app_pt_enter_boot, 进入boot模式);
MSH_CMD_EXPORT(hl_app_pt_enter_lowpower, 进入低功耗模式);
MSH_CMD_EXPORT(hl_app_pt_enter_upgrade, 进入升级模式);
/*
 * EOF
 */