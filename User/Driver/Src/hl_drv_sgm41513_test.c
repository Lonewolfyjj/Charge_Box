/**
 * @file hl_drv_charger_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-18
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
 * <tr><td>2022-10-18     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_sgm41513.h"
#include "stdlib.h"
#include "rtthread.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
 
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void charge_status_printf()
{
    rt_uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_CHARGE_STATUS, &data, 1);
    rt_kprintf("charge status: \n");
    switch (data) {
        case NOT_CHARGE:
            rt_kprintf("    not charge\n");
            break;
        case CHARGE_PRE:
            rt_kprintf("    pre charge\n");
            break;
        case CHARGE_RUN:
            rt_kprintf("    fast charge\n");
            break;
        case CHARGE_TEMINATED:
            rt_kprintf("    charge stop\n");
            break;
        default:
            break;
    }
}

void input_power_vbus_printf()
{
    uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_INPUT_POWER_STATUS, &data, 1);
    rt_kprintf("input power status: \n");
    if (data == EXIST_INPUT_POWER) {
        rt_kprintf("    exist input power\n");
    } else {
        rt_kprintf("    not input power\n");
    }
}



void battery_temp_status_printf()
{
    uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_BATTERY_TEMP_STATUS, &data, 1);
    rt_kprintf("battery temp status: \n");
    switch (data) {
        case NORMAL:
            rt_kprintf("    temp normal\n");
            break;
        case TEMP_WARM:
            rt_kprintf("    temp warm\n");
            break;
        case TEMP_COOL:
            rt_kprintf("    temp cool\n");
            break;
        case TEMP_COLD:
            rt_kprintf("    temp cold\n");
            break;
        case TEMP_HOT:
            rt_kprintf("    temp hot\n");
            break;
        default:
            break;
    }

}

void battery_error_status_printf()
{
    uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_BATTERY_ERROR_STATUS, &data, 1);
    rt_kprintf("battery error status: \n");
    if (data == NORMAL) {
        rt_kprintf("    battery normal\n");
    } else {
        rt_kprintf("    battery error\n");
    }
}


void charge_error_status_printf()
{
    uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_CHARGE_ERROR_STATUS, &data, 1);
    rt_kprintf("charge error status: \n");
    switch (data) {
        case NORMAL:
            rt_kprintf("    charge normal\n");
            break;
        case CHARGE_INPUT_ERROR:
            rt_kprintf("    input error\n");
            break;
        case CHARGE_THERMAL_SHUTDOWN:
            rt_kprintf("    thermal shutdown\n");
            break;
        case CHARGE_SAFT_TIMER_EXPIRED:
            rt_kprintf("    safe timer expired\n");
            break;
        default:
            break;
    }
}

void temp_regulation_status_printf()
{
   
   uint8_t data = 0;
    hl_drv_sgm41513_ctrl(GET_THERMAL_REGULATION_STATUS, &data, 1);
    rt_kprintf("temp regulation status: \n");
    if (data == EXIST_THERMAL_REGULATION) {
        rt_kprintf("    exist temp regulation\n");
    } else {
        rt_kprintf("    not exist temp regulation\n");
    } 
}

/* Private function(only *.c)  -----------------------------------------------*/

void hl_drv_sgm41513_test(int argc, char *argv[])
{
    uint8_t data = 0;

    hl_drv_sgm41513_init();
    
    /* 打印所有寄存器的值 */
    hl_drv_sgm41513_ctrl(PRINTF_REG_VAL, &data, 1);
    /* 打印充电状态 */
    charge_status_printf();

    /* 打印输入电源（VBUS)状态 */
    input_power_vbus_printf();

    /* 获取电池温度状态 */
    battery_temp_status_printf();

    /* 获取电池故障状态 */
    battery_error_status_printf();

    /* 获取充电故障状态 */
    charge_error_status_printf();

    /* 打印热调节状态 */
    temp_regulation_status_printf();

    rt_kprintf("\n*************************************************************\n");

}

MSH_CMD_EXPORT(hl_drv_sgm41513_test, sgm41513 driver test);
/*
 * EOF
 */