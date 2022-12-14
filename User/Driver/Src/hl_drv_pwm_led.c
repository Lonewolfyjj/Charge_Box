/**
 * @file hl_drv_pwm_led.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-14
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
 * <tr><td>2022-11-14     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_pwm.h"
#include "hl_drv_pwm_led.h"
#include "n32l40x_lptim.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_drv_pwm_led_breath_state {
    BREATH_DISABLE = 0,
    BREATH_ENABLE
}hl_drv_pwm_led_breath_state_e;

typedef enum _hl_drv_pwm_led_work_mode_e{
    PWM_LED_BREATH_WORK_MODE,
    PWM_LED_GPIO_WORK_MODE,
    PWM_LED_SLEEP_WORK_MODE
}hl_drv_pwm_led_work_mode_e;

typedef struct {
    GPIO_Module *gpiox;
    uint16_t gpio_pin_x;
    uint32_t alternate;
    uint8_t breath_flag;
}hl_drv_pwm_led_item_st;

/* define --------------------------------------------------------------------*/

#define BREATH_ARR_DEFAULT_VAL      500      //定时器重装载值，该为默认值
#define BREATH_MAX_CCR_DEFAULT_VAL  200     //呼吸灯的亮度最大值，该为默认值
#define BREATH_CCR_VAL_ADD_TIME     4       //2毫秒，CCR加1或减1
#define LED_MAX_NUM                 8       //led的数量

/* variables -----------------------------------------------------------------*/

static hl_drv_pwm_led_item_st led_item[LED_MAX_NUM] = {
    {GPIOA, GPIO_PIN_10, GPIO_AF2_TIM1, BREATH_DISABLE},    //LED1 PWM输出通道3
    {GPIOA, GPIO_PIN_9, GPIO_AF2_TIM1, BREATH_DISABLE},     //LED2 PWM输出通道2
    {GPIOB, GPIO_PIN_2, GPIO_AF2_LPTIM, BREATH_DISABLE},     //LED3 LPTIM_OUT
    {GPIOB, GPIO_PIN_1, GPIO_AF2_TIM3, BREATH_DISABLE},     //LED4 PWM输出通道4
    {GPIOA, GPIO_PIN_6, GPIO_AF2_TIM3, BREATH_DISABLE},     //LED5 PWM输出通道1 （红灯）

    {GPIOA, GPIO_PIN_15, GPIO_AF5_TIM2, BREATH_DISABLE},    //RX PWM输出通道1
    {GPIOB, GPIO_PIN_6, GPIO_AF2_TIM4, BREATH_DISABLE},     //TX2 PWM输出通道1
    {GPIOB, GPIO_PIN_7, GPIO_AF2_TIM4, BREATH_DISABLE},     //TX1 PWM输出通道2

};

static hl_hal_pwm_timx_info_st tim1_info = {
    .max_ccr_val = BREATH_ARR_DEFAULT_VAL,                 //最大亮度值
    .tim_channel = HL_HAL_PWM_TIM_CHANNEL_2 | HL_HAL_PWM_TIM_CHANNEL_3,
    .timx        = TIM1
};

static hl_hal_pwm_timx_info_st tim2_info = {
    .max_ccr_val = BREATH_ARR_DEFAULT_VAL,
    .tim_channel = HL_HAL_PWM_TIM_CHANNEL_1,
    .timx        = TIM2
};
static hl_hal_pwm_timx_info_st tim3_info = {
    .max_ccr_val = BREATH_ARR_DEFAULT_VAL,
    .tim_channel = HL_HAL_PWM_TIM_CHANNEL_1 | HL_HAL_PWM_TIM_CHANNEL_4,
    .timx        = TIM3
};

static hl_hal_pwm_timx_info_st tim4_info = {
    .max_ccr_val = BREATH_ARR_DEFAULT_VAL,
    .tim_channel = HL_HAL_PWM_TIM_CHANNEL_1 | HL_HAL_PWM_TIM_CHANNEL_2,
    .timx        = TIM4
};

static uint8_t _drv_init_flag = 0;

static struct rt_timer  s_breath_timer;
static uint16_t ledpwmval = 0;    
static uint8_t toggle = 1;
static uint16_t breath_max_ccr_val = BREATH_MAX_CCR_DEFAULT_VAL;

/* Private function(only *.c)  -----------------------------------------------*/
void _hl_drv_lptim_box3_led_init();

static void _led_work_mode(GPIO_Module *gpio, uint16_t pin, uint32_t alternate, uint8_t workmode)
{
    GPIO_InitType gpio_init_struct;

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = pin;
    gpio_init_struct.GPIO_Current = GPIO_DC_4mA;
    gpio_init_struct.GPIO_Pull = GPIO_No_Pull;
    switch (workmode) {
        case PWM_LED_BREATH_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
            gpio_init_struct.GPIO_Alternate = alternate;
            break;
        case PWM_LED_GPIO_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
            break;
        case PWM_LED_SLEEP_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
            break;
        default:
            DBG_LOG("led_init mode failed!\n");
            break;
    }
    GPIO_InitPeripheral(gpio, &gpio_init_struct);
}

