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

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

static void hl_app_pt_enter_boot(void)
{
    hl_util_config_st_p config;

    hl_util_config_get(&config);
    config->boot_jump_flag = 1;
    hl_util_config_save();

    __NVIC_SystemReset();                                      //重启
}

static void hl_app_pt_enter_lowpower(void)
{
    hl_util_config_st_p config;

    hl_util_config_get(&config);
    config->lowpower_flag = 1;
    hl_util_config_save();

    __NVIC_SystemReset();                                      //重启
}

MSH_CMD_EXPORT(hl_app_pt_enter_boot, 进入boot模式);
MSH_CMD_EXPORT(hl_app_pt_enter_lowpower, 进入低功耗模式);
/*
 * EOF
 */