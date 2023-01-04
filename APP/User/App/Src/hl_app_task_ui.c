/**
 * @file hl_app_task_ui.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-08
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
 * <tr><td>2022-11-08     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_task_ui.h"
#include "hl_app_task.h"

#include "hl_mod_ui.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

static hl_mod_ui_charge_state_e charge_state_convert(hl_app_bat_charge_state_e state)
{
    if (state == HL_APP_BAT_CHARGE_STATE_NO_CHARGE) {
        return HL_MOD_UI_NO_CHARGING;
    } else if (state == HL_APP_BAT_CHARGE_STATE_CHARGING) {
        return HL_MOD_UI_CHARGING;
    } else if (state == HL_APP_BAT_CHARGE_STATE_CHARGE_COMPLETE) {
        return HL_MOD_UI_CHARG_FULL;
    } else if (state == HL_APP_BAT_CHARGE_STATE_UNKNOWN) {
        return HL_MOD_UI_CHARG_UNKNOW;
    }

    return HL_MOD_UI_NO_CHARGING;
}

static hl_mod_load_online_state_e hall_state_convert(hl_app_hall_state_e state)
{
    if (state == HL_APP_HALL_STATE_UNKNOWN) {
        return HL_MOD_UI_ONLINE_STATE_UNKNOW;
    } else if (state == HL_APP_HALL_STATE_IN) {
        return HL_MOD_UI_ONLINE;
    } else if (state == HL_APP_HALL_STATE_OUT) {
        return HL_MOD_UI_NO_ONLINE;
    }

    return HL_MOD_UI_NO_ONLINE;
}

/* Exported functions --------------------------------------------------------*/

void hl_app_task_ui_init(void)
{
}

void hl_app_task_ui_msg_proc(hl_app_msg_st* msg)
{
}

void hl_app_task_ui_proc(void)
{
    hl_app_task_common_st*     task_comm;
    hl_mod_ui_charge_state_e   charge_state;
    hl_mod_ui_fault_state_e    fault_state;
    hl_mod_ui_upgrade_state_e  upgrade_state;
    hl_mod_load_online_state_e hall_state;

    task_comm = hl_app_task_get();

    if (task_comm->pm_start_flag == false || task_comm->extcom_start_flag == false) {
        return;
    }

    hall_state = hall_state_convert(task_comm->tx1_hall_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_TX1_ONLINE_STATE, &(hall_state), sizeof(hall_state));

    hall_state = hall_state_convert(task_comm->tx2_hall_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_TX2_ONLINE_STATE, &(hall_state), sizeof(hall_state));

    hall_state = hall_state_convert(task_comm->rx_hall_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_RX_ONLINE_STATE, &(hall_state), sizeof(hall_state));

    charge_state = charge_state_convert(task_comm->charge_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_BOX_CHARGE_STATE, &(charge_state), sizeof(charge_state));

    charge_state = charge_state_convert(task_comm->tx1_charge_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_TX1_CHARGE_STATE, &(charge_state), sizeof(charge_state));

    charge_state = charge_state_convert(task_comm->tx2_charge_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_TX2_CHARGE_STATE, &(charge_state), sizeof(charge_state));

    charge_state = charge_state_convert(task_comm->rx_charge_state);

    hl_mod_ui_ctrl(HL_MOD_UI_SET_RX_CHARGE_STATE, &(charge_state), sizeof(charge_state));

    hl_mod_ui_ctrl(HL_MOD_UI_SET_BAT_SOC_VAL, &(task_comm->bat_soc), sizeof(task_comm->bat_soc));

    if (task_comm->drv_state == HL_APP_DRV_STATE_NO_ERR) {
        fault_state = HL_MOD_UI_NO_FAULT;
    } else {
        fault_state = HL_MOD_UI_FAULT;
    }

    hl_mod_ui_ctrl(HL_MOD_UI_SET_FAULT_STATE, &(fault_state), sizeof(fault_state));

    upgrade_state = HL_MOD_UI_NO_UPGRADE;

    hl_mod_ui_ctrl(HL_MOD_UI_SET_UPGRADE_STATE, &(upgrade_state), sizeof(upgrade_state));

    if (task_comm->pm_timeout_flag == true) {
        hl_mod_ui_ctrl(HL_MOD_UI_SET_TIMEOUT_FLAG, RT_NULL, 0);
    } else {
        hl_mod_ui_ctrl(HL_MOD_UI_CLEAR_TIMEOUT_FLAG, RT_NULL, 0);
    }
}

/*
 * EOF
 */