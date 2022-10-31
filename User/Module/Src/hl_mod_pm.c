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
    uint8_t                sys_vol_status;      //系统电压调节状态
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
    bool                  charge_it_update_flag;
    bool                  soc_it_update_flag;
    void*                 msg_hd;
    struct rt_thread      pm_thread;
    int                   thread_exit_flag; 
}hl_mod_pm_info_st;

typedef enum _hl_mod_pm_bat_info_e {
    HL_MOD_PM_BAT_INFO_SOC = 0,
    HL_MOD_PM_BAT_INFO_VOL,
    HL_MOD_PM_BAT_INFO_CUR,
    HL_MOD_PM_BAT_INFO_TEMP,
    HL_MOD_PM_BAT_INFO_SOH,
    HL_MOD_PM_BAT_INFO_CYCLE,
} hl_mod_pm_bat_info_e;

typedef enum _hl_mod_pm_charge_status_e {
    HL_MOD_PM_CHAR_STAT = 0,
    HL_MOD_PM_VBUS_STAT,
    HL_MOD_PM_INPUT_STAT,
    HL_MOD_PM_VINDPM_STAT,
    HL_MOD_PM_IINDPM_STAT,
    HL_MOD_PM_SYS_VOL_STAT,
    HL_MOD_PM_BAT_ERR_STAT,
    HL_MOD_PM_CHAR_ERR_STAT,
    HL_MOD_PM_BOOST_ERR_STAT,
    HL_MOD_PM_WATCHDOG_ERR_STAT
};
/* define --------------------------WATCHDOG_ERR_STAT------------------------------------------*/

#define DBG_LOG rt_kprintf

#define PM_THREAD_STACK_SIZE 512

/* variables -----------------------------------------------------------------*/

static hl_mod_pm_info_st  pm_mod_info = {
    .pm_init_flag     = false,
    .pth_start_flag   = false,
    .update_flag      = false,
    .charge_it_update_flag      = false,
    .soc_it_update_flag  = false,
    .msg_hd           = RT_NULL,
    .pm_thread        = { 0 },
    .thread_exit_flag = 0
};

static hl_mod_pm_bat_info_st bat_info = {
    .soc_val.soc     = 0,
    .soc_val.soc_d   = 0,
    .current     = 0,
    .cycle       = 0,
    .soh         = 0,
    .temp.temp   = 0,
    .temp.temp_d = 0,
    .voltage     = 0,
};

static hl_mod_pm_charge_info_st old_charge_info = {
    .charge_status = 0,
    .vbus_status = 0,
    .input_power_status = 0,
    .vindpm_status = 0,
    .iindpm_status = 0
};

static hl_mod_pm_error_info_st old_charge_error_info = {
    .bat_error_status = 0,
    .charge_error_status = 0,
    .boost_mode_status = 0,
    .watchdog_error_status = 0,
};

static hl_mod_pm_charge_info_st new_charge_info;
static hl_mod_pm_error_info_st new_charge_error_info;

static uint8_t pm_thread_stack[PM_THREAD_STACK_SIZE] = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/

/**
 * @brief 电量计中断服务函数
 * @param [in] args 
 * @date 2022-10-27
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-27      <td>yijiujun     <td>新建
 * </table>
 */
static void _soc_gpio_irq_handle(void* args)
{
    pm_mod_info.soc_it_update_flag = true;
}

static inline void _guage_soc_gpio_irq_init()
{
    hl_hal_gpio_init(GPIO_GAUGE_INT);
    hl_hal_gpio_attach_irq(GPIO_GAUGE_INT, PIN_IRQ_MODE_FALLING, _soc_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_GAUGE_INT, PIN_IRQ_ENABLE);
}

static inline void _guage_soc_gpio_irq_deinit()
{
    hl_hal_gpio_irq_enable(GPIO_GAUGE_INT, PIN_IRQ_DISABLE);
    hl_hal_gpio_deinit(GPIO_GAUGE_INT);
}

