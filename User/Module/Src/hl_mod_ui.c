/**
 * @file hl_mod_ui.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-31
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
 * <tr><td>2022-10-31     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_ui.h"
#include "hl_drv_aw2016a.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_ui_led_type {
    HL_MOD_UI_LED1 = 1,
    HL_MOD_UI_LED2,
    HL_MOD_UI_LED3,
    HL_MOD_UI_LED_1_2,
    HL_MOD_UI_LED_ALL,

    HL_MOD_UI_RED_LED,
    HL_MOD_UI_GREEN_LED,
    HL_MOD_UI_BLUE_LED,
    HL_MOD_UI_R_G_LED,
    HL_MOD_UI_RGB_ALL_LED,
}hl_mod_ui_led_type;
typedef struct _hl_mod_ui_st {
    bool                     init_flag;
    void*                    msg_hd;
} hl_mod_ui_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG     rt_kprintf

#define UI_LED_OUTPUT_CUR        1
#define UI_LED_PWM_CCR           22
#define UI_BREATH_PARAM_T0       0
#define UI_BREATH_PARAM_T1       9    
#define UI_BREATH_PARAM_T2       3
#define UI_BREATH_PARAM_T3       9
#define UI_BREATH_PARAM_T4       0
#define UI_BREATH_PARAM_REPEAT   0

/* variables -----------------------------------------------------------------*/

static hl_mod_ui_st _ui_info = {
    .init_flag  = false,
    .msg_hd     = RT_NULL
};

