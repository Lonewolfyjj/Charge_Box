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
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

static void hl_app_pt_enter_boot(void)
{
    int                 ret;
    hl_util_config_st_p config;

    ret = hl_util_config_get(&config);
    if (ret == HL_UTIL_CONFIG_FUNC_ERR) {
        LOG_E("config get err!");
        return;
    }

    config->boot_jump_flag = 1;
    hl_util_config_save();

    __NVIC_SystemReset();  //重启
}

static void hl_app_pt_enter_lowpower(void)
{
    int                 ret;
    hl_util_config_st_p config;

    ret = hl_util_config_get(&config);
    if (ret == HL_UTIL_CONFIG_FUNC_ERR) {
        LOG_E("config get err!");
        return;
    }

    config->lowpower_flag = 1;
    hl_util_config_save();

    __NVIC_SystemReset();  //重启
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