static void _hl_drv_pwm_led_set_work_mode(uint8_t ledx, uint8_t workmode)
{
    _led_work_mode(led_item[ledx].gpiox, led_item[ledx].gpio_pin_x, led_item[ledx].alternate, workmode);
}

static void _hl_drv_pwm_led_set_breath_val(uint8_t ledx, uint16_t ccr_val)
{   
    switch (ledx) {
        case HL_DRV_PWM_LED_BOX1:
            tim1_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_3;
            hl_hal_pwm_set_ccr_val(&tim1_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_BOX2:
            tim1_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_2;
            hl_hal_pwm_set_ccr_val(&tim1_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_BOX3:
            LPTIM_SetCompare(LPTIM, ccr_val);
            break;
        case HL_DRV_PWM_LED_BOX4:
            tim3_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_4;
            hl_hal_pwm_set_ccr_val(&tim3_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_BOX5:
            tim3_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_1;
            hl_hal_pwm_set_ccr_val(&tim3_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_RX:
            tim2_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_1;
            hl_hal_pwm_set_ccr_val(&tim2_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_TX2:
            tim4_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_1;
            hl_hal_pwm_set_ccr_val(&tim4_info, ccr_val);
            break;
        case HL_DRV_PWM_LED_TX1:
            tim4_info.tim_channel = HL_HAL_PWM_TIM_CHANNEL_2;
            hl_hal_pwm_set_ccr_val(&tim4_info, ccr_val);
            break;
        default:
            DBG_LOG("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
            break;
    }
}

static void _hl_drv_pwm_led_set_breath_state(uint8_t ledx)
{
    _hl_drv_pwm_led_set_work_mode(ledx, PWM_LED_BREATH_WORK_MODE);
    led_item[ledx].breath_flag = BREATH_ENABLE;
}

static void _hl_drv_pwm_led_set_bright(uint8_t ledx)
{
    //同时关闭该灯在定时器执行函数中运行呼吸的标志
    led_item[ledx].breath_flag = BREATH_DISABLE;

    _hl_drv_pwm_led_set_work_mode(ledx, PWM_LED_BREATH_WORK_MODE);

    _hl_drv_pwm_led_set_breath_val(ledx, breath_max_ccr_val);
}

static void _hl_drv_pwm_led_set_close(uint8_t ledx)
{
    //同时关闭该灯在定时器执行函数中运行呼吸的标志
    led_item[ledx].breath_flag = BREATH_DISABLE;

    _hl_drv_pwm_led_set_work_mode(ledx, PWM_LED_GPIO_WORK_MODE);

    GPIO_WriteBit(led_item[ledx].gpiox, led_item[ledx].gpio_pin_x, (Bit_OperateType)0);
}

static void _hl_drv_pwm_led_sleep_mode()
{
    int i;

    rt_timer_stop(&s_breath_timer);

    for (i = 0; i < LED_MAX_NUM; i++) {
        led_item[i].breath_flag = BREATH_DISABLE;
        _hl_drv_pwm_led_set_work_mode(i, PWM_LED_SLEEP_WORK_MODE);
    }

    hl_hal_pwm_enbale(&tim1_info, PWM_DISABLE);
    hl_hal_pwm_enbale(&tim2_info, PWM_DISABLE);
    hl_hal_pwm_enbale(&tim3_info, PWM_DISABLE);
    hl_hal_pwm_enbale(&tim4_info, PWM_DISABLE);
    LPTIM_Disable(LPTIM);
}

static void _hl_drv_pwm_led_active_mode()
{
    hl_hal_pwm_enbale(&tim1_info, PWM_ENABLE);
    hl_hal_pwm_enbale(&tim2_info, PWM_ENABLE);
    hl_hal_pwm_enbale(&tim3_info, PWM_ENABLE);
    hl_hal_pwm_enbale(&tim4_info, PWM_ENABLE);
    _hl_drv_lptim_box3_led_init();

    rt_timer_start(&s_breath_timer);
}

static void _breath_timeout_handle(void *arg)
{
    int i;
    if (toggle) {
        ledpwmval++;
    } else {
        ledpwmval--;
    }	            
    if (ledpwmval >= breath_max_ccr_val) {
        toggle = 0;
    }                   
    if (ledpwmval == 1) {
        toggle = 1;
    } 	
    
    for (i = 0; i < LED_MAX_NUM; i++) {
        if (led_item[i].breath_flag == BREATH_ENABLE) {
            _hl_drv_pwm_led_set_breath_val(i, ledpwmval);
        }
    }
}

static void _hl_drv_lptim_box3_led_init()
{
    /* Enable the LSI source */
    RCC_EnableLsi(ENABLE);	
    RCC_ConfigLPTIMClk(RCC_LPTIMCLK_SRC_APB1);                  //修改例程
    RCC_EnableRETPeriphClk(RCC_RET_PERIPH_LPTIM, ENABLE);
	
    /* Init output IO */
    LPTIM_SetPrescaler(LPTIM, LPTIM_PRESCALER_DIV4);            //修改例程
    /* Init lptim module */ 
    LPTIM->INTEN = 0x7F;

    LPTIM_SetWaveform(LPTIM, LPTIM_OUTPUT_WAVEFORM_PWM); 
    /* output wave */ 
    LPTIM_SetPolarity(LPTIM, LPTIM_OUTPUT_POLARITY_INVERSE);    //修改例程
    /* config the prescaler */          
    LPTIM_EnableIT_CMPOK(LPTIM);

    LPTIM_Enable(LPTIM);
    /* config ARR ande compare register */ 
    LPTIM_SetAutoReload(LPTIM, BREATH_ARR_DEFAULT_VAL);             //修改例程       
    //while(!(LPTIM->INTSTS & LPTIM_INTSTS_CMPUPD));
    LPTIM_StartCounter(LPTIM, LPTIM_OPERATING_MODE_CONTINUOUS); 

}

/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_pwm_led_init()
{
    uint8_t ret;
    if (_drv_init_flag == 1) {
        DBG_LOG("PWM LED is already inited!\n");
        return PWM_LED_FUNC_RET_ERR;
    }
    
    _hl_drv_lptim_box3_led_init();
    
    ret = hl_hal_pwm_init(&tim1_info);
    if (ret == PWM_FUNC_RET_ERR) {
        DBG_LOG("PWM LED is init failed 1!\n");
        return PWM_LED_FUNC_RET_ERR;
    }

    ret = hl_hal_pwm_init(&tim2_info);
    if (ret == PWM_FUNC_RET_ERR) {
        DBG_LOG("PWM LED is init failed 2!\n");
        return PWM_LED_FUNC_RET_ERR;
    }

    ret = hl_hal_pwm_init(&tim3_info);
    if (ret == PWM_FUNC_RET_ERR) {
        DBG_LOG("PWM LED is init failed 3!\n");
        return PWM_LED_FUNC_RET_ERR;
    }

    ret = hl_hal_pwm_init(&tim4_info);
    if (ret == PWM_FUNC_RET_ERR) {
        DBG_LOG("PWM LED is init failed 4!\n");
        return PWM_LED_FUNC_RET_ERR;
    }

    rt_timer_init(&s_breath_timer, "led_breath_timer", _breath_timeout_handle,
                  RT_NULL, BREATH_CCR_VAL_ADD_TIME, RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    rt_timer_start(&s_breath_timer);

    DBG_LOG("PWM LED init success!\n");
    _drv_init_flag = 1;

    return PWM_LED_FUNC_RET_OK;
}

uint8_t hl_drv_pwm_led_deinit()
{
    if (_drv_init_flag == 0) {
        DBG_LOG("PWM LED is already deinited!\n");
        return PWM_LED_FUNC_RET_ERR;
    }
    rt_timer_detach(&s_breath_timer);

    hl_hal_pwm_deinit(&tim1_info);
    hl_hal_pwm_deinit(&tim2_info);
    hl_hal_pwm_deinit(&tim3_info);
    hl_hal_pwm_deinit(&tim4_info);

    _drv_init_flag = 0;
    return PWM_LED_FUNC_RET_OK;
}


uint8_t hl_drv_pwm_led_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size)
{
    if (_drv_init_flag == 0) {
        DBG_LOG("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
        return PWM_LED_FUNC_RET_ERR;
    }
    switch (op_cmd) {
        case HL_DRV_PWM_SET_BREATH_MODE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return PWM_LED_FUNC_RET_ERR;
            }
            _hl_drv_pwm_led_set_breath_state(*((uint8_t *)arg));
            break;
        case HL_DRV_PWM_SET_BRIGHT_MODE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return PWM_LED_FUNC_RET_ERR;
            }
            _hl_drv_pwm_led_set_bright(*((uint8_t *)arg));
            break;
        case HL_DRV_PWM_SET_CLOSE_MODE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return PWM_LED_FUNC_RET_ERR;
            }
            _hl_drv_pwm_led_set_close(*((uint8_t *)arg));
            break;
        case HL_DRV_PWM_SET_BREATH_MAX_VAL:
            if (arg_size != sizeof(uint16_t)) {
                DBG_LOG("size err, ctrl arg need <uint16_t> type pointer!\n");
                return PWM_LED_FUNC_RET_ERR;
            }
            breath_max_ccr_val = *((uint16_t *)arg);
            break;
        case HL_DRV_PWM_LED_SLEEP_MODE:
            _hl_drv_pwm_led_sleep_mode();
            break;
        case HL_DRV_PWM_LED_ACTIVE_MODE:
            _hl_drv_pwm_led_active_mode();
            break;
        default:
            DBG_LOG("[%s][line:%d] cmd(%d) unkown!!! \r\n", __FILE__, __LINE__, 1);
            break;
    }
    return PWM_LED_FUNC_RET_OK;
}

/*
 * EOF
 */