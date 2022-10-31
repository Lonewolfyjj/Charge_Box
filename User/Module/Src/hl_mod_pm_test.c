/**
 * @file hl_mod_pm_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-24
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
 * <tr><td>2022-10-24     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_pm.h"
#include "stdlib.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


void hl_mod_pm_test(int argc, char *argv[])
{
    if (argc <= 1) {
        rt_kprintf("format : [cmd 1/2] 1:deinit 2:init_start 3:pm_lowpower\n");
        return;
    }

    if (atoi(argv[1]) == 1) {
        hl_mod_pm_deinit();
    } else if (atoi(argv[1]) == 2) {
        hl_mod_pm_init(RT_NULL);
        hl_mod_pm_start();
    } else if (atoi(argv[1]) == 3) {
        /* 低功耗模式 */
        hl_mod_pm_ctrl(HL_MOD_PM_ENTER_LOWPOWER, RT_NULL, 1);
    }
     
}

MSH_CMD_EXPORT(hl_mod_pm_test, pm mod start test);
/*
 * EOF
 */