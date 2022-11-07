/**
 * @file hl_app_mng_task.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-04
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
 * <tr><td>2022-11-04     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_mng_task.h"
#include "hl_app_mng.h"
#include "hl_mod_extcom.h"
#include "hl_mod_pm.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

static void _extcom_mod_msg_proc(hl_app_msg_st* msg)
{
    hl_app_mng_st* mng_st = hl_app_mng_get();

    switch (msg->cmd) {
        case HL_MOD_EXTCOM_MSG_START: {
            DBG_LOG("extcom mod thread start!\n");
            mng_st->extcom_start_flag = true;
        } break;

        default:
            break;
    }
}

static void _pm_mod_msg_proc(hl_app_msg_st* msg)
{
    hl_app_mng_st* mng_st = hl_app_mng_get();
    
    switch (msg->cmd) {
        case HL_MOD_PM_MSG_START: {
            DBG_LOG("pm mod thread start!\n");
            mng_st->pm_start_flag = true;
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

void hl_app_mng_task_msg_proc(hl_app_msg_st* msg)
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

void hl_app_mng_task_proc(void)
{
    
}

/*
 * EOF
 */