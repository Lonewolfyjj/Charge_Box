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
#include "n32l40x.h"
#include "hl_drv_cw2215.h"
#include "hl_drv_sgm41513.h"
#include "hl_hal_gpio.h"
#include "hl_util_config.h"
#include "hl_mod_typedef.h"
#include "pin.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_mod_pm_bat_info_st
{
    hl_st_drv_guage_soc_t  soc_val;
    uint16_t               voltage;
    int32_t                current;
    hl_st_drv_guage_temp_t temp;
    uint8_t                soh;
    uint32_t               cycle;
} hl_mod_pm_bat_info_st;

typedef struct _hl_mod_pm_error_info_st
{
    uint8_t bat_error_status;       //电池故障
    uint8_t charge_error_status;    //充电故障状态
    uint8_t boost_mode_status;      //Boost模式故障状态
    uint8_t watchdog_error_status;  //看门狗故障状态
} hl_mod_pm_error_info_st;

typedef struct _hl_mod_pm_charge_info_st
{
    uint8_t charge_status;       //充电状态
    uint8_t vbus_status;         //VBUS输入状态
    uint8_t vbus_connect_status;  //输入电源状态
    uint8_t sys_vol_status;      //系统电压调节状态
    uint8_t vindpm_status;       //VINDPM 输入电压动态电源管理状态
    uint8_t iindpm_status;       //IINDPM 输入电流动态电源管理状态
} hl_mod_pm_charge_info_st;

typedef struct _hl_mod_pm_hall_info_st
{
    uint8_t tx1_status;
    uint8_t tx2_status;
    uint8_t rx_status;
    uint8_t box_status;
    bool    tx1_irq_flag;
    bool    tx2_irq_flag;
    bool    rx_irq_flag;
    bool    box_irq_flag;
} hl_mod_pm_hall_info_st;
typedef struct _hl_mod_pm_info_st
{
    bool             pm_init_flag;           //PM模块初始化标志
    bool             pth_start_flag;         //线程启动标志
    bool             init_bat_update_flag;   //程序运行后，电池信息第一次更新标志
    void*            msg_hd;                 //消息队列句柄指针
    struct rt_thread pm_thread;              //线程ID
    int              thread_exit_flag;       //线程运行，退出标志
} hl_mod_pm_info_st;

typedef struct _hl_mod_pm_drv_init_st
{
    bool pm_init_guage_flag;   //电量计初始化标志
    bool pm_init_charge_flag;  //充电驱动初始化标志
} hl_mod_pm_drv_init_st;
typedef enum _hl_mod_pm_bat_info_e
{
    HL_MOD_PM_BAT_INFO_SOC = 0,
    HL_MOD_PM_BAT_INFO_VOL,
    HL_MOD_PM_BAT_INFO_CUR,
    HL_MOD_PM_BAT_INFO_TEMP,
    HL_MOD_PM_BAT_INFO_SOH,
    HL_MOD_PM_BAT_INFO_CYCLE,
} hl_mod_pm_bat_info_e;

/**
 * @brief 充电状态枚举类型
 * @date 2022-11-09
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-09      <td>yijiujun     <td>新建
 * </table>
 */
typedef enum _hl_mod_pm_charge_status_e
{
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
} hl_mod_pm_charge_status_e;

typedef enum _hl_mod_pm_temp_state_e {
    HL_MOD_PM_TEMP_FAULT,
    HL_MOD_PM_TEMP_CHCK,
    HL_TEMP_NOMAL
}hl_mod_pm_temp_state_e;

/* define --------------------------WATCHDOG_ERR_STAT------------------------------------------*/

#define DBG_LOG rt_kprintf

#define PM_THREAD_STACK_SIZE 512

#define PM_STAT_COMPARE(m, n) ((m) == (n) ? (0) : (1))

#define TEMP_MIN    -10
#define TEMP_MAX    45

/* variables -----------------------------------------------------------------*/

static uint8_t  temp_state = HL_MOD_PM_TEMP_FAULT;

