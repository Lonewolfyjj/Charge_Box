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
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_ui_soc_type_e {
    HL_MOD_UI_SOC_5_25_PERCENT,
    HL_MOD_UI_SOC_25_50_PERCENT,
    HL_MOD_UI_SOC_50_75_PERCENT,
    HL_MOD_UI_SOC_75_100_PERCENT,
    HL_MOD_UI_SOC_FULL,
    HL_MOD_UI_SOC_LOWPOWER,
    HL_MOD_UI_SOC_NOMAL,
    HL_MOD_UI_SOC_UNKNOW
}hl_mod_ui_soc_type_e;
typedef struct _hl_mod_ui_st {
    void*                   msg_hd;
    bool                    init_flag;
    bool                    start_flag;
    struct rt_thread        ui_thread_fd;
    int                     thread_exit_flag;
} hl_mod_ui_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG     rt_kprintf

#define UI_THREAD_STACK_SIZE    512

#define TIMEOUT_FALG_NULL   0       //未超时标志
#define TIMEOUT_FALG_SET    1       //超时标志

#define BAT_MIN_SOC_VAL     5       //电池的最低电量

/* variables -----------------------------------------------------------------*/

static hl_mod_ui_st _ui_info = {
    .msg_hd     = RT_NULL,
    .init_flag  = false,
    .ui_thread_fd = {0}
};

static uint8_t _ui_thread_stack[UI_THREAD_STACK_SIZE] = { 0 };

static hl_ui_mod_info_st _new_ui_state_st = {
    .box_charge_state   = HL_MOD_UI_CHARG_UNKNOW,
    .tx1_charge_state   = 0,
    .tx2_charge_state   = 0,
    .rx_charge_state    = 0,
    .soc_val            = 255,
    .fault_state        = 0,
    .upgrade_state      = 0,
    .timeout_flag       = 0
};

static hl_ui_mod_info_st _old_ui_state_st;

/* Private function(only *.c)  -----------------------------------------------*/

static void _set_all_box_led_close()
{
    uint8_t ledx;
    ledx = HL_DRV_PWM_LED_BOX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_BOX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_BOX3;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_BOX4;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_BOX5;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));

}

static void _set_all_load_led_close()
{
    uint8_t ledx;
    ledx = HL_DRV_PWM_LED_TX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_TX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
    ledx = HL_DRV_PWM_LED_RX;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));

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
static int _ui_load_state_display(uint8_t state)
{
    uint8_t ledx;
    switch (state) {
        case HL_MOD_UI_TX1_BREATH:
            ledx = HL_DRV_PWM_LED_TX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_TX1_LIGHT:
            ledx = HL_DRV_PWM_LED_TX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_TX1_DIS_LIGHT:
            ledx = HL_DRV_PWM_LED_TX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_TX2_BREATH:
            ledx = HL_DRV_PWM_LED_TX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_TX2_LIGHT:
            ledx = HL_DRV_PWM_LED_TX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_TX2_DIS_LIGHT:
            ledx = HL_DRV_PWM_LED_TX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_RX_BREATH:
            ledx = HL_DRV_PWM_LED_RX;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_RX_LIGHT:
            ledx = HL_DRV_PWM_LED_RX;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_RX_DIS_LIGHT:
            ledx = HL_DRV_PWM_LED_RX;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        default :
            break;
    }
    return HL_MOD_UI_FUNC_OK;
}
/**
 * @brief 充电页面的LED显示情况，分不充电和充电两种情况
 * @param [in] ch_state 充电状态
 * @date 2022-12-13
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-13      <td>yijiujun     <td>新建
 * </table>
 */
static int _battery_soc_state_display(uint8_t ch_state)
{
    uint8_t ledx;
    switch (ch_state) {
        case HL_MOD_UI_CH_MOD1:
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_CH_MOD2:
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_CH_MOD3: 
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_CH_MOD4: 
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_FULL_MOD: 
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_DIS_CH_MOD1:
            _set_all_box_led_close();
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_DIS_CH_MOD2: 
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_DIS_CH_MOD3: 
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_RED_BREATH: 
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case HL_MOD_UI_REG_LIGHT: 
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        default:
            break;            
    }
    return HL_MOD_UI_FUNC_OK;
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
    uint8_t ledx;
    switch (err_st) {
        case ENABLE:
            _set_all_load_led_close();                   
            ledx = HL_DRV_PWM_LED_BOX1;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX2;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX3;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX4;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &ledx, sizeof(ledx));
            ledx = HL_DRV_PWM_LED_BOX5;
            hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &ledx, sizeof(ledx));
            break;
        case DISABLE:                    //升级成功
            _set_all_load_led_close(); 
            _set_all_box_led_close(); 
            break;
        default:
            break;   
    }
    
    return HL_MOD_UI_FUNC_OK;
}

