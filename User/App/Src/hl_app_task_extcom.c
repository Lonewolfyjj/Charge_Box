/**
 * @file hl_app_task_extcom.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-09
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
 * <tr><td>2022-11-09     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_task_extcom.h"
#include "hl_app_task.h"

#include "hl_mod_extcom.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_task_extcom_st
{
    hl_app_task_common_st* task_comm;
} hl_app_task_extcom_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_app_task_extcom_st _extcom_task = {
    .task_comm = RT_NULL,
};

/* Private function(only *.c)  -----------------------------------------------*/

static void _extcom_dev_online_probe_set(void)
{
    bool flag;

    if (_extcom_task.task_comm->tx1_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->tx1_online_flag == false) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX1_PROBE, &flag, sizeof(flag));
    } else if (_extcom_task.task_comm->tx1_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->tx1_online_flag == true) {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX1_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX1_PROBE, &flag, sizeof(flag));
        _extcom_task.task_comm->tx1_online_flag = false;
    }

    if (_extcom_task.task_comm->tx2_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->tx2_online_flag == false) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX2_PROBE, &flag, sizeof(flag));
    } else if (_extcom_task.task_comm->tx2_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->tx2_online_flag == true) {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX2_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX2_PROBE, &flag, sizeof(flag));
        _extcom_task.task_comm->tx2_online_flag = false;
    }

    if (_extcom_task.task_comm->rx_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->rx_online_flag == false) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_RX_PROBE, &flag, sizeof(flag));
    } else if (_extcom_task.task_comm->rx_hall_state == HL_APP_HALL_STATE_IN && _extcom_task.task_comm->rx_online_flag == true) {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_RX_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_RX_PROBE, &flag, sizeof(flag));
        _extcom_task.task_comm->rx_online_flag = false;
    }
}

/* Exported functions --------------------------------------------------------*/

void hl_app_task_extcom_init(void)
{
    _extcom_task.task_comm = hl_app_task_get();

    _extcom_task.task_comm->tx1_online_flag = false;
    _extcom_task.task_comm->tx2_online_flag = false;
    _extcom_task.task_comm->rx_online_flag = false;
}

void hl_app_task_extcom_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_EXTCOM_MSG_START: {
            DBG_LOG("extcom mod thread start!\n");
            _extcom_task.task_comm->extcom_start_flag = true;
        } break;
        case HL_MOD_EXTCOM_MSG_RX_ONLINE_STATE: {
            _extcom_task.task_comm->rx_online_flag = *(bool*)msg->param;
        } break;
        case HL_MOD_EXTCOM_MSG_TX1_ONLINE_STATE: {
            _extcom_task.task_comm->tx1_online_flag = *(bool*)msg->param;
        } break;
        case HL_MOD_EXTCOM_MSG_TX2_ONLINE_STATE: {
            _extcom_task.task_comm->tx2_online_flag = *(bool*)msg->param;
        } break;
        default:
            break;
    }
}

void hl_app_task_extcom_proc(void)
{
    if (_extcom_task.task_comm->extcom_start_flag == false) {
        return;
    }

    _extcom_dev_online_probe_set();
}

/*
 * EOF
 */