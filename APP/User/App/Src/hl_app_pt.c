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

#include "hl_app_task.h"
#include "n32l40x.h"
#include "hl_util_config.h"
#include "hl_mod_extcom.h"
#include "hl_drv_cw2215.h"

#define DBG_SECTION_NAME "app_pt"
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define BOX_APP_VERSION "v1.0.0.0"

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

static void hl_app_pt_show_version(void)
{
    LOG_I(BOX_APP_VERSION);
}

static void hl_app_pt_guage_info(void)
{
    int                    ret;
    hl_st_drv_guage_soc_t  soc;
    uint16_t               voltage;
    int32_t                current;
    hl_st_drv_guage_temp_t temp;
    uint8_t                soh;

    ret = hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_SOC, &(soc), sizeof(soc));
    if (ret != CW2215_FUNC_RET_ERR) {
        LOG_I("soc:%d.%d", soc.soc, soc.soc_d);
    }

    ret = hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_VOLTAGE, &(voltage), sizeof(voltage));
    if (ret != CW2215_FUNC_RET_ERR) {
        LOG_I("voltage:%d", voltage);
    }

    ret = hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_CURRENT, &(current), sizeof(current));
    if (ret != CW2215_FUNC_RET_ERR) {
        LOG_I("current:%d", current);
    }

    ret = hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_TEMP, &(temp), sizeof(temp));
    if (ret != CW2215_FUNC_RET_ERR) {
        LOG_I("temp:%d.%d", temp.temp, temp.temp_d);
    }

    ret = hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_SOH, &(soh), sizeof(soh));
    if (ret != CW2215_FUNC_RET_ERR) {
        LOG_I("soh:%d", soh);
    }
}

static void hl_app_pt_dev_state_info(void)
{
    hl_app_task_common_st* task_comm;

    task_comm = hl_app_task_get();

    LOG_I("tx1 tx2 rx");
    LOG_I("in box:%d %d %d", task_comm->tx1_online_flag, task_comm->tx2_online_flag, task_comm->rx_online_flag);
    LOG_I("soc:%d %d %d", task_comm->tx1_bat_soc, task_comm->tx2_bat_soc, task_comm->rx_bat_soc);
    LOG_I("charge:%d %d %d", task_comm->tx1_charge_state, task_comm->tx2_charge_state, task_comm->rx_charge_state);
}

MSH_CMD_EXPORT(hl_app_pt_enter_boot, 进入boot模式);
MSH_CMD_EXPORT(hl_app_pt_enter_lowpower, 进入低功耗模式);
MSH_CMD_EXPORT(hl_app_pt_enter_upgrade, 进入升级模式);
MSH_CMD_EXPORT(hl_app_pt_show_version, 打印固件版本信息);
MSH_CMD_EXPORT(hl_app_pt_guage_info, 打印电量计信息);
MSH_CMD_EXPORT(hl_app_pt_dev_state_info, 打印设备状态信息);
/*
 * EOF
 */