static inline void _guage_soc_gpio_irq_enable(bool flag)
{
    if (flag) {
        hl_hal_gpio_irq_enable(GPIO_GAUGE_INT, PIN_IRQ_ENABLE);
    } else {
        hl_hal_gpio_irq_enable(GPIO_GAUGE_INT, PIN_IRQ_DISABLE);
    }
}

/**
 * @brief 充电中断服务函数
 * @param [in] args 
 * @date 2022-10-27
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-27      <td>yijiujun     <td>新建
 * </table>
 */
static void _charge_gpio_irq_handle(void* args)
{
    pm_mod_info.charge_it_update_flag = true;
}

static inline void _charge_gpio_irq_init()
{
    hl_hal_gpio_init(GPIO_CH_INT_N);
    hl_hal_gpio_attach_irq(GPIO_CH_INT_N, PIN_IRQ_MODE_FALLING, _charge_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_CH_INT_N, PIN_IRQ_ENABLE);
}

static inline void _charge_gpio_irq_deinit()
{
    hl_hal_gpio_irq_enable(GPIO_CH_INT_N, PIN_IRQ_DISABLE);
    hl_hal_gpio_deinit(GPIO_CH_INT_N);
}

/**
 * @brief 霍尔传感器的引脚初始化
 * @date 2022-10-27
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-27      <td>yijiujun     <td>新建
 * </table>
 */
static void _hall_gpio_init()
{
    hl_hal_gpio_init(GPIO_HALL_TX2);
    hl_hal_gpio_init(GPIO_HALL_BOX);
    hl_hal_gpio_init(GPIO_HALL_TX1);
    hl_hal_gpio_init(GPIO_HALL_RX);

    hl_hal_gpio_init(GPIO_RX_POW_EN);
    hl_hal_gpio_init(GPIO_TX1_POW_EN);
    hl_hal_gpio_init(GPIO_TX2_POW_EN);
}

static void _hall_gpio_deinit()
{
    hl_hal_gpio_deinit(GPIO_TX2_POW_EN);
    hl_hal_gpio_deinit(GPIO_TX1_POW_EN);
    hl_hal_gpio_deinit(GPIO_RX_POW_EN);

    hl_hal_gpio_deinit(GPIO_HALL_RX);
    hl_hal_gpio_deinit(GPIO_HALL_TX1);
    hl_hal_gpio_deinit(GPIO_HALL_BOX);
    hl_hal_gpio_deinit(GPIO_HALL_TX2);
}





static void _pm_update_bat_info(hl_mod_pm_bat_info_e type)
{
    hl_mod_pm_bat_info_st* p_bat_info;

    p_bat_info = &(bat_info);

    switch (type) {
        case HL_MOD_PM_BAT_INFO_SOC: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_SOC, &(p_bat_info->soc_val), sizeof(p_bat_info->soc_val));
            DBG_LOG("soc:%d . %d\n", p_bat_info->soc_val.soc, p_bat_info->soc_val.soc_d);
        } break;
        case HL_MOD_PM_BAT_INFO_VOL: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_VOLTAGE, &(p_bat_info->voltage), sizeof(p_bat_info->voltage));
            DBG_LOG("voltage:%d\n", p_bat_info->voltage);
        } break;
        case HL_MOD_PM_BAT_INFO_CUR: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_CURRENT, &(p_bat_info->current), sizeof(p_bat_info->current));
            DBG_LOG("current:%d\n", p_bat_info->current);
        } break;
        case HL_MOD_PM_BAT_INFO_TEMP: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_TEMP, &(p_bat_info->temp), sizeof(p_bat_info->temp));
            DBG_LOG("temp:%d . %d\n", p_bat_info->temp.temp, p_bat_info->temp.temp_d);
        } break;
        case HL_MOD_PM_BAT_INFO_SOH: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_SOH, &(p_bat_info->soh), sizeof(p_bat_info->soh));
            DBG_LOG("soh:%d\n", p_bat_info->soh);
        } break;
        case HL_MOD_PM_BAT_INFO_CYCLE: {
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_GET_CYCLE_COUNT, &(p_bat_info->cycle), sizeof(p_bat_info->cycle));
            DBG_LOG("cycle:%d\n", p_bat_info->cycle);
        } break;
        default:
            break;
    }
}

