/**
 * @file hl_drv_rt4813_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-09
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
 * <tr><td>2022-12-09     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_rt4813.h"
#include "rtthread.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

static uint8_t hl_drv_rt4813_test(int argc, char argv[])
{
    uint8_t val;
    hl_rt4813_reg01_t reg_val;
    hl_drv_rt4813_init();

    //获取LG的驱动能力值
    hl_drv_rt4813_ctrl(HL_DRV_RT4813_LG_DRV_ABILITY_GET, (uint8_t *)&reg_val, sizeof(uint8_t));

    if (RT4813_LG_DRV_ABILITY == reg_val.DRV_SEL) {
        rt_kprintf("LG driving capability:0x%x, Fastest (default)\n", reg_val.DRV_SEL);
    }
    
    //开启放电功能
    val = 1;
    hl_drv_rt4813_ctrl(HL_DRV_RT4813_LG_DRV_ABILITY_GET, &val, sizeof(uint8_t));
    rt_kprintf("load discharge start\n");

    return RT4813_FUNC_RET_OK;

}

MSH_CMD_EXPORT(hl_drv_rt4813_test, rt4813 test);

/*
 * EOF
 */