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

void hl_drv_sgm41513_test(int argc, char *argv[])
{
    uint8_t data = 0;
    if (argc <= 1) {
        rt_kprintf("format : [cmd 0/1]\n");
        return;
    }
    hl_drv_sgm41513_init();
    if (atoi(argv[1]) == 0) {
        /* 重置所有寄存器的R/W位至默认值并重置安全定时器 */
        hl_drv_sgm41513_ctrl(REST_ALL_REG_VAL, &data, 1);

        /* 设置充电状态为使能 */
        data = CHARGER_ENABLE;
        hl_drv_sgm41513_ctrl(SET_CHARGE_STATUS, &data, 1);

        /* 设置充电终止为使能 */
        data = CHARGER_ENABLE;
        hl_drv_sgm41513_ctrl(SET_CHARGEE_TERMINATION_ENABLE, &data, 1);
        
        /* 设置恒流时的的充电电流值（快速充电时的电流大小） */
        data = 0x34;    //1980mA (110100)
        hl_drv_sgm41513_ctrl(SET_FAST_CHARGE_CURRENT_VAL, &data, 1);
        
        /* 设置预充电的电流 */
        data = 0x00;    //10mA
        hl_drv_sgm41513_ctrl(SET_PRE_CHARGE_CURRENT_VAL, &data, 1);

        /* 设置终止充电的电流 */
        data = 0x0a;    //120mA (1010)
        hl_drv_sgm41513_ctrl(SET_STOP_CHARGE_CURRENT_VAL, &data, 1);

        /* 设置循环充电电压限制值L */
        data = 0x00;    //100mV
        hl_drv_sgm41513_ctrl(SET_RECHARGE_VOL_VAL, &data, 1);

        /* 设置电池充电的电压阈值H  */
        data = 0x0b;    //4.208V (01011)
        hl_drv_sgm41513_ctrl(SET_CHARGE_VOL_LIMIT_VAL, &data, 1);

        /* 设置输入电压动态电源管理阈值  */
        data = 0x06;    //4.5V (0110)
        hl_drv_sgm41513_ctrl(SET_VINDPM_THRESHOLD_VAL, &data, 1);

        /* 设置热调节阈值 （0-80，1-120）*/
        data = 0x01;
        hl_drv_sgm41513_ctrl(SET_THERMAL_REGULATION_MAX_VAL, &data, 1);

    } else {
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
    }
    

}

MSH_CMD_EXPORT(hl_drv_sgm41513_test, sgm41513 driver test);
/*
 * EOF
 */