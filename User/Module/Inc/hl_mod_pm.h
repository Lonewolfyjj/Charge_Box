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

/**
 * @brief hl_mod_pm_ctrl接口的操作指令类型
 * @date 2022-11-08
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-08      <td>yijiujun     <td>新建
 * </table>
 */
typedef enum _hl_mod_pm_op_e {
    HL_MOD_PM_ENTER_LOWPOWER,

    HL_MOD_PM_GET_SOC,

    HL_MOD_PM_GET_CHARGE_STATE,
    HL_MOD_PM_GET_VBUS_STATE,

    HL_MOD_PM_GET_TX1_STATE,
    HL_MOD_PM_GET_TX2_STATE,
    HL_MOD_PM_GET_RX_STATE,
    HL_MOD_PM_GET_BOX_STATE,

    HL_MOD_PM_SET_TX1_CHARGE,
    HL_MOD_PM_SET_TX2_CHARGE,
    HL_MOD_PM_SET_RX_CHARGE
}hl_mod_pm_op_e;

/**
 * @brief 向APP发送的消息类型
 * @date 2022-11-08
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-08      <td>yijiujun     <td>新建
 * </table>
 */
typedef enum _hl_mod_pm_msg_e {
    HL_MOD_PM_MSG_START,
    HL_MOD_PM_GUAGE_ERR_MSG,
    HL_MOD_PM_CHARGE_ERR_MSG,

    HL_MOD_PM_VBUS_MSG,
    HL_MOD_PM_CHARGE_MSG,
    HL_MOD_PM_SOC_MSG,

    HL_MOD_PM_TX1_MSG,
    HL_MOD_PM_TX2_MSG,
    HL_MOD_PM_RX_MSG,
    HL_MOD_PM_BOX_MSG
} hl_mod_pm_msg_e;

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