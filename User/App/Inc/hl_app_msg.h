/**
 * @file hl_app_msg.h
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-01
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
 * <tr><td>2022-11-01     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_APP_MSG_H__
#define __HL_APP_MSG_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_app_msg_id_e
{
    HL_APP_MSG_ID_PT_APP,
    HL_APP_MSG_ID_PM_MOD,
    HL_APP_MSG_ID_UI_MOD,
    HL_APP_MSG_ID_EXTCOM_MOD,
    HL_APP_MSG_ID_UPGRADE_MOD,
} hl_app_msg_id_e;

typedef struct _hl_app_msg_st
{
    hl_app_msg_id_e msg_id;
    uint8_t         cmd;
    uint16_t        len;
    void*           param;
} hl_app_msg_st;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

#define HL_APP_MSG_FUNC_ERR 1
#define HL_APP_MSG_FUNC_OK 0

/* Exported functions --------------------------------------------------------*/

int hl_app_msg_init(void);

int hl_app_msg_deinit(void);

int hl_app_msg_send(hl_app_msg_id_e msg_id, uint8_t cmd, void* param, uint16_t len, int timeout);

int hl_app_msg_recv(hl_app_msg_st* msg, int timeout);

#endif
/*
 * EOF
 */