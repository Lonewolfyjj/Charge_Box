/**
 * @file hl_app_task.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-07
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
 * <tr><td>2022-11-07     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_task.h"
#include "hl_app_mng.h"

#include "hl_app_task_pm.h"
#include "hl_app_task_ui.h"

#include "hl_mod_extcom.h"
#include "hl_mod_pm.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_app_task_common_st _task_comm_st = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/

static void _extcom_mod_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_EXTCOM_MSG_START: {
            DBG_LOG("extcom mod thread start!\n");
            _task_comm_st.extcom_start_flag = true;
        } break;
        
        default:
            break;
    }
}

static void _pm_mod_bat_state_set()
{
    if (_task_comm_st.bat_soc == 100) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_FULL;
    } else if (_task_comm_st.bat_soc < 100 && _task_comm_st.bat_soc >= 75) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_75_100_PERCENT;
    } else if (_task_comm_st.bat_soc < 75 && _task_comm_st.bat_soc >= 50) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_50_75_PERCENT;
    } else if (_task_comm_st.bat_soc < 50 && _task_comm_st.bat_soc >= 25) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_25_50_PERCENT;
    } else if (_task_comm_st.bat_soc < 25 && _task_comm_st.bat_soc > 5) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_5_25_PERCENT;
    } else if (_task_comm_st.bat_soc <= 5 && _task_comm_st.bat_soc >= 0) {
        _task_comm_st.bat_state = HL_APP_BAT_STATE_LOWPOWER;
    }
}

static void _pm_mod_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_PM_MSG_START: {
            DBG_LOG("pm mod thread start!\n");
            _task_comm_st.pm_start_flag = true;
        } break;
        case HL_MOD_PM_SOC_MSG: {
            hl_mod_pm_ctrl(HL_MOD_PM_GET_SOC, &(_task_comm_st.bat_soc), sizeof(_task_comm_st.bat_soc));
            _pm_mod_bat_state_set();
        } break;
        case HL_MOD_PM_CHARGE_MSG: {

        } break;
        case HL_MOD_PM_TX1_MSG: {
            
        } break;
        case HL_MOD_PM_TX2_MSG: {
            
        } break;
        case HL_MOD_PM_RX_MSG: {
            
        } break;
        case HL_MOD_PM_BOX_MSG: {
            
        } break;
        default:
            break;
    }
}

static void _upgrade_mod_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_EXTCOM_MSG_START: {

        } break;

        default:
            break;
    }
}

static void _task_state_update(void)
{
}

/* Exported functions --------------------------------------------------------*/

hl_app_task_common_st* hl_app_task_get(void)
{
    return &_task_comm_st;
}

void hl_app_task_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->msg_id) {
        case HL_APP_MSG_ID_EXTCOM_MOD: {
            _extcom_mod_msg_proc(msg);
        } break;
        case HL_APP_MSG_ID_PM_MOD: {
            _pm_mod_msg_proc(msg);
        } break;
        case HL_APP_MSG_ID_UPGRADE_MOD: {
            _upgrade_mod_msg_proc(msg);
        } break;
        default:
            break;
    }

    _task_state_update();
}

void hl_app_task_proc(void)
{
    hl_app_task_pm_proc();
    hl_app_task_ui_proc();
}

/*
 * EOF
 */