static inline void _pm_update_bat_info_check(void)
{
    hl_drv_guage_check_it_flag_st it_flag;
    uint8_t                       flag;
    bool                          update_flag = false;

    if (pm_mod_info.soc_it_update_flag == true) {
        it_flag.it_flag = HL_DRV_GUAGE_IT_FLAG_SOC;
        it_flag.ret     = 0;

        hl_drv_cw2215_ctrl(HL_DRV_GUAGE_CHECK_IT_FLAG, &it_flag, sizeof(it_flag));

        if (it_flag.ret == 1) {
            flag = HL_DRV_GUAGE_IT_FLAG_SOC;
            hl_drv_cw2215_ctrl(HL_DRV_GUAGE_CLEAR_IT_FLAG, &flag, sizeof(flag));
            update_flag = true;
        }

        pm_mod_info.soc_it_update_flag = false;
    }

    if (update_flag == true) {
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_SOC);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_VOL);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_CUR);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_TEMP);
    }
}





static void _pm_charge_status_info()
{
    uint8_t reg_val;

    hl_drv_sgm41513_ctrl(GET_CHARGE_STATUS, &new_charge_info.charge_status, 1);

    hl_drv_sgm41513_ctrl(GET_INPUT_POWER_STATUS, &new_charge_info.input_power_status, 1);

    hl_drv_sgm41513_ctrl(GET_BATTERY_ERROR_STATUS, &new_charge_error_info.bat_error_status, 1);

    hl_drv_sgm41513_ctrl(GET_CHARGE_ERROR_STATUS, &new_charge_error_info.charge_error_status, 1);

    hl_drv_sgm41513_ctrl(GET_BOOST_MODE_ERROR_STATUS, &new_charge_error_info.boost_mode_status, 1);

    hl_drv_sgm41513_ctrl(GET_WATCHDOG_ERROR_STATUS, &new_charge_error_info.watchdog_error_status, 1);
}

static inline uint8_t _pm_charge_it_status(uint8_t old_status, uint8_t new_status)
{
    if (old_status == new_status) {
        return 0;
    } else {
        return 1;
    }
}

static void pm_charge_it_status_check()
{
    uint8_t count, change_arr[10] = {0};

    _pm_charge_status_info();

    change_arr[0] = _pm_charge_it_status(old_charge_info.charge_status, new_charge_info.charge_status);
    change_arr[1] = _pm_charge_it_status(old_charge_info.vbus_status, new_charge_info.vbus_status);
    change_arr[2] = _pm_charge_it_status(old_charge_info.input_power_status, new_charge_info.input_power_status);
    change_arr[3] = _pm_charge_it_status(old_charge_info.vindpm_status, new_charge_info.vindpm_status);
    change_arr[4] = _pm_charge_it_status(old_charge_info.iindpm_status, new_charge_info.iindpm_status);
    change_arr[5] = _pm_charge_it_status(old_charge_error_info.bat_error_status, new_charge_error_info.bat_error_status);
    change_arr[6] = _pm_charge_it_status(old_charge_error_info.charge_error_status, new_charge_error_info.charge_error_status);
    change_arr[7] = _pm_charge_it_status(old_charge_error_info.boost_mode_status, new_charge_error_info.boost_mode_status);
    change_arr[8] = _pm_charge_it_status(old_charge_error_info.watchdog_error_status, new_charge_error_info.watchdog_error_status);
    
    for (count = 0; count < 9; count++) {
        if (change_arr[count] == 1) {
            break;
        }
    }
    switch (count) {
        case HL_MOD_PM_CHAR_STAT:
            rt_kprintf("    0\n");
            break;
        case HL_MOD_PM_VBUS_STAT:
            rt_kprintf("    1\n");
            break;
        case HL_MOD_PM_INPUT_STAT:
            rt_kprintf("    2\n");
            break;
        case HL_MOD_PM_VINDPM_STAT:
            rt_kprintf("    3\n");
            break;
        case HL_MOD_PM_IINDPM_STAT:
            rt_kprintf("    4\n");
            break;
        case HL_MOD_PM_SYS_VOL_STAT:
            rt_kprintf("    5\n");
            break;
        case HL_MOD_PM_BAT_ERR_STAT:
            rt_kprintf("    6\n");
            break;
        case HL_MOD_PM_CHAR_ERR_STAT:
            rt_kprintf("    7\n");
            break;
        case HL_MOD_PM_BOOST_ERR_STAT:
            rt_kprintf("    8\n");
            break;
        case HL_MOD_PM_WATCHDOG_ERR_STAT:
            rt_kprintf("    9\n");
            break;
        default:
            break;
    }

}

