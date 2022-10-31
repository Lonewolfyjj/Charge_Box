/**
 * @file hl_mod_pm.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 电源管理模块头文件
 * @version V1.0
 * @date 2022-10-24
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
 * <tr><td>2022-10-24     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_MOD_PM_H__
#define __HL_MOD_PM_H__


/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_pm_op_e{
    HL_MOD_PM_ENTER_LOWPOWER
}hl_mod_pm_op_e;
typedef enum _hl_mod_pm_charge_status_e {
    HL_MOD_PM_CHAR_STAT = 0,
    HL_MOD_PM_VBUS_STAT,
    HL_MOD_PM_INPUT_STAT,
    HL_MOD_PM_VINDPM_STAT,
    HL_MOD_PM_IINDPM_STAT,
    HL_MOD_PM_SYS_VOL_STAT,
    HL_MOD_PM_BAT_ERR_STAT,
    HL_MOD_PM_CHAR_ERR_STAT,
    HL_MOD_PM_BOOST_ERR_STAT,
    HL_MOD_PM_WATCHDOG_ERR_STAT
}hl_mod_pm_charge_status_e;

/* define --------------------------------------------------------------------*/

#define HL_MOD_PM_FUNC_RET_OK 0
#define HL_MOD_PM_FUNC_RET_ERR 1

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_pm_init(void* msg_hd);

int hl_mod_pm_deinit(void);

int hl_mod_pm_start(void);

int hl_mod_pm_stop(void);

int hl_mod_pm_ctrl(int op, void* arg, int arg_size);

#endif
/*
 * EOF
 */