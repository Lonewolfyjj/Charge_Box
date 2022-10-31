/**
 * @file hl_mod_pm.c
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
#include "hl_drv_cw2215.h"
#include "hl_drv_sgm41513.h"
#include "hl_hal_gpio.h"
#include "pin.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_mod_pm_bat_info_st {
    hl_st_drv_guage_soc_t  soc_val;
    uint16_t               voltage;
    int32_t                current;
    hl_st_drv_guage_temp_t temp;
    uint8_t                soh;
    uint32_t               cycle;
} hl_mod_pm_bat_info_st;

typedef struct _hl_mod_pm_error_info_st {
    uint8_t                bat_error_status;        //电池故障
    uint8_t                charge_error_status;     //充电故障状态
    uint8_t                boost_mode_status;       //Boost模式故障状态
    uint8_t                watchdog_error_status;   //看门狗故障状态
} hl_mod_pm_error_info_st;

typedef struct _hl_mod_pm_charge_info_st {
    uint8_t                charge_status;       //充电状态
    uint8_t                vbus_status;         //VBUS输入状态
    uint8_t                input_power_status;  //输入电源状态
    uint8_t                vindpm_status;       //VINDPM 电压动态电源管理状态
    uint8_t                iindpm_status;       //IINDPM 电流动态电源管理状态
} hl_mod_pm_charge_info_st;

typedef struct {
    bool                  pth_start_flag;
    bool                  update_flag;
}hl_mod_pm_run_pthread_st;

typedef struct _hl_mod_pm_info_st {
    bool                  pm_init_flag;
    bool                  pth_start_flag;
    bool                  update_flag;
    bool                  soc_update_flag;
    void*                 msg_hd;
    struct rt_thread      pm_thread;
    int                   thread_exit_flag;
    hl_mod_pm_bat_info_st bat_info;
    hl_mod_pm_charge_info_st charge_info; 
    hl_mod_pm_error_info_st charge_error_info;  //充电模块故障状态 
}hl_mod_pm_info_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

#define PM_THREAD_STACK_SIZE 512

/* variables -----------------------------------------------------------------*/

static hl_mod_pm_info_st  pm_mod_info = {
    .pm_init_flag     = false,
    .pth_start_flag   = false,
    .update_flag      = false,
    .soc_update_flag  = false,
    .msg_hd           = RT_NULL,
    .pm_thread        = { 0 },
    .thread_exit_flag = 0,
    .bat_info         = {
        .soc_val.soc     = 0,
        .soc_val.soc_d   = 0,
        .current     = 0,
        .cycle       = 0,
        .soh         = 0,
        .temp.temp   = 0,
        .temp.temp_d = 0,
        .voltage     = 0,
    },
    .charge_info    = {
        .charge_status = 0,
        .vbus_status = 0,
        .input_power_status = 0,
        .vindpm_status = 0,
        .iindpm_status = 0
    },
    .charge_error_info = {
        .bat_error_status = 0,
        .charge_error_status = 0,
        .boost_mode_status = 0,
        .watchdog_error_status = 0,
    }
};

static uint8_t pm_thread_stack[PM_THREAD_STACK_SIZE] = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/



/* Exported functions --------------------------------------------------------*/

int hl_mod_pm_init(void* msg_hd)
{
    int ret;

    if (pm_mod_info.pm_init_flag == true) {
        DBG_LOG("pm is already inited!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    ret = hl_drv_cw2215_init();
    if (ret == CW2215_FUNC_RET_ERR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    _guage_soc_gpio_irq_init();

    
    hl_drv_sgm41513_init();
    _charge_gpio_irq_init();


    hall_gpio_init();

    pm_mod_info.msg_hd = msg_hd;
    DBG_LOG("pm init success!\n");

    pm_mod_info.pm_init_flag = true;

    return HL_MOD_PM_FUNC_RET_OK;
}

int hl_mod_pm_deinit(void)
{

}

int hl_mod_pm_start(void)
{
    
}

int hl_mod_pm_stop(void)
{

}

int hl_mod_pm_ctrl(int op, void* arg, int arg_size)
{

}

/*
 * EOF
 */