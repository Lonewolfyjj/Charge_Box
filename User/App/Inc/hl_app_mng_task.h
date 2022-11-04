/**
 * @file hl_app_mng_task.h
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

#ifndef __HL_APP_MNG_TASK_H__
#define __HL_APP_MNG_TASK_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

#include "hl_app_msg.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_app_mng_task_msg_proc(hl_app_msg_st* msg);

void hl_app_mng_task_proc(void);

#endif
/*
 * EOF
 */