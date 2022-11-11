/**
 * @file hl_app_task_pm.c
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

#include "hl_app_task_pm.h"
#include "hl_app_task.h"

#include "hl_mod_pm.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_task_pm_st
{
    hl_app_task_common_st* task_comm;
    struct rt_timer        timer;
} hl_app_task_pm_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_app_task_pm_st _pm_task = {
    .task_comm    = RT_NULL,
    .timer        = { 0 },
};

/* Private function(only *.c)  -----------------------------------------------*/

static void _pm_mod_bat_state_set()
{
    hl_mod_pm_ctrl(HL_MOD_PM_GET_SOC, &(_pm_task.task_comm->bat_soc), sizeof(_pm_task.task_comm->bat_soc));

    if (_pm_task.task_comm->bat_soc == 100) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_FULL;
    } else if (_pm_task.task_comm->bat_soc < 100 && _pm_task.task_comm->bat_soc >= 75) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_75_100_PERCENT;
    } else if (_pm_task.task_comm->bat_soc < 75 && _pm_task.task_comm->bat_soc >= 50) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_50_75_PERCENT;
    } else if (_pm_task.task_comm->bat_soc < 50 && _pm_task.task_comm->bat_soc >= 25) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_25_50_PERCENT;
    } else if (_pm_task.task_comm->bat_soc < 25 && _pm_task.task_comm->bat_soc > 5) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_5_25_PERCENT;
    } else if (_pm_task.task_comm->bat_soc <= 5 && _pm_task.task_comm->bat_soc >= 0) {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_LOWPOWER;
    } else {
        _pm_task.task_comm->bat_state = HL_APP_BAT_STATE_ERR;
    }
}

static void _pm_mod_charge_state_set(void)
{
    uint8_t charge_value;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_CHARGE_STATE, &(charge_value), sizeof(charge_value));

    if (charge_value == 0) {
        _pm_task.task_comm->charge_state = HL_APP_BAT_CHARGE_STATE_NO_CHARGE;
        DBG_LOG("pm no charge!\n");
    } else if (charge_value == 3) {
        DBG_LOG("pm charge full!\n");
        _pm_task.task_comm->charge_state = HL_APP_BAT_CHARGE_STATE_CHARGE_FULL;
    } else {
        DBG_LOG("pm is charging!\n");
        _pm_task.task_comm->charge_state = HL_APP_BAT_CHARGE_STATE_CHARGING;
    }
}

static void _pm_mod_tx1_hall_state_set(void)
{
    uint8_t hall_state;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_TX1_STATE, &(hall_state), sizeof(hall_state));

    if (hall_state == 0) {
        _pm_task.task_comm->tx1_hall_state = HL_APP_HALL_STATE_IN;
    } else {
        _pm_task.task_comm->tx1_hall_state = HL_APP_HALL_STATE_OUT;
    }
}

static void _pm_mod_tx2_hall_state_set(void)
{
    uint8_t hall_state;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_TX2_STATE, &(hall_state), sizeof(hall_state));

    if (hall_state == 0) {
        _pm_task.task_comm->tx2_hall_state = HL_APP_HALL_STATE_IN;
    } else {
        _pm_task.task_comm->tx2_hall_state = HL_APP_HALL_STATE_OUT;
    }
}

static void _pm_mod_rx_hall_state_set(void)
{
    uint8_t hall_state;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_RX_STATE, &(hall_state), sizeof(hall_state));

    if (hall_state == 0) {
        _pm_task.task_comm->rx_hall_state = HL_APP_HALL_STATE_IN;
    } else {
        _pm_task.task_comm->rx_hall_state = HL_APP_HALL_STATE_OUT;
    }
}

static void _pm_mod_box_hall_state_set(void)
{
    uint8_t hall_state;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_BOX_STATE, &(hall_state), sizeof(hall_state));

    if (hall_state == 0) {
        _pm_task.task_comm->box_hall_state = HL_APP_HALL_STATE_IN;
    } else {
        _pm_task.task_comm->box_hall_state = HL_APP_HALL_STATE_OUT;
    }
}

static void _pm_mod_vbus_state_set(void)
{
    uint8_t state;

    hl_mod_pm_ctrl(HL_MOD_PM_GET_VBUS_STATE, &(state), sizeof(state));

    if (state == 0) {
        _pm_task.task_comm->charge_state = HL_APP_BAT_CHARGE_STATE_NO_CHARGE;
    } else {
        _pm_task.task_comm->charge_state = HL_APP_BAT_CHARGE_STATE_CHARGING;
    }
}

static void _timer_timeout_handle(void* arg)
{
    DBG_LOG("pm task timeout\n");
    _pm_task.task_comm->pm_timeout_flag = true;
}

/* Exported functions --------------------------------------------------------*/

void hl_app_task_pm_init(void)
{
    _pm_task.task_comm = hl_app_task_get();

    rt_timer_init(&(_pm_task.timer), "task_pm_timer", _timer_timeout_handle, RT_NULL, 1000 * 60 * 2,
                  RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

    _pm_task.task_comm->pm_timeout_flag = false;

    rt_timer_start(&(_pm_task.timer));
}

void hl_app_task_pm_msg_proc(hl_app_msg_st* msg)
{
    switch (msg->cmd) {
        case HL_MOD_PM_MSG_START: {
            DBG_LOG("pm mod thread start!\n");
            _pm_task.task_comm->pm_start_flag = true;
        } break;
        case HL_MOD_PM_SOC_MSG: {
            _pm_mod_bat_state_set();
        } break;
        case HL_MOD_PM_CHARGE_MSG: {
            _pm_mod_charge_state_set();
        } break;
        case HL_MOD_PM_TX1_MSG: {
            _pm_mod_tx1_hall_state_set();
        } break;
        case HL_MOD_PM_TX2_MSG: {
            _pm_mod_tx2_hall_state_set();
        } break;
        case HL_MOD_PM_RX_MSG: {
            _pm_mod_rx_hall_state_set();
        } break;
        case HL_MOD_PM_BOX_MSG: {
            _pm_mod_box_hall_state_set();
        } break;
        case HL_MOD_PM_GUAGE_ERR_MSG: {
            _pm_task.task_comm->drv_state |= HL_APP_DRV_STATE_GUAGE_ERR;
        } break;
        case HL_MOD_PM_CHARGE_ERR_MSG: {
            _pm_task.task_comm->drv_state |= HL_APP_DRV_STATE_CHARGER_ERR;
        } break;
        case HL_MOD_PM_VBUS_MSG: {
            _pm_mod_vbus_state_set();
        } break;
        default:
            break;
    }
}

void hl_app_task_pm_proc(void)
{
    switch (_pm_task.task_comm->bat_state) {
        case HL_APP_BAT_STATE_LOWPOWER: {
            
        } break;
        case HL_APP_BAT_STATE_5_25_PERCENT:
        case HL_APP_BAT_STATE_25_50_PERCENT:
        case HL_APP_BAT_STATE_50_75_PERCENT:
        case HL_APP_BAT_STATE_75_100_PERCENT: {

        } break;
        case HL_APP_BAT_STATE_FULL: {

        } break;
        default:
            break;
    }
}

/*
 * EOF
 */