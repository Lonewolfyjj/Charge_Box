/**
 * @file hl_app_task.h
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

#ifndef __HL_APP_TASK_H__
#define __HL_APP_TASK_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

#include "hl_app_msg.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_app_bat_state_e
{
    HL_APP_BAT_STATE_UNKNOWN = 0,
    HL_APP_BAT_STATE_LOWPOWER,
    HL_APP_BAT_STATE_5_25_PERCENT,
    HL_APP_BAT_STATE_25_50_PERCENT,
    HL_APP_BAT_STATE_50_75_PERCENT,
    HL_APP_BAT_STATE_75_100_PERCENT,
    HL_APP_BAT_STATE_FULL,
    HL_APP_BAT_STATE_ERR,
} hl_app_bat_state_e;

typedef enum _hl_app_bat_charge_state_e
{
    HL_APP_BAT_CHARGE_STATE_UNKNOWN = 0,
    HL_APP_BAT_CHARGE_STATE_NO_CHARGE,
    HL_APP_BAT_CHARGE_STATE_CHARGING,
    HL_APP_BAT_CHARGE_STATE_CHARGE_COMPLETE,
} hl_app_bat_charge_state_e;

typedef enum _hl_app_vbus_state_e
{
    HL_APP_VBUS_STATE_UNKNOWN = 0,
    HL_APP_VBUS_STATE_IN,
    HL_APP_VBUS_STATE_OUT,
} hl_app_vbus_state_e;

typedef enum _hl_app_hall_state_e
{
    HL_APP_HALL_STATE_UNKNOWN = 0,
    HL_APP_HALL_STATE_IN,
    HL_APP_HALL_STATE_OUT,
} hl_app_hall_state_e;

typedef enum _hl_app_drv_state_e
{
    HL_APP_DRV_STATE_NO_ERR      = 0x00000000,
    HL_APP_DRV_STATE_GUAGE_ERR   = 0x00000001,
    HL_APP_DRV_STATE_CHARGER_ERR = 0x00000002,
    HL_APP_DRV_STATE_FLASH_ERR   = 0x00000004,
    HL_APP_DRV_STATE_RTC_ERR     = 0x00000008,
} hl_app_drv_state_e;

typedef struct _hl_app_task_common_st
{
    hl_app_drv_state_e drv_state;

    /// PM
    bool                      pm_start_flag;
    uint8_t                   bat_soc;
    hl_app_bat_state_e        bat_state;
    hl_app_bat_charge_state_e charge_state;
    hl_app_vbus_state_e       vbus_state;
    hl_app_hall_state_e       tx1_hall_state;
    hl_app_hall_state_e       tx2_hall_state;
    hl_app_hall_state_e       rx_hall_state;
    hl_app_hall_state_e       box_hall_state;
    bool                      pm_timeout_flag;

    /// extcom
    bool                      extcom_start_flag;
    bool                      rx_online_flag;
    bool                      tx1_online_flag;
    bool                      tx2_online_flag;
    uint8_t                   tx1_bat_soc;
    uint8_t                   tx2_bat_soc;
    uint8_t                   rx_bat_soc;
    hl_app_bat_charge_state_e tx1_charge_state;
    hl_app_bat_charge_state_e tx2_charge_state;
    hl_app_bat_charge_state_e rx_charge_state;
} hl_app_task_common_st;

/* define --------------------------------------------------------------------*/

#define HL_APP_TASK_FUNC_ERR 1
#define HL_APP_TASK_FUNC_OK 0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

hl_app_task_common_st* hl_app_task_get(void);

void hl_app_task_init(void);

void hl_app_task_msg_proc(hl_app_msg_st* msg);

void hl_app_task_proc(void);

#endif
/*
 * EOF
 */