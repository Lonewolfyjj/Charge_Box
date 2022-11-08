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

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_app_task_pm_e
{
    HL_APP_TASK_PM_INIT,
} hl_app_task_pm_e;

typedef struct _hl_app_task_pm_st
{
    hl_app_task_pm_e statu;
} hl_app_task_pm_st;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

static hl_app_task_pm_st _pm_task = {
    .statu = HL_APP_TASK_PM_INIT,
};

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_app_task_pm_proc(void)
{
    switch (_pm_task.statu) {
        case HL_APP_TASK_PM_INIT: {
            
        } break;
        
        default:
            break;
    }
}

/*
 * EOF
 */