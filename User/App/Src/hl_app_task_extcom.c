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

#define DBG_SECTION_NAME "app_extcom"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_task_extcom_st
{
    hl_app_task_common_st* task_comm;
} hl_app_task_extcom_st;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

static hl_app_task_extcom_st _extcom_task = {
    .task_comm = RT_NULL,
};

/* Private function(only *.c)  -----------------------------------------------*/

static void _extcom_state_reset_poll(void)
{
    if (_extcom_task.task_comm->tx1_hall_state == HL_APP_HALL_STATE_OUT) {
        _extcom_task.task_comm->tx1_online_flag  = false;
        _extcom_task.task_comm->tx1_bat_soc      = 0xFF;
        _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    }

    if (_extcom_task.task_comm->tx2_hall_state == HL_APP_HALL_STATE_OUT) {
        _extcom_task.task_comm->tx2_online_flag  = false;
        _extcom_task.task_comm->tx2_bat_soc      = 0xFF;
        _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    }

    if (_extcom_task.task_comm->rx_hall_state == HL_APP_HALL_STATE_OUT) {
        _extcom_task.task_comm->rx_online_flag  = false;
        _extcom_task.task_comm->rx_bat_soc      = 0xFF;
        _extcom_task.task_comm->rx_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    }
}

static void _extcom_dev_online_probe_set(void)
{
    bool flag;

    if (_extcom_task.task_comm->tx1_hall_state == HL_APP_HALL_STATE_IN) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX1_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX1_PROBE, &flag, sizeof(flag));
    }

    if (_extcom_task.task_comm->tx2_hall_state == HL_APP_HALL_STATE_IN) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX2_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_TX2_PROBE, &flag, sizeof(flag));
    }

    if (_extcom_task.task_comm->rx_hall_state == HL_APP_HALL_STATE_IN) {
        flag = true;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_RX_PROBE, &flag, sizeof(flag));
    } else {
        flag = false;
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_START_RX_PROBE, &flag, sizeof(flag));
    }
}

static void _extcom_bat_soc_set_poll(void)
{
    if (_extcom_task.task_comm->bat_state != HL_APP_BAT_STATE_ERR
        && _extcom_task.task_comm->bat_state != HL_APP_BAT_STATE_UNKNOWN) {
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_SET_BOX_BAT_INFO, &(_extcom_task.task_comm->bat_soc), sizeof(uint8_t));
    }

    if (_extcom_task.task_comm->charge_state != HL_APP_BAT_CHARGE_STATE_UNKNOWN) {
        hl_mod_extcom_ctrl(HL_MOD_EXTCOM_SET_BOX_CHARGE_STATE, &(_extcom_task.task_comm->charge_state),
                           sizeof(uint32_t));
    }
}

static void _extcom_box_lid_state_set_poll(void)
{
    hl_mod_extcom_ctrl(HL_MOD_EXTCOM_SET_BOX_LID_STATE, &(_extcom_task.task_comm->box_hall_state), sizeof(uint32_t));
}

static void _update_tx1_bat_state(uint8_t soc)
{
    _extcom_task.task_comm->tx1_bat_soc = soc;
}

static void _update_tx2_bat_state(uint8_t soc)
{
    _extcom_task.task_comm->tx2_bat_soc = soc;
}

static void _update_rx_bat_state(uint8_t soc)
{
    _extcom_task.task_comm->rx_bat_soc = soc;
}

static void _update_tx1_charge_state(uint8_t state)
{
    if (state == 0) {
        _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    } else if (state == 1) {
        _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_NO_CHARGE;
    } else if (state == 2) {
        _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGING;
    } else if (state == 3) {
        _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGE_COMPLETE;
    }
}

static void _update_tx2_charge_state(uint8_t state)
{
    if (state == 0) {
        _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    } else if (state == 1) {
        _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_NO_CHARGE;
    } else if (state == 2) {
        _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGING;
    } else if (state == 3) {
        _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGE_COMPLETE;
    }
}

static void _update_rx_charge_state(uint8_t state)
{
    if (state == 0) {
        _extcom_task.task_comm->rx_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    } else if (state == 1) {
        _extcom_task.task_comm->rx_charge_state = HL_APP_BAT_CHARGE_STATE_NO_CHARGE;
    } else if (state == 2) {
        _extcom_task.task_comm->rx_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGING;
    } else if (state == 3) {
        _extcom_task.task_comm->rx_charge_state = HL_APP_BAT_CHARGE_STATE_CHARGE_COMPLETE;
    }
}

/* Exported functions --------------------------------------------------------*/

void hl_app_task_extcom_init(void)
{
    _extcom_task.task_comm = hl_app_task_get();

    _extcom_task.task_comm->tx1_online_flag  = false;
    _extcom_task.task_comm->tx2_online_flag  = false;
    _extcom_task.task_comm->rx_online_flag   = false;
    _extcom_task.task_comm->tx1_bat_soc      = 0XFF;
    _extcom_task.task_comm->tx2_bat_soc      = 0XFF;
    _extcom_task.task_comm->rx_bat_soc       = 0XFF;
    _extcom_task.task_comm->tx1_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    _extcom_task.task_comm->tx2_charge_state = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
    _extcom_task.task_comm->rx_charge_state  = HL_APP_BAT_CHARGE_STATE_UNKNOWN;
}

void hl_app_task_extcom_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_EXTCOM_MSG_START: {
            LOG_I("extcom mod thread start!");
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
        case HL_MOD_EXTCOM_MSG_TX1_BAT_STATE: {
            _update_tx1_bat_state(*(uint8_t*)msg->param);
        } break;
        case HL_MOD_EXTCOM_MSG_TX2_BAT_STATE: {
            _update_tx2_bat_state(*(uint8_t*)msg->param);
        } break;
        case HL_MOD_EXTCOM_MSG_RX_BAT_STATE: {
            _update_rx_bat_state(*(uint8_t*)msg->param);
        } break;
        case HL_MOD_EXTCOM_MSG_TX1_CHARGE_STATE: {
            _update_tx1_charge_state(*(uint8_t*)msg->param);
        } break;
        case HL_MOD_EXTCOM_MSG_TX2_CHARGE_STATE: {
            _update_tx2_charge_state(*(uint8_t*)msg->param);
        } break;
        case HL_MOD_EXTCOM_MSG_RX_CHARGE_STATE: {
            _update_rx_charge_state(*(uint8_t*)msg->param);
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

    _extcom_state_reset_poll();
    _extcom_dev_online_probe_set();
    _extcom_bat_soc_set_poll();
    _extcom_box_lid_state_set_poll();
}

/*
 * EOF
 */