/* Private function(only *.c)  -----------------------------------------------*/
/**
 * @brief 设置灯为呼吸灯显示，先设置呼吸时间的6个参数，再设置为Pattern模式（自主呼吸模式）
 * @param [in] ledx 灯的类型
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static void set_ledx_breath_mod(uint8_t ledx)
{
    uint8_t led_chan, hl_drv_aw2016_ledx;
    hl_drv_aw2016a_breath_param_st pattern_param;
    if (ledx >= HL_MOD_UI_RED_LED) {
        ledx -= 5;
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED1;
    } else {
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED0;
    }
    pattern_param.repeat   = 0;
    pattern_param.t0       = UI_BREATH_PARAM_T0;
    pattern_param.t1       = UI_BREATH_PARAM_T1;
    pattern_param.t2       = UI_BREATH_PARAM_T2;
    pattern_param.t3       = UI_BREATH_PARAM_T3;
    pattern_param.t4       = UI_BREATH_PARAM_T4;

    switch (ledx) {
        case HL_MOD_UI_LED1:
            pattern_param.led_chan = HL_DRV_AW2016A_LED_CHANNEL1;
            led_chan = HL_DRV_AW2016A_LED_CHANNEL1;   
            break;
        case HL_MOD_UI_LED2:
            pattern_param.led_chan = HL_DRV_AW2016A_LED_CHANNEL2;
            led_chan = HL_DRV_AW2016A_LED_CHANNEL2;
            break;
        case HL_MOD_UI_LED3:
            pattern_param.led_chan = HL_DRV_AW2016A_LED_CHANNEL3;
            led_chan = HL_DRV_AW2016A_LED_CHANNEL3;
            break;
        case HL_MOD_UI_LED_ALL:
            pattern_param.led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
            led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
            break;
        default:
            break; 
    }
    hl_drv_aw2016a_ctrl(hl_drv_aw2016_ledx, HL_DRV_AW2016A_SET_BREATH_PARAM, &pattern_param,sizeof(pattern_param));
    hl_drv_aw2016a_ctrl(hl_drv_aw2016_ledx, HL_DRV_AW2016A_SET_PATTERN_MODE, &led_chan, sizeof(led_chan));
}

/**
 * @brief 设置灯为常亮显示，先设置为手动模式，再设置对应灯的亮度
 * @param [in] ledx 灯的类型
 * @param [in] cur_val 输出电流的值
 * @param [in] pwm_top_val PWM占空比的值
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static void set_ledx_lighting_mod(uint8_t ledx, uint8_t cur_val, uint8_t pwm_top_val)
{
    uint8_t led_chan, hl_drv_aw2016_ledx;
    hl_drv_aw2016a_light_st light;
    if (ledx >= HL_MOD_UI_RED_LED) {
        ledx -= 5;
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED1;
    } else {
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED0;
    }
    switch (ledx) {
        case HL_MOD_UI_LED1:
            led_chan = HL_DRV_AW2016A_LED_CHANNEL1;
            light.r          = cur_val;
            light.g          = 0;
            light.b          = 0;
            light.brightness = pwm_top_val;  
            break;
        case HL_MOD_UI_LED2:
            led_chan = HL_DRV_AW2016A_LED_CHANNEL2; 
            light.r          = 0;
            light.g          = cur_val;
            light.b          = 0;
            light.brightness = pwm_top_val;
            break;
        case HL_MOD_UI_LED3: 
            led_chan = HL_DRV_AW2016A_LED_CHANNEL3;
            light.r          = 0;
            light.g          = 0;
            light.b          = cur_val;
            light.brightness = pwm_top_val;
            break;
        case HL_MOD_UI_LED_1_2: 
            led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2;
            light.r          = cur_val;
            light.g          = cur_val;
            light.b          = 0;
            light.brightness = pwm_top_val;
            break;
        case HL_MOD_UI_LED_ALL: 
            led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
            light.r          = cur_val;
            light.g          = cur_val;
            light.b          = cur_val;
            light.brightness = pwm_top_val;
            break;
        default:
            break; 
    }
    hl_drv_aw2016a_ctrl(hl_drv_aw2016_ledx, HL_DRV_AW2016A_SET_MANUAL_MODE, &led_chan, sizeof(led_chan));
    hl_drv_aw2016a_ctrl(hl_drv_aw2016_ledx, HL_DRV_AW2016A_SET_LED_LIGHT_EFFECT, &light, sizeof(light));
}

static void set_led_all_close(uint8_t aw2016_ledx)
{
    uint8_t led_chan;
    hl_drv_aw2016a_light_st light;
    
    led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
    light.r          = 0;
    light.g          = 0;
    light.b          = 0;
    light.brightness = 0;
    hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_MANUAL_MODE, &led_chan, sizeof(led_chan));
    hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_LED_LIGHT_EFFECT, &light, sizeof(light));
}

/**
 * @brief 充电页面的LED显示情况，分不充电和充电两种情况
 * @param [in] soc_st 充电状态结构体
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static void _battery_soc_state_display(hl_mod_ui_charge_soc_st *soc_st)
{
    if (soc_st == RT_NULL) {
        return HL_MOD_UI_FUNC_ERR;
    }
    uint8_t ret;
    set_led_all_close(HL_DRV_AW2016A_LED1);

    DBG_LOG("set led light \n");
    if (soc_st->charge_state == HL_MOD_UI_OUT_CHARGE) {   //未充电中
        switch (soc_st->soc_level) {
            case HL_MOD_UI_SOC_LEVEL_0:
                DBG_LOG("charge soc level 0\n");
                set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                set_ledx_breath_mod(HL_MOD_UI_LED1);
                break;
            case HL_MOD_UI_SOC_LEVEL_1:
                set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                break;
            case HL_MOD_UI_SOC_LEVEL_2: 
                set_ledx_lighting_mod(HL_MOD_UI_LED_1_2, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                break;
            case HL_MOD_UI_SOC_LEVEL_3: 
                set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                break;
            default:
                break; 
        }
    } else {
        switch (soc_st->soc_level) {                    //充电中
            case HL_MOD_UI_SOC_LEVEL_0:
                set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                set_ledx_breath_mod(HL_MOD_UI_LED1);
                break;
            case HL_MOD_UI_SOC_LEVEL_1:
                set_ledx_lighting_mod(HL_MOD_UI_LED_1_2, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                set_ledx_breath_mod(HL_MOD_UI_LED2);
                break;
            case HL_MOD_UI_SOC_LEVEL_2: 
                set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
                set_ledx_breath_mod(HL_MOD_UI_LED3);
                break;
            case HL_MOD_UI_SOC_LEVEL_3: 
                set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);    //充满电
                break;
            default:
                break;            
        }
    }
    return HL_MOD_UI_FUNC_OK;
}
/**
 * @brief 升级页面LED显示状态，分升级准备，升级中，升级成功三种情况
 * @param [in] up_st 升级状态
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static void _upgrading_state_display(uint8_t *up_st)
{
    if (up_st == RT_NULL) {
        return HL_MOD_UI_FUNC_ERR;
    }
    DBG_LOG("set upd led light \n");
    set_led_all_close(HL_DRV_AW2016A_LED0);
    switch (*up_st) {
        case HL_MOD_UI_UPGRADING_READY:
            set_ledx_lighting_mod(HL_MOD_UI_RED_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_UPGRADING_RUN:
            set_ledx_lighting_mod(HL_MOD_UI_GREEN_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_UPGRADING_OK: 
            set_ledx_lighting_mod(HL_MOD_UI_BLUE_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        default:
            break;   
    }
    return HL_MOD_UI_FUNC_OK;
}

/**
 * @brief 错误故障LED显示，分充电故障，充电故障，RTC时钟故障，flash故障四种情况
 * @param [in] err_st 故障类型
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static int _error_state_display(uint8_t *err_st)
{
    if (err_st == RT_NULL) {
        return HL_MOD_UI_FUNC_ERR;
    }
    DBG_LOG("set err led light \n");
    switch (*err_st) {
        case HL_MOD_UI_GUAGE_ERR:
            set_ledx_lighting_mod(HL_MOD_UI_RED_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_RED_LED);
            break;
        case HL_MOD_UI_CHARGER_ERR:
            set_ledx_lighting_mod(HL_MOD_UI_GREEN_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_GREEN_LED);
            break;
        case HL_MOD_UI_RTC_ERR:
            set_ledx_lighting_mod(HL_MOD_UI_BLUE_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_BLUE_LED);
            break;
        case HL_MOD_UI_FLASH_ERR:
            set_led_all_close(HL_DRV_AW2016A_LED1);
            break;
        default:
            break;   
    }
    set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
    set_ledx_breath_mod(HL_MOD_UI_LED_ALL);

    return HL_MOD_UI_FUNC_OK;
}

/**
 * @brief LED的初始化，设置工作模式，最大输出电流，使能灯的状态
 * @param [in] aw2016_ledx 驱动灯的类型，分RGB灯和三个青色灯
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
static void _led_init_config(uint8_t aw2016_ledx)
{
    int                            ret;
    uint8_t                        led_chan;
    uint8_t                        work_mode;

    work_mode = HL_DRV_AW2016A_ACTIVE_MODE;
    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_WORK_MODE, &work_mode, sizeof(work_mode));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return ;
    }

    DBG_LOG("set work mode\n");
    
    work_mode = HL_DRV_AW2016A_IMAX_5MA;
    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_GLOBAL_MAX_OUTPUT_CURRENT, &work_mode,
                              sizeof(work_mode));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return ;
    }
    DBG_LOG("set g current max\n");

    led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;

    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_OPEN_LED_CHANNEL, &led_chan, sizeof(led_chan));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return ;
    }

    DBG_LOG("open led \n");

}

/* Exported functions --------------------------------------------------------*/