static int _ui_mod_lowpower_state(uint8_t state_val)
{
    int     ret;
    uint8_t led_chan;
    uint8_t work_mode;
    
    if (state_val == ENABLE) {
        hl_drv_pwm_led_ctrl(HL_DRV_PWM_LED_SLEEP_MODE, RT_NULL, 1);
        DBG_LOG("ui enter lowpower\n");
    } else {
        
        hl_drv_pwm_led_ctrl(HL_DRV_PWM_LED_ACTIVE_MODE, RT_NULL, 1);
        DBG_LOG("ui mod wake up\n");
    }
    return HL_MOD_UI_FUNC_OK;
}

/**
 * @brief BOX外面五颗LED, 充电状态下的显示
 * @date 2022-12-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-14      <td>yijiujun     <td>新建
 * </table>
 */
static void _ui_box_led_charge_show()
{
    uint8_t soc_val = _new_ui_state_st.soc_val / 25;
    if (_old_ui_state_st.soc_val != _new_ui_state_st.soc_val) {
        switch (soc_val) {
            case HL_MOD_UI_SOC_5_25_PERCENT:                    //0 < soc < 25
                _battery_soc_state_display(HL_MOD_UI_CH_MOD1);
                break;
            case HL_MOD_UI_SOC_25_50_PERCENT:                   //25 <= soc < 50
                _battery_soc_state_display(HL_MOD_UI_CH_MOD2);
                break;
            case HL_MOD_UI_SOC_50_75_PERCENT:                   //50 <= soc < 75
                _battery_soc_state_display(HL_MOD_UI_CH_MOD3);
                break;
            case HL_MOD_UI_SOC_75_100_PERCENT:                  //50 <= soc < 75
                _battery_soc_state_display(HL_MOD_UI_CH_MOD4);
                break;
            case HL_MOD_UI_SOC_FULL:                            //soc = 100
                _battery_soc_state_display(HL_MOD_UI_FULL_MOD);
                break;
            default:
                break;  
        }
        _old_ui_state_st.soc_val = _new_ui_state_st.soc_val;
    }
}

/**
 * @brief BOX外面五颗LED, 未在充电状态下的显示
 * @date 2022-12-14
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-14      <td>yijiujun     <td>新建
 * </table>
 */
static void _ui_box_led_no_charge_show()
{
    uint8_t soc_val = _new_ui_state_st.soc_val / 25;
    if (_old_ui_state_st.soc_val != _new_ui_state_st.soc_val) {
        switch (soc_val) {
            case HL_MOD_UI_SOC_5_25_PERCENT:                    //0 < soc < 25
                if (_new_ui_state_st.soc_val <= BAT_MIN_SOC_VAL) {
                    _battery_soc_state_display(HL_MOD_UI_RED_BREATH);
                } else {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD1);
                }
                break;
            case HL_MOD_UI_SOC_25_50_PERCENT:                   //25 <= soc < 50
                if (_new_ui_state_st.soc_val == 25) {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD1);
                } else {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD2);
                }
                break;
            case HL_MOD_UI_SOC_50_75_PERCENT:                   //50 <= soc < 75
                if (_new_ui_state_st.soc_val == 50) {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD2);
                } else {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD3);
                }
                break;
            case HL_MOD_UI_SOC_75_100_PERCENT:                  //75 <= soc < 100
                if (_new_ui_state_st.soc_val == 75) {
                    _battery_soc_state_display(HL_MOD_UI_DIS_CH_MOD3);
                } else {
                    _battery_soc_state_display(HL_MOD_UI_FULL_MOD);
                }
                break;
            case HL_MOD_UI_SOC_FULL:                            //soc = 100
                _battery_soc_state_display(HL_MOD_UI_FULL_MOD);
                break;
            default:
                break;
        }
        _old_ui_state_st.soc_val = _new_ui_state_st.soc_val;
    }

}

