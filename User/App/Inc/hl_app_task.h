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
    HL_APP_BAT_STATE_LOWPOWER,
    HL_APP_BAT_STATE_5_25_PERCENT,
    HL_APP_BAT_STATE_25_50_PERCENT,
    HL_APP_BAT_STATE_50_75_PERCENT,
    HL_APP_BAT_STATE_75_100_PERCENT,
    HL_APP_BAT_STATE_FULL,
} hl_app_bat_state_e;

typedef struct _hl_app_task_common_st
{
    bool               extcom_start_flag;
    bool               pm_start_flag;
    uint8_t            bat_soc;
    hl_app_bat_state_e bat_state;
    uint8_t            charge_state;
} hl_app_task_common_st;

/* define --------------------------------------------------------------------*/

#define HL_APP_TASK_FUNC_ERR 1
#define HL_APP_TASK_FUNC_OK 0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

hl_app_task_common_st* hl_app_task_get(void);

void hl_app_task_msg_proc(hl_app_msg_st* msg);

void hl_app_task_proc(void);

#endif
/*
 * EOF
 */