int hl_mod_ui_init(void *msg_hd)
{
    if (_ui_info.init_flag == true) {
        DBG_LOG("ui mod already inited!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    hl_drv_aw2016a_init();
    _led_init_config(HL_DRV_AW2016A_LED0);
    _led_init_config(HL_DRV_AW2016A_LED1);

    DBG_LOG("ui mod init success\n");

    _ui_info.msg_hd = msg_hd;
    _ui_info.init_flag = true;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_deinit(void)
{
    uint8_t val;
    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    val = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
    hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED0, HL_DRV_AW2016A_CLOSE_LED_CHANNEL, &val, sizeof(val));
    hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED1, HL_DRV_AW2016A_CLOSE_LED_CHANNEL, &val, sizeof(val));

    hl_drv_aw2016a_deinit();

    DBG_LOG("ui mod init success\n");

    _ui_info.msg_hd = RT_NULL;
    _ui_info.init_flag = false;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_ctrl(int op, void *arg, int arg_size)
{
    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    
    switch (op) {
        case HL_MOD_UI_BATTERY_PAGE:
            if (arg_size != sizeof(hl_mod_ui_charge_soc_st)) {
                DBG_LOG("size err, ctrl arg need <hl_mod_ui_charge_soc_st> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            }
            _battery_soc_state_display((hl_mod_ui_charge_soc_st *)arg);
            break;
        case HL_MOD_UI_UPGRADE_PAGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            }
            _upgrading_state_display((uint8_t *)arg); 
            break;
        case HL_MOD_UI_ERROR_PAGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            } 
            _error_state_display((uint8_t *)arg);
            break;
        default:
            DBG_LOG("op err, hl_mod_ui_ctrl!\n");
            break;
    }

    return HL_MOD_UI_FUNC_OK;

}


/*
 * EOF
 */