static void _ui_box_led_show()
{
    if (_old_ui_state_st.box_charge_state != _new_ui_state_st.box_charge_state) {
        _old_ui_state_st.soc_val = 254;
        switch (_new_ui_state_st.box_charge_state) {
            case HL_MOD_UI_NO_CHARGING:
                _ui_box_led_no_charge_show();
                break;
            case HL_MOD_UI_CHARGING:
                _ui_box_led_charge_show();
                break;
            case HL_MOD_UI_CHARG_FULL:
                _battery_soc_state_display(HL_MOD_UI_FULL_MOD);
                break;
            default:
                break;  
        }
    }
}

static void _ui_tx1_led_show()
{
    if (_old_ui_state_st.tx1_charge_state != _new_ui_state_st.tx1_charge_state) {
        switch (_new_ui_state_st.tx1_charge_state) {
            case HL_MOD_UI_NO_CHARGING:
                _ui_load_state_display(HL_MOD_UI_TX1_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARG_FULL:
                _ui_load_state_display(HL_MOD_UI_TX1_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARGING:
                _ui_load_state_display(HL_MOD_UI_TX1_LIGHT);
                break;
            default:
                break;  
        }
        _old_ui_state_st.tx1_charge_state = _new_ui_state_st.tx1_charge_state;
    }
    
}

static void _ui_tx2_led_show()
{
    if (_old_ui_state_st.tx2_charge_state != _new_ui_state_st.tx2_charge_state) {
        switch (_new_ui_state_st.tx2_charge_state) {
            case HL_MOD_UI_NO_CHARGING:
                _ui_load_state_display(HL_MOD_UI_TX2_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARG_FULL:
                _ui_load_state_display(HL_MOD_UI_TX2_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARGING:
                _ui_load_state_display(HL_MOD_UI_TX2_LIGHT);
                break;
            default:
                break;  
        }
        _old_ui_state_st.tx2_charge_state = _new_ui_state_st.tx2_charge_state;
    }
    
}

static void _ui_rx_led_show()
{
    if (_old_ui_state_st.rx_charge_state != _new_ui_state_st.rx_charge_state) {
        switch (_new_ui_state_st.rx_charge_state) {
            case HL_MOD_UI_NO_CHARGING:
                _ui_load_state_display(HL_MOD_UI_RX_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARG_FULL:
                _ui_load_state_display(HL_MOD_UI_RX_DIS_LIGHT);
                break;
            case HL_MOD_UI_CHARGING:
                _ui_load_state_display(HL_MOD_UI_RX_LIGHT);
                break;
            default:
                break;  
        }
        _old_ui_state_st.rx_charge_state = _new_ui_state_st.rx_charge_state;
    }
}

static void _ui_mod_state_check()
{
    if (_new_ui_state_st.fault_state == HL_MOD_UI_NO_FAULT) {       //无故障显示

        _ui_tx1_led_show();
        _ui_tx2_led_show();
        _ui_rx_led_show();

        if (_new_ui_state_st.timeout_flag == TIMEOUT_FALG_NULL) {   //无超时显示
            _ui_box_led_show();
        } else {                                                    //超时显示
            rt_memset(&_old_ui_state_st, 0, sizeof(_old_ui_state_st));
            _old_ui_state_st.soc_val = 255;                                             
            _set_all_box_led_close();
        }  
            
    } else {                                                      //故障显示
        rt_memset(&_old_ui_state_st, 0, sizeof(_old_ui_state_st));
        _old_ui_state_st.soc_val = 255;
        _error_state_display(ENABLE);
    }
}


static void _ui_thread_entry(void* arg)
{
    while (_ui_info.thread_exit_flag == 0) {
   
        if (_new_ui_state_st.upgrade_state == HL_MOD_UI_UPGRADING) {        //升级中
            rt_memset(&_old_ui_state_st, 0, sizeof(_old_ui_state_st));
            _old_ui_state_st.soc_val = 255;
            _set_all_box_led_close();
            _set_all_load_led_close();
        } else {                                                        //不在升级中/升级成功恢复显示

            _ui_mod_state_check();
        }
        
        rt_thread_mdelay(200);
    }

    _ui_info.thread_exit_flag = -1;
}

/* Exported functions --------------------------------------------------------*/

int hl_mod_ui_init(void *msg_hd)
{
    int ret;
    if (_ui_info.init_flag == true) {
        DBG_LOG("ui mod already inited!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    ret = hl_drv_pwm_led_init();
    if (PWM_LED_FUNC_RET_ERR == ret) {
        DBG_LOG("[erro] ui mod init failed!\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    _set_all_box_led_close();
    _set_all_load_led_close();

    DBG_LOG("ui mod init success\n");

    _old_ui_state_st = _new_ui_state_st;
    _ui_info.msg_hd = msg_hd;
    _ui_info.init_flag = true;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_start(void)
{
    rt_err_t rt_err;

    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    if (_ui_info.start_flag == true) {
        DBG_LOG("ui mod already start!\n");
        return HL_MOD_UI_FUNC_OK;
    }

    _ui_info.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_ui_info.ui_thread_fd), "ui_thread", _ui_thread_entry, RT_NULL, _ui_thread_stack,
                            sizeof(_ui_thread_stack), 6, 10);
    if (rt_err == RT_ERROR) {
        DBG_LOG("ui thread create failed\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    rt_thread_startup(&(_ui_info.ui_thread_fd));

    DBG_LOG("ui mod start success!\n");

    _ui_info.start_flag = true;

    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_stop(void)
{
    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    if (_ui_info.start_flag == false) {
        DBG_LOG("ui mod not start!\n");
        return HL_MOD_UI_FUNC_OK;
    }

    _ui_info.thread_exit_flag = 1;

    DBG_LOG("wait ui thread exit\n");

    while (_ui_info.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    DBG_LOG("ui mod stop success!\n");

    _ui_info.start_flag = false;

    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_deinit(void)
{
    uint8_t val;
    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    hl_mod_ui_stop();

    rt_thread_detach(&(_ui_info.ui_thread_fd));

    hl_drv_pwm_led_deinit();

    DBG_LOG("ui mod init success\n");

    _ui_info.msg_hd = RT_NULL;
    _ui_info.init_flag = false;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_ctrl(int op, void *arg, int arg_size)
{
    uint8_t *state_val = (uint8_t *)arg;
    if (_ui_info.init_flag == false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    
    switch (op) {
        case HL_MOD_UI_SET_BOX_CHARGE_STATE:
            _new_ui_state_st.box_charge_state = *state_val;
            break;
        case HL_MOD_UI_SET_TX1_CHARGE_STATE:
            _new_ui_state_st.tx1_charge_state = *state_val;
            break;
        case HL_MOD_UI_SET_TX2_CHARGE_STATE:
            _new_ui_state_st.tx2_charge_state = *state_val;
            break;
        case HL_MOD_UI_SET_RX_CHARGE_STATE:
            _new_ui_state_st.rx_charge_state = *state_val;
            break;
        case HL_MOD_UI_SET_BAT_SOC_VAL:
            _new_ui_state_st.soc_val = *state_val;
            break;
        case HL_MOD_UI_SET_FAULT_STATE:
            _new_ui_state_st.fault_state = *state_val;
            break;
        case HL_MOD_UI_SET_UPGRADE_STATE:
            _new_ui_state_st.upgrade_state = *state_val;
            break;
        case HL_MOD_UI_SET_TIMEOUT_FLAG:
            _new_ui_state_st.timeout_flag = TIMEOUT_FALG_SET;
            break;
        case HL_MOD_UI_CLEAR_TIMEOUT_FLAG:
            _new_ui_state_st.timeout_flag = TIMEOUT_FALG_NULL;
            break;
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
        case HL_MOD_UI_RED_BREATH:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_REG_LIGHT:
            _battery_soc_state_display(op);
            break;
        case HL_MOD_UI_ERROR_MOD:
            _error_state_display(ENABLE);
            break;
        case HL_MOD_UI_CLOSE_MOD:
            _error_state_display(DISABLE);
            break;
        case HL_MOD_UI_TX1_BREATH:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_TX2_BREATH:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_RX_BREATH:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_TX1_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_TX2_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_RX_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_TX1_DIS_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_TX2_DIS_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_RX_DIS_LIGHT:
            _ui_load_state_display(op);
            break;
        case HL_MOD_UI_ALL_CLOSE_MOD:
            _set_all_box_led_close();
            _set_all_load_led_close();
            break;
        case HL_MOD_UI_LOWPOWER_MOD:
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