static hl_mod_pm_info_st pm_mod_info = { .pm_init_flag          = false,
                                         .pth_start_flag        = false,
                                         .msg_hd                = RT_NULL,
                                         .pm_thread             = { 0 },
                                         .thread_exit_flag      = -1 };

static hl_mod_pm_bat_info_st bat_info = { .soc_val.soc   = 0,
                                          .soc_val.soc_d = 0,
                                          .current       = 0,
                                          .cycle         = 0,
                                          .soh           = 0,
                                          .temp.temp     = 0,
                                          .temp.temp_d   = 0,
                                          .voltage       = 0 };

static hl_mod_pm_charge_info_st old_charge_info = { .charge_status      = 0,
                                                    .vbus_status        = 0,
                                                    .vbus_connect_status = 0,
                                                    .sys_vol_status     = 0,
                                                    .vindpm_status      = 0,
                                                    .iindpm_status      = 0 };

static hl_mod_pm_error_info_st old_charge_error_info = {
    .bat_error_status = 0, .charge_error_status = 0, .boost_mode_status = 0, .watchdog_error_status = 0
};

static hl_mod_pm_hall_info_st hall_info = { .tx1_status   = 0,
                                            .tx2_status   = 0,
                                            .rx_status    = 0,
                                            .box_status   = 0,
                                            .tx1_irq_flag = false,
                                            .tx2_irq_flag = false,
                                            .rx_irq_flag  = false,
                                            .box_irq_flag = false };

static hl_mod_pm_charge_info_st new_charge_info;

static hl_mod_pm_error_info_st new_charge_error_info;

static uint8_t pm_thread_stack[PM_THREAD_STACK_SIZE] = { 0 };

static hl_mod_msg_handle_st _pm_msg_hd = { .msg_id = 0, .msg_send = RT_NULL };

static hl_mod_pm_drv_init_st _pm_drv_flag = { .pm_init_charge_flag = false, .pm_init_guage_flag = false };

static bool temp_ovmax_flag = false;
static bool  temp_ovmin_flag = false;

/* Private function(only *.c)  -----------------------------------------------*/

static int _mod_msg_send(uint8_t cmd, void* param, uint16_t len)
{
    if (pm_mod_info.msg_hd != RT_NULL && _pm_msg_hd.msg_send != RT_NULL) {
        _pm_msg_hd.msg_send(_pm_msg_hd.msg_id, cmd, param, len);
        return HL_MOD_PM_FUNC_RET_OK;
    }

    return HL_MOD_PM_FUNC_RET_ERR;
}

/**
 * @brief 负载TX1的中断服务函数
 * @param [in] args 
 * @date 2022-10-31
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-31      <td>yijiujun     <td>新建
 * </table>
 */
static void _hall_load_tx1_irq_handle(void* args)
{
    hall_info.tx1_irq_flag = true;
}

static void _hall_load_tx2_irq_handle(void* args)
{
    hall_info.tx2_irq_flag = true;
}

static void _hall_load_rx_irq_handle(void* args)
{
    hall_info.rx_irq_flag = true;
}

static void _hall_box_irq_handle(void* args)
{
    hall_info.box_irq_flag = true;
}