static void _pm_charge_status_info_check(void)
{
    hl_sgm41513_charge_status_t charge_stu;
    if (pm_mod_info.charge_it_update_flag == true) {
        hl_drv_sgm41513_ctrl(PRINTF_REG_VAL, RT_NULL, 1);
        
        pm_charge_it_status_check();
        hl_drv_sgm41513_read_reg(REG08_ADDR, (rt_uint8_t *)&charge_stu);
        rt_kprintf("VBUS_STAT:%x, CHRG_STAT:%x, PG_STAT:%x, THERM_STAT:%x, VSYS_STAT:%x\n", 
        charge_stu.VBUS_STAT, charge_stu.CHRG_STAT, charge_stu.PG_STAT, charge_stu.THERM_STAT, charge_stu.VSYS_STAT);
        rt_kprintf("\n*************charge*******it********\n\n");
        // switch (new_charge_info.charge_status) {
        //     case NOT_CHARGE:
        //         rt_kprintf("    not charge\n");
        //         break;
        //     case CHARGE_PRE:
        //         rt_kprintf("    pre charge\n");
        //         break;
        //     case CHARGE_RUN:
        //         rt_kprintf("    fast charge\n");
        //         break;
        //     case CHARGE_TEMINATED:
        //         rt_kprintf("    charge stop\n");
        //         break;
        //     default:
        //         break;
        // }
        // if (new_charge_info.input_power_status == EXIST_INPUT_POWER) {
        //     rt_kprintf("    exist input power\n");
        // } else {
        //     rt_kprintf("    not input power\n");
        // }
        // switch (new_charge_error_info.charge_error_status) {
        //     case NORMAL:
        //         rt_kprintf("    charge normal\n");
        //         break;
        //     case CHARGE_INPUT_ERROR:
        //         rt_kprintf("    input error\n");
        //         break;
        //     case CHARGE_THERMAL_SHUTDOWN:
        //         rt_kprintf("    thermal shutdown\n");
        //         break;
        //     case CHARGE_SAFT_TIMER_EXPIRED:
        //         rt_kprintf("    safe timer expired\n");
        //         break;
        //     default:
        //         break;
        // }
        old_charge_info = new_charge_info;
        old_charge_error_info = new_charge_error_info;
        pm_mod_info.charge_it_update_flag = false;
    }
}

static void _pm_hall_load_info()
{

}

static void _pm_hall_load_info_check(void)
{
    
}


static void _pm_thread_entry(void* arg)
{
    while (pm_mod_info.thread_exit_flag == 0) {
        _pm_update_bat_info_check();

        _pm_charge_status_info_check();
        rt_thread_mdelay(10);
    }

    pm_mod_info.thread_exit_flag = -1;
}

/* Exported functions --------------------------------------------------------*/

