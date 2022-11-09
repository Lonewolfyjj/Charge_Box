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

#include "hl_app_task_pm.h"
#include "hl_app_task_ui.h"
#include "hl_app_task_extcom.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_app_task_common_st _task_comm_st = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/

static void _task_state_update(void)
{
}

/* Exported functions --------------------------------------------------------*/

hl_app_task_common_st* hl_app_task_get(void)
{
    return &_task_comm_st;
}

void hl_app_task_init(void)
{
    rt_memset(&_task_comm_st, 0, sizeof(_task_comm_st));

    hl_app_task_pm_init();
    hl_app_task_extcom_init();
    hl_app_task_ui_init();
}

void hl_app_task_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->msg_id) {
        case HL_APP_MSG_ID_EXTCOM_MOD: {
            hl_app_task_extcom_msg_proc(msg);
        } break;
        case HL_APP_MSG_ID_PM_MOD: {
            hl_app_task_pm_msg_proc(msg);
        } break;
        case HL_APP_MSG_ID_UI_MOD: {
            hl_app_task_ui_msg_proc(msg);
        } break;
        default:
            break;
    }

    _task_state_update();
}

void hl_app_task_proc(void)
{
    hl_app_task_pm_proc();
    hl_app_task_extcom_proc();
    hl_app_task_ui_proc();
}

/*
 * EOF
 */