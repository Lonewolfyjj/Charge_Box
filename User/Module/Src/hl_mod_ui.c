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
#include "n32l40x.h"

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

typedef enum _hl_mod_ui_led_ctrl_e {
    UI_LED_CLOSE,
    UI_LED_OPEN,
    UI_LED_BREATH
}hl_mod_ui_led_ctrl_e;
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

static struct rt_timer  s_upgrade_syn_timer;

static bool timer_run_flag;

static hl_drv_aw2016a_breath_param_st pattern_param = {
   .repeat   = UI_BREATH_PARAM_REPEAT,
    .t0       = UI_BREATH_PARAM_T0,
    .t1       = UI_BREATH_PARAM_T1,
    .t2       = UI_BREATH_PARAM_T2,
    .t3       = UI_BREATH_PARAM_T3,
    .t4       = UI_BREATH_PARAM_T4,
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
    
    if (ledx >= HL_MOD_UI_RED_LED) {
        ledx -= 5;
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED1;
    } else {
        hl_drv_aw2016_ledx = HL_DRV_AW2016A_LED0;
    }

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
 * @brief 负载UI显示
 * @param [in] state 
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
static int _load_state_display(uint8_t state)
{
    if (state < HL_MOD_UI_TX1_LIGHT && state > HL_MOD_UI_RX_DIS_LIGHT) {
        return HL_MOD_UI_FUNC_ERR;
    }
    switch (state) {
        case HL_MOD_UI_TX1_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_TX1_DIS_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED1, 0, 0);
            break;
        case HL_MOD_UI_TX2_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED2, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_TX2_DIS_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED2, 0, 0);
            break;
        case HL_MOD_UI_RX_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED3, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_RX_DIS_LIGHT:
            set_ledx_lighting_mod(HL_MOD_UI_LED3, 0, 0);
            break;
        default :
            break;
    }
    return HL_MOD_UI_FUNC_OK;
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
static int _battery_soc_state_display(uint8_t soc_st)
{
    if (soc_st < HL_MOD_UI_CH_MOD1 && soc_st > HL_MOD_UI_DIS_CH_MOD3) {
        return HL_MOD_UI_FUNC_ERR;
    }
    DBG_LOG("set led light \n");

    switch (soc_st) {
        case HL_MOD_UI_CH_MOD1:
            set_led_all_close(HL_DRV_AW2016A_LED0);
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_RGB_ALL_LED);
            break;
        case HL_MOD_UI_CH_MOD2:
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_LED1);
            break;
        case HL_MOD_UI_CH_MOD3: 
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED_1_2, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_LED2);
            break;
        case HL_MOD_UI_CH_MOD4: 
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_LED3);
            break;
        case HL_MOD_UI_FULL_MOD: 
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_DIS_CH_MOD1:
            set_led_all_close(HL_DRV_AW2016A_LED0);
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_DIS_CH_MOD2: 
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED1, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        case HL_MOD_UI_DIS_CH_MOD3: 
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_lighting_mod(HL_MOD_UI_LED_1_2, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            break;
        default:
            break;            
    }
    return HL_MOD_UI_FUNC_OK;
}



static void _error_syn_timer_start()
{
    if (timer_run_flag == false) {
        rt_timer_start(&s_upgrade_syn_timer);
        timer_run_flag = true;
    }
}
static void _error_syn_timer_stop()
{
    timer_run_flag = false;
    rt_timer_stop(&s_upgrade_syn_timer);
}