int hl_mod_pm_init(void* msg_hd)
{
    int ret;
    if (pm_mod_info.pm_init_flag == true) {
        DBG_LOG("pm is already init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    /* 电量计初始化 */
    ret = hl_drv_cw2215_init();
    if (ret == CW2215_FUNC_RET_ERR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    _guage_soc_gpio_irq_init();

    /* 充电驱动初始化 */
    ret = hl_drv_sgm41513_init();
    if (ret == SGM41513_ERROR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    _charge_gpio_irq_init();

    /* 霍尔感应引脚初始化 */
    _hall_gpio_init();

    pm_mod_info.msg_hd = msg_hd;
    DBG_LOG("pm init success!\n");
    pm_mod_info.pm_init_flag = true;
    return HL_MOD_PM_FUNC_RET_OK;
}

int hl_mod_pm_deinit(void)
{
    int ret;
    if (pm_mod_info.pm_init_flag == false) {
        DBG_LOG("pm is already deinited!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    hl_mod_pm_stop();
    _hall_gpio_deinit();

    _charge_gpio_irq_deinit();
    ret = hl_drv_sgm41513_deinit();
    if (ret == SGM41513_ERROR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    _guage_soc_gpio_irq_deinit();
    ret = hl_drv_cw2215_deinit();
    if (ret == CW2215_FUNC_RET_ERR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    pm_mod_info.msg_hd = RT_NULL;
    DBG_LOG("pm deinit success!\n");
    pm_mod_info.pm_init_flag = false;
    return HL_MOD_PM_FUNC_RET_OK;
}

int hl_mod_pm_start(void)
{
    int      ret;
    rt_err_t rt_err;

    if (pm_mod_info.pm_init_flag == false) {
        DBG_LOG("pm is not init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    if (pm_mod_info.pth_start_flag == true) {
        ret = hl_mod_pm_stop();
        if (ret == HL_MOD_PM_FUNC_RET_ERR) {
            DBG_LOG("pm try stop failed\n");
            return HL_MOD_PM_FUNC_RET_ERR;
        }
    }

    pm_mod_info.soc_it_update_flag = false;

    _guage_soc_gpio_irq_enable(true);

    pm_mod_info.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(pm_mod_info.pm_thread), "hl_mod_pm_thread", _pm_thread_entry, RT_NULL, pm_thread_stack,
                            sizeof(pm_thread_stack), 6, 10);
    if (rt_err == RT_ERROR) {
        DBG_LOG("pm thread create failed\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    rt_thread_startup(&(pm_mod_info.pm_thread));

    DBG_LOG("pm start success\n");

    pm_mod_info.pth_start_flag = true;

    return HL_MOD_PM_FUNC_RET_OK;
}

int hl_mod_pm_stop(void)
{
    if (pm_mod_info.pm_init_flag == false) {
        DBG_LOG("pm is not init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    if (pm_mod_info.pth_start_flag == false) {
        DBG_LOG("pm already stop\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    _guage_soc_gpio_irq_enable(false);

    pm_mod_info.thread_exit_flag = 1;

    DBG_LOG("wait pm thread exit\n");

    while (pm_mod_info.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    DBG_LOG("pm stop success\n");

    pm_mod_info.pth_start_flag = false;

    return HL_MOD_PM_FUNC_RET_OK;
}

int hl_mod_pm_ctrl(int op, void* arg, int arg_size)
{
    if (pm_mod_info.pm_init_flag == false) {
        DBG_LOG("pm is not init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    switch (op) {
        case SEGT_INPUT_CURRENT_LIMIT_VAL:
            set_input_current_limit(1);
            break;
        case SET_CHARGE_STATUS:
            set_charge_status(1);
            break;
        case SET_FAST_CHARGE_CURRENT_VAL:
            set_fast_charge_current(1);
            break;
        default:
            break;
    }
    return HL_MOD_PM_FUNC_RET_OK;
}

/*
 * EOF
 */