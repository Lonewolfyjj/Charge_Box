/**
 * @file hl_app_mng.h
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-03
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
 * <tr><td>2022-11-03     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_APP_MNG_H__
#define __HL_APP_MNG_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

#include "hl_app_msg.h"

#include "hl_mod_extcom.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_mng_st
{
    bool extcom_start_flag;
} hl_app_mng_st;

/* define --------------------------------------------------------------------*/

#define HL_APP_MNG_FUNC_ERR 1
#define HL_APP_MNG_FUNC_OK 0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_app_mng_init(void);

int hl_app_mng_deinit(void);

int hl_app_mng_start(void);

int hl_app_mng_stop(void);

hl_app_mng_st* hl_app_mng_get(void);
      
int hl_app_mng_extcom_proc(hl_app_msg_st* msg);

int hl_app_mng_ui_proc(hl_app_msg_st* msg);

int hl_app_mng_charger_proc(hl_app_msg_st* msg);

int hl_app_mng_upgrade_proc(hl_app_msg_st* msg);

#endif
/*
 * EOF
 */