/**
 * @brief 霍尔传感器上，负载对应的引脚初始化
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
    hl_hal_gpio_clock_init();

    hl_hal_gpio_init(GPIO_HALL_TX2);
    hl_hal_gpio_attach_irq(GPIO_HALL_TX2, PIN_IRQ_MODE_RISING_FALLING, _hall_load_tx2_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_TX2, PIN_IRQ_DISABLE);

    hl_hal_gpio_init(GPIO_HALL_BOX);
    hl_hal_gpio_attach_irq(GPIO_HALL_BOX, PIN_IRQ_MODE_RISING_FALLING, _hall_box_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_BOX, PIN_IRQ_DISABLE);

    hl_hal_gpio_init(GPIO_HALL_TX1);
    hl_hal_gpio_attach_irq(GPIO_HALL_TX1, PIN_IRQ_MODE_RISING_FALLING, _hall_load_tx1_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_TX1, PIN_IRQ_DISABLE);

    hl_hal_gpio_init(GPIO_HALL_RX);
    hl_hal_gpio_attach_irq(GPIO_HALL_RX, PIN_IRQ_MODE_RISING_FALLING, _hall_load_rx_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_RX, PIN_IRQ_DISABLE);

    hl_hal_gpio_init(GPIO_1V8_EN);
    hl_hal_gpio_init(GPIO_BOOST_EN);
    hl_hal_gpio_init(GPIO_RX_POW_EN);
    hl_hal_gpio_init(GPIO_TX1_POW_EN);
    hl_hal_gpio_init(GPIO_TX2_POW_EN);

    hl_hal_gpio_high(GPIO_1V8_EN);
    hl_hal_gpio_high(GPIO_BOOST_EN);
    hl_hal_gpio_low(GPIO_RX_POW_EN);
    hl_hal_gpio_low(GPIO_TX1_POW_EN);
    hl_hal_gpio_low(GPIO_TX2_POW_EN);
}

static void _hall_gpio_deinit()
{
    hl_hal_gpio_deinit(GPIO_TX2_POW_EN);
    hl_hal_gpio_deinit(GPIO_TX1_POW_EN);
    hl_hal_gpio_deinit(GPIO_RX_POW_EN);
    hl_hal_gpio_deinit(GPIO_BOOST_EN);
    hl_hal_gpio_deinit(GPIO_1V8_EN);

    hl_hal_gpio_deinit(GPIO_HALL_RX);
    hl_hal_gpio_deinit(GPIO_HALL_TX1);
    hl_hal_gpio_deinit(GPIO_HALL_BOX);
    hl_hal_gpio_deinit(GPIO_HALL_TX2);
}

static inline void _hall_gpio_irq_enable(bool flag)
{
    if (flag) {
        hl_hal_gpio_irq_enable(GPIO_HALL_RX, PIN_IRQ_ENABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_TX1, PIN_IRQ_ENABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_BOX, PIN_IRQ_ENABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_TX2, PIN_IRQ_ENABLE);
    } else {
        hl_hal_gpio_irq_enable(GPIO_HALL_RX, PIN_IRQ_DISABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_TX1, PIN_IRQ_DISABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_BOX, PIN_IRQ_DISABLE);
        hl_hal_gpio_irq_enable(GPIO_HALL_TX2, PIN_IRQ_DISABLE);
    }
}
static inline void _hall_load_charge_enable(HL_GPIO_PORT_E gpio_index, bool enable_val)
{
    if (enable_val == ENABLE) {
        hl_hal_gpio_high(gpio_index);
    } else {
        hl_hal_gpio_low(gpio_index);
    }
}

/**
 * @brief 电池信息更新获取
 * @param [in] type 
 * @date 2022-10-29
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-29      <td>yijiujun     <td>新建
 * </table>
 */
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
    hl_drv_guage_it_flag_e it_flag;
    uint8_t                reg_val;
    bool                   update_flag = false;

    hl_drv_cw2215_ctrl(HL_DRV_GUAGE_CHECK_IT_FLAG, &it_flag, sizeof(it_flag));

    if (it_flag & HL_DRV_GUAGE_IT_FLAG_SOC) {   //检测是否产生电量变化中断
        update_flag = true;
    }

    if (it_flag & HL_DRV_GUAGE_IT_FLAG_TMAX) {  //检测是否产生温度超最高阈值中断
        temp_ovmax_flag = true;
    }

    if (it_flag & HL_DRV_GUAGE_IT_FLAG_TMIN) {  //检测是否产生温度超最低阈值中断
        temp_ovmin_flag = true;
    }

    hl_drv_cw2215_ctrl(HL_DRV_GUAGE_CLEAR_IT_FLAG, &it_flag, sizeof(it_flag));

    if (pm_mod_info.init_bat_update_flag == true) {
        update_flag                      = true;
        pm_mod_info.init_bat_update_flag = false;
    }

    if (update_flag == true) {
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_SOC);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_VOL);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_CUR);
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_TEMP);

        _mod_msg_send(HL_MOD_PM_SOC_MSG, RT_NULL, 0);
    }

    /* 温度中断报警，温度超过限定值，在-10°c 和 45°c之外，关闭电池充电*/
    if (temp_ovmax_flag == true || temp_ovmin_flag == true) {
        switch (temp_state) {
            case HL_MOD_PM_TEMP_FAULT:                         //关闭充电
                reg_val = 0;
                hl_drv_sgm41513_ctrl(SET_CHARGE_STATUS, &reg_val, sizeof(uint8_t));
                _mod_msg_send(HL_MOD_PM_TEMP_FAULT_MSG, RT_NULL, 0);
                temp_state = 1;
                break;
            case HL_MOD_PM_TEMP_CHCK:                         //检测温度是否正常
                _pm_update_bat_info(HL_MOD_PM_BAT_INFO_TEMP);
                DBG_LOG("-----temp fault: %d°c-----\n", bat_info.temp.temp);
                if (bat_info.temp.temp > TEMP_MIN && bat_info.temp.temp < TEMP_MAX) {
                    temp_state = HL_TEMP_NOMAL;  
                }
                break;
            case HL_TEMP_NOMAL:                         //温度警报解除，恢复充电
                temp_state = HL_MOD_PM_TEMP_FAULT;
                temp_ovmax_flag = false;
                temp_ovmin_flag = false;
                reg_val = 1;
                hl_drv_sgm41513_ctrl(SET_CHARGE_STATUS, &reg_val, sizeof(uint8_t));
                _mod_msg_send(HL_MOD_PM_TEMP_NOMAL_MSG, RT_NULL, 0);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief 充电状态信息获取
 * @date 2022-10-29
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-29      <td>yijiujun     <td>新建
 * </table>
 */

static void _pm_get_charge_status_info()
{
    uint8_t reg_val;
    hl_drv_sgm41513_ctrl(GET_CHARGE_STATUS, &new_charge_info.charge_status, 1);
    hl_drv_sgm41513_ctrl(GET_VBUS_STATUS, &new_charge_info.vbus_status, 1);
    hl_drv_sgm41513_ctrl(GET_VBUS_CONNECT_STATE, &new_charge_info.vbus_connect_status, 1);
    hl_drv_sgm41513_ctrl(GET_VINDPM_STATUS, &new_charge_info.vindpm_status, 1);
    hl_drv_sgm41513_ctrl(GET_IINDPM_STATUS, &new_charge_info.iindpm_status, 1);
    hl_drv_sgm41513_ctrl(GET_SYS_VOL_REGULATION_STATUS, &new_charge_info.sys_vol_status, 1);

    hl_drv_sgm41513_ctrl(GET_BATTERY_ERROR_STATUS, &new_charge_error_info.bat_error_status, 1);
    hl_drv_sgm41513_ctrl(GET_CHARGE_ERROR_STATUS, &new_charge_error_info.charge_error_status, 1);
    hl_drv_sgm41513_ctrl(GET_BOOST_MODE_ERROR_STATUS, &new_charge_error_info.boost_mode_status, 1);
    hl_drv_sgm41513_ctrl(GET_WATCHDOG_ERROR_STATUS, &new_charge_error_info.watchdog_error_status, 1);

    hl_drv_sgm41513_ctrl(GET_VBUS_IN_DET_STATUS, &reg_val, 1);  //只用作清除中断标志位
}

/**
 * @brief 充电状态的变化检测，并处理
 * @param [in] val 
 * @date 2022-10-31
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-31      <td>yijiujun     <td>新建
 * </table>
 */
static void _pm_charge_irq_pair_deal(uint8_t val)
{
    switch (val) {
        case HL_MOD_PM_CHAR_STAT:
            if (PM_STAT_COMPARE(old_charge_info.charge_status, new_charge_info.charge_status) == 1) {
                
                _mod_msg_send(HL_MOD_PM_CHARGE_MSG, RT_NULL, 0);

                DBG_LOG("    msg send 0, charge status :%d, input:%d, bat_err:%d, charge_err:%d\n",
                        new_charge_info.charge_status, new_charge_info.vbus_connect_status,
                        new_charge_error_info.bat_error_status, new_charge_error_info.charge_error_status);
            }
            break;
        case HL_MOD_PM_VBUS_STAT:
            if (PM_STAT_COMPARE(old_charge_info.vbus_status, new_charge_info.vbus_status) == 1) {
                DBG_LOG("    1, vbus status : %d\n", new_charge_info.vbus_status);
            }
            break;
        case HL_MOD_PM_INPUT_STAT:
            if (PM_STAT_COMPARE(old_charge_info.vbus_connect_status, new_charge_info.vbus_connect_status) == 1) {

                _mod_msg_send(HL_MOD_PM_VBUS_MSG, RT_NULL, 0);

                DBG_LOG("    msg send 2, input power status: %d\n", new_charge_info.vbus_connect_status);
            }
            break;
        case HL_MOD_PM_VINDPM_STAT:
            if (PM_STAT_COMPARE(old_charge_info.vindpm_status, new_charge_info.vindpm_status) == 1) {
                DBG_LOG("    3\n");
            }
            break;
        case HL_MOD_PM_IINDPM_STAT:
            if (PM_STAT_COMPARE(old_charge_info.iindpm_status, new_charge_info.iindpm_status) == 1) {
                DBG_LOG("    4\n");
            }
            break;
        case HL_MOD_PM_SYS_VOL_STAT:
            if (PM_STAT_COMPARE(old_charge_info.sys_vol_status, new_charge_info.sys_vol_status) == 1) {
                DBG_LOG("    5\n");
            }
            break;
        case HL_MOD_PM_BAT_ERR_STAT:
            if (PM_STAT_COMPARE(old_charge_error_info.bat_error_status, new_charge_error_info.bat_error_status) == 1) {
                _mod_msg_send(HL_MOD_PM_BAT_FAULT_MSG, RT_NULL, 0);

                DBG_LOG("    msg send 6, battery fault status: %d\n", new_charge_error_info.bat_error_status);
            }
            break;
        case HL_MOD_PM_CHAR_ERR_STAT:
            if (PM_STAT_COMPARE(old_charge_error_info.charge_error_status, new_charge_error_info.charge_error_status) == 1) {
                _mod_msg_send(HL_MOD_CHAR_FAULT_MSG, RT_NULL, 0);

                DBG_LOG("    msg send 7, charger fault status: %d\n", new_charge_error_info.charge_error_status);
            }
            break;
        case HL_MOD_PM_BOOST_ERR_STAT:
            if (PM_STAT_COMPARE(old_charge_error_info.boost_mode_status, new_charge_error_info.boost_mode_status) == 1) {
                _mod_msg_send(HL_MOD_BOOST_FAULT_MSG, RT_NULL, 0);

                DBG_LOG("    msg send 8, BOOST fault status: %d\n", new_charge_error_info.boost_mode_status);
            }
            break;
        case HL_MOD_PM_WATCHDOG_ERR_STAT:
            if (PM_STAT_COMPARE(old_charge_error_info.watchdog_error_status, new_charge_error_info.watchdog_error_status) == 1) {
                DBG_LOG("    9\n");
            }
            break;
        default:
            break;
    }
}

/**
 * @brief 充电状态的检测，是否有充电中断产生
 * @date 2022-10-29
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-29      <td>yijiujun     <td>新建
 * </table>
 */
static void _pm_charge_status_info_check(void)
{
    uint8_t count;
    _pm_get_charge_status_info();

    for (count = 0; count < 10; count++) {
        _pm_charge_irq_pair_deal(count);
    }

    old_charge_info                   = new_charge_info;
    old_charge_error_info             = new_charge_error_info;
}

/**
 * @brief 霍尔感应检测，是否负载发生插入拔出动作
 * @date 2022-10-29
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-29      <td>yijiujun     <td>新建
 * </table>
 */
static void _pm_hall_load_info_check(void)
{
    if (hall_info.tx1_irq_flag == true) {
        hall_info.tx1_status = hl_hal_gpio_read(GPIO_HALL_TX1);
        DBG_LOG("    tx1:%d\n", hall_info.tx1_status);
        hall_info.tx1_irq_flag = false;

        _mod_msg_send(HL_MOD_PM_TX1_MSG, RT_NULL, 0);
    }

    if (hall_info.tx2_irq_flag == true) {
        hall_info.tx2_status = hl_hal_gpio_read(GPIO_HALL_TX2);
        DBG_LOG("    tx2:%d\n", hall_info.tx2_status);
        hall_info.tx2_irq_flag = false;

        _mod_msg_send(HL_MOD_PM_TX2_MSG, RT_NULL, 0);
    }

    if (hall_info.rx_irq_flag == true) {
        hall_info.rx_status = hl_hal_gpio_read(GPIO_HALL_RX);
        DBG_LOG("    rx:%d\n", hall_info.rx_status);
        hall_info.rx_irq_flag = false;

        _mod_msg_send(HL_MOD_PM_RX_MSG, RT_NULL, 0);
    }

    if (hall_info.box_irq_flag == true) {
        hall_info.box_status = hl_hal_gpio_read(GPIO_HALL_BOX);
        DBG_LOG("    box:%d\n", hall_info.box_status);
        hall_info.box_irq_flag = false;

        _mod_msg_send(HL_MOD_PM_BOX_MSG, RT_NULL, 0);
    }
}

/**
 * @brief PM线程处理启动之前，初始化PM模块数据，同时发给APP去获取初始状态
 * @return int 
 * @date 2022-11-08
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-08      <td>yijiujun     <td>新建
 * </table>
 */
static void _pm_thread_start_init()
{
    if (_pm_drv_flag.pm_init_guage_flag == false) {         //驱动初始化出现故障，上报故障类型给APP
        _mod_msg_send(HL_MOD_PM_GUAGE_ERR_MSG, RT_NULL, 0);
    } else {
        _pm_get_charge_status_info();                       //驱动初始化成功，上报电量给APP
        _mod_msg_send(HL_MOD_PM_SOC_MSG, RT_NULL, 0);
    }
    if (_pm_drv_flag.pm_init_charge_flag == false) {        //驱动初始化出现故障，上报故障类型给APP
        _mod_msg_send(HL_MOD_PM_CHARGE_ERR_MSG, RT_NULL, 0);
    } else {
        _pm_update_bat_info(HL_MOD_PM_BAT_INFO_SOC);        //驱动初始化成功，上报USB状态给APP
        _mod_msg_send(HL_MOD_PM_VBUS_MSG, RT_NULL, 0);
    }

    hall_info.tx1_status = hl_hal_gpio_read(GPIO_HALL_TX1);
    hall_info.tx2_status = hl_hal_gpio_read(GPIO_HALL_TX2);
    hall_info.rx_status  = hl_hal_gpio_read(GPIO_HALL_RX);
    hall_info.box_status = hl_hal_gpio_read(GPIO_HALL_BOX);
    _mod_msg_send(HL_MOD_PM_TX1_MSG, RT_NULL, 0);
    _mod_msg_send(HL_MOD_PM_TX2_MSG, RT_NULL, 0);
    _mod_msg_send(HL_MOD_PM_RX_MSG, RT_NULL, 0);
    _mod_msg_send(HL_MOD_PM_BOX_MSG, RT_NULL, 0);
}

static void _pm_thread_entry(void* arg)
{
    _pm_thread_start_init();

    _mod_msg_send(HL_MOD_PM_MSG_START, RT_NULL, 0);

    while (pm_mod_info.thread_exit_flag == 0) {
        _pm_update_bat_info_check();    //轮询读取

        _pm_charge_status_info_check(); //轮询读取

        _pm_hall_load_info_check();     //中断检测
        
        rt_thread_mdelay(500);
    }

    pm_mod_info.thread_exit_flag = -1;
}

static void _hl_mod_pm_lowpower_enter(void)
{
    hl_util_config_st_p config;

    hl_util_config_get(&config);
    config->lowpower_flag = 1;
    hl_util_config_save();

    __NVIC_SystemReset();  //重启
}

/* Exported functions --------------------------------------------------------*/

int hl_mod_pm_init(void* msg_hd)
{
    int ret;
    if (pm_mod_info.pm_init_flag == true) {
        DBG_LOG("pm is already init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }

    if (msg_hd != RT_NULL) {
        _pm_msg_hd.msg_id   = ((hl_mod_msg_handle_st*)(msg_hd))->msg_id;
        _pm_msg_hd.msg_send = ((hl_mod_msg_handle_st*)(msg_hd))->msg_send;
        pm_mod_info.msg_hd  = &_pm_msg_hd;
    } else {
        pm_mod_info.msg_hd = RT_NULL;
    }

    /* 电量计初始化 */
    ret = hl_drv_cw2215_init();
    if (ret == CW2215_FUNC_RET_ERR) {
        _pm_drv_flag.pm_init_guage_flag = false;
    } else {
        _pm_drv_flag.pm_init_guage_flag = true;
    }

    /* 充电驱动初始化 */
    ret = hl_drv_sgm41513_init();
    if (ret == SGM41513_ERROR) {
        _pm_drv_flag.pm_init_charge_flag = false;
    } else {
        _pm_drv_flag.pm_init_charge_flag = true;
    }

    /* 霍尔感应引脚初始化 */
    _hall_gpio_init();

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

    ret = hl_drv_sgm41513_deinit();
    if (ret == SGM41513_ERROR) {
        return HL_MOD_PM_FUNC_RET_ERR;
    }

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

    hall_info.tx1_irq_flag            = false;
    hall_info.tx2_irq_flag            = false;
    hall_info.rx_irq_flag             = false;
    hall_info.box_irq_flag            = false;

    pm_mod_info.init_bat_update_flag = false;

    _hall_gpio_irq_enable(true);

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

    _hall_gpio_irq_enable(true);

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
    uint8_t* arg_val = (uint8_t*)arg;
    if (pm_mod_info.pm_init_flag == false) {
        DBG_LOG("pm is not init!\n");
        return HL_MOD_PM_FUNC_RET_ERR;
    }
    switch (op) {
        case HL_MOD_PM_ENTER_LOWPOWER:
            _hl_mod_pm_lowpower_enter();
            break;
        case HL_MOD_PM_GET_SOC:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = bat_info.soc_val.soc;
            break;
        case HL_MOD_PM_GET_CHARGE_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = new_charge_info.charge_status;
            break;
        case HL_MOD_PM_GET_VBUS_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = new_charge_info.vbus_connect_status;
            break;
        case HL_MOD_PM_GET_TX1_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = hall_info.tx1_status;
            break;
        case HL_MOD_PM_GET_TX2_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = hall_info.tx2_status;
            break;
        case HL_MOD_PM_GET_RX_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = hall_info.rx_status;
            break;
        case HL_MOD_PM_GET_BOX_STATE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            *arg_val = hall_info.box_status;
            break;
        case HL_MOD_PM_SET_TX1_CHARGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            _hall_load_charge_enable(GPIO_TX1_POW_EN, *arg_val);
            break;
        case HL_MOD_PM_SET_TX2_CHARGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            _hall_load_charge_enable(GPIO_TX2_POW_EN, *arg_val);
            break;
        case HL_MOD_PM_SET_RX_CHARGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_PM_FUNC_RET_ERR;
            }
            _hall_load_charge_enable(GPIO_RX_POW_EN, *arg_val);
            break;
        default:
            break;
    }
    return HL_MOD_PM_FUNC_RET_OK;
}

/*
 * EOF
 */