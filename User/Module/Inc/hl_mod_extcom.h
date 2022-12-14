/**
 * @file hl_mod_extcom.h
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-29
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
 * <tr><td>2022-10-29     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_MOD_EXTCOM_H__
#define __HL_MOD_EXTCOM_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_extcom_op_e
{
    /// 进入升级模式，使用 usb_cdc 进行通信，无法使用 finsh 终端，参数类型为<bool*>。
    HL_MOD_EXTCOM_ENTER_UPGRADE,
    HL_MOD_EXTCOM_START_TX1_PROBE,
    HL_MOD_EXTCOM_START_TX2_PROBE,
    HL_MOD_EXTCOM_START_RX_PROBE,
    HL_MOD_EXTCOM_SET_BOX_BAT_INFO,
    HL_MOD_EXTCOM_SET_BOX_CHARGE_STATE,
    HL_MOD_EXTCOM_SET_BOX_LID_STATE,
} hl_mod_extcom_op_e;

typedef enum _hl_mod_extcom_object_e
{
    HL_MOD_EXTCOM_OBJECT_RX = 0,
    HL_MOD_EXTCOM_OBJECT_TX1,
    HL_MOD_EXTCOM_OBJECT_TX2,
    HL_MOD_EXTCOM_OBJECT_BOX,
    NUM_OF_HL_MOD_EXTCOM_OBJECT,
} hl_mod_extcom_object_e;

typedef enum _hl_mod_extcom_msg_e
{
    HL_MOD_EXTCOM_MSG_START,
    HL_MOD_EXTCOM_MSG_TX1_ONLINE_STATE,
    HL_MOD_EXTCOM_MSG_TX2_ONLINE_STATE,
    HL_MOD_EXTCOM_MSG_RX_ONLINE_STATE,
    HL_MOD_EXTCOM_MSG_TX1_BAT_STATE,
    HL_MOD_EXTCOM_MSG_TX2_BAT_STATE,
    HL_MOD_EXTCOM_MSG_RX_BAT_STATE,
    HL_MOD_EXTCOM_MSG_TX1_CHARGE_STATE,
    HL_MOD_EXTCOM_MSG_TX2_CHARGE_STATE,
    HL_MOD_EXTCOM_MSG_RX_CHARGE_STATE,    
} hl_mod_extcom_msg_e;

/* define --------------------------------------------------------------------*/

#define HL_MOD_EXTCOM_FUNC_ERR 1
#define HL_MOD_EXTCOM_FUNC_OK 0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_extcom_init(void* msg_hd);

int hl_mod_extcom_deinit(void);

int hl_mod_extcom_start(void);

int hl_mod_extcom_stop(void);

int hl_mod_extcom_ctrl(hl_mod_extcom_op_e op, void* arg, int arg_size);

#endif
/*
 * EOF
 */