/**
 * @brief 错误故障LED显示
 * @param [in] err_st 
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
static int _error_state_display(uint8_t err_st)
{
    switch (err_st) {
        case ENABLE:                   //gu
            _error_syn_timer_start();
            set_ledx_lighting_mod(HL_MOD_UI_RGB_ALL_LED, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_RGB_ALL_LED);
            set_ledx_lighting_mod(HL_MOD_UI_LED_ALL, UI_LED_OUTPUT_CUR, UI_LED_PWM_CCR);
            set_ledx_breath_mod(HL_MOD_UI_LED_ALL);
            break;
        case DISABLE:                    //升级成功
            _error_syn_timer_stop();
            set_led_all_close(HL_DRV_AW2016A_LED0);
            set_led_all_close(HL_DRV_AW2016A_LED1);
            break;
        default:
            break;   
    }
    
    return HL_MOD_UI_FUNC_OK;
}

static void _error_syn_timeout_handle(void *arg)
{
    uint8_t val = ENABLE;
    _error_state_display(val);
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
static int _led_init_config(uint8_t aw2016_ledx)
{
    int     ret;
    uint8_t led_chan;
    uint8_t work_mode;

    work_mode = HL_DRV_AW2016A_ACTIVE_MODE;
    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_WORK_MODE, &work_mode, sizeof(work_mode));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return HL_MOD_UI_FUNC_ERR;
    }

    DBG_LOG("set work mode\n");
    
    work_mode = HL_DRV_AW2016A_IMAX_5MA;
    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_SET_GLOBAL_MAX_OUTPUT_CURRENT, &work_mode,
                              sizeof(work_mode));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return HL_MOD_UI_FUNC_ERR;
    }
    DBG_LOG("set g current max\n");

    led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;

    ret = hl_drv_aw2016a_ctrl(aw2016_ledx, HL_DRV_AW2016A_OPEN_LED_CHANNEL, &led_chan, sizeof(led_chan));
    if (ret == AW2016A_FUNC_RET_ERR) {
        return HL_MOD_UI_FUNC_ERR;
    }

    DBG_LOG("open led \n");
    return HL_MOD_UI_FUNC_OK;
}

static int _ui_mod_lowpower_state(uint8_t state_val)
{
    int     ret;
    uint8_t led_chan;
    uint8_t work_mode;
    
    if (state_val == ENABLE) {

        _error_syn_timer_stop();                    //关闭错误UI同步定时器

        set_led_all_close(HL_DRV_AW2016A_LED0);     //清除之前的配置
        set_led_all_close(HL_DRV_AW2016A_LED1);
                                                    // LED模块停止工作
        led_chan = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
        ret = hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED0, HL_DRV_AW2016A_CLOSE_LED_CHANNEL, &led_chan, sizeof(led_chan));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return HL_MOD_UI_FUNC_ERR;
        }
        ret = hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED1, HL_DRV_AW2016A_CLOSE_LED_CHANNEL, &led_chan, sizeof(led_chan));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return HL_MOD_UI_FUNC_ERR;
        }

        work_mode = HL_DRV_AW2016A_STANDBY_MODE;    //LED驱动进入待机状态
        ret = hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED0, HL_DRV_AW2016A_SET_WORK_MODE, &work_mode, sizeof(work_mode));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return HL_MOD_UI_FUNC_ERR;
        }
        ret = hl_drv_aw2016a_ctrl(HL_DRV_AW2016A_LED1, HL_DRV_AW2016A_SET_WORK_MODE, &work_mode, sizeof(work_mode));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return HL_MOD_UI_FUNC_ERR;
        }

        DBG_LOG("ui enter lowpower\n");
    } else {
        _led_init_config(HL_DRV_AW2016A_LED0);      //LED驱动进入活跃状态，LED输出启用
        _led_init_config(HL_DRV_AW2016A_LED1);

        DBG_LOG("ui mod wake up\n");
    }
    return HL_MOD_UI_FUNC_OK;
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

    rt_timer_init(&s_upgrade_syn_timer, "error_syn_timer", _error_syn_timeout_handle,
                  RT_NULL, 5000, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    timer_run_flag = false;
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
    rt_timer_detach(&s_upgrade_syn_timer);

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
        case HL_MOD_UI_CH_MOD1:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_CH_MOD2:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_CH_MOD3:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_CH_MOD4:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_FULL_MOD:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_DIS_CH_MOD1:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_DIS_CH_MOD2:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_DIS_CH_MOD3:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_ERROR_MOD:
            _error_state_display(ENABLE);
            break;
        case HL_MOD_UI_CLOSE_MOD:
            _error_state_display(DISABLE);
            break;
        case HL_MOD_UI_SIGN_LIGHT:

            break;
        case HL_MOD_UI_SIGN_DIS_LIGHT:

            break;
        case HL_MOD_UI_TX1_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_TX2_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_RX_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_TX1_DIS_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_TX2_DIS_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_RX_DIS_LIGHT:
            _load_state_display(op);
            break;
        case HL_MOD_UI_ALL_CLOSE_MOD:

            break;
        case HL_MOD_UI_LOWPOWER_MOD:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            } 
            _ui_mod_lowpower_state(ENABLE);
            break;
        case HL_MOD_UI_WAKE_UP_MOD:
            _ui_mod_lowpower_state(DISABLE);
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