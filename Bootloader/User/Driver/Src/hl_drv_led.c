/**
 * @file hl_drv_led.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-24
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
 * <tr><td>2022-12-24     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_led.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_drv_led_work_mode_e{
    LED_BREATH_WORK_MODE,
    LED_GPIO_WORK_MODE,
    LED_SLEEP_WORK_MODE
}hl_drv_led_work_mode_e;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

/*
static hl_drv_led_item_st led_item[LED_MAX_NUM] = {
    {GPIOA, GPIO_PIN_10, GPIO_AF2_TIM1, BREATH_DISABLE},    //LED1 PWM输出通道3
    {GPIOA, GPIO_PIN_9, GPIO_AF2_TIM1, BREATH_DISABLE},     //LED2 PWM输出通道2
    {GPIOB, GPIO_PIN_2, GPIO_AF2_LPTIM, BREATH_DISABLE},     //LED3 LPTIM_OUT
    {GPIOB, GPIO_PIN_1, GPIO_AF2_TIM3, BREATH_DISABLE},     //LED4 PWM输出通道4
    {GPIOA, GPIO_PIN_6, GPIO_AF2_TIM3, BREATH_DISABLE},     //LED5 PWM输出通道1 （红灯）

    {GPIOA, GPIO_PIN_15, GPIO_AF5_TIM2, BREATH_DISABLE},    //RX PWM输出通道1
    {GPIOB, GPIO_PIN_6, GPIO_AF2_TIM4, BREATH_DISABLE},     //TX2 PWM输出通道1
    {GPIOB, GPIO_PIN_7, GPIO_AF2_TIM4, BREATH_DISABLE},     //TX1 PWM输出通道2

};
*/

static uint8_t _drv_init_flag = 0;


/* Private function(only *.c)  -----------------------------------------------*/

static void _delay_ms(int ts)
{
    int i;
    while(ts--)
    {
        for(i = 38400; i > 0; i--);
    }
}


static void _led_work_mode(GPIO_Module *gpio, uint16_t pin, uint32_t alternate, uint8_t workmode)
{
    GPIO_InitType gpio_init_struct;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = pin;
    gpio_init_struct.GPIO_Current = GPIO_DC_4mA;
    gpio_init_struct.GPIO_Pull = GPIO_No_Pull;
    switch (workmode) {
        case LED_BREATH_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
            gpio_init_struct.GPIO_Alternate = alternate;
            break;
        case LED_GPIO_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
            break;
        case LED_SLEEP_WORK_MODE:
            gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
            break;
        default:
            break;
    }
    GPIO_InitPeripheral(gpio, &gpio_init_struct);
}

static void _led_bootlaoder_show1(uint8_t count)
{
    while(count--) {
        GPIO_WriteBit(GPIOA, GPIO_PIN_10, (Bit_OperateType)1);
        GPIO_WriteBit(GPIOA, GPIO_PIN_9, (Bit_OperateType)1);

        _delay_ms(10);

        GPIO_WriteBit(GPIOA, GPIO_PIN_10, (Bit_OperateType)0);
        GPIO_WriteBit(GPIOA, GPIO_PIN_9, (Bit_OperateType)0);

        _delay_ms(10);
    }
}

static void _led_bootlaoder_show2(uint8_t count)
{
    GPIO_WriteBit(GPIOA, GPIO_PIN_10, (Bit_OperateType)1);
    GPIO_WriteBit(GPIOA, GPIO_PIN_9, (Bit_OperateType)1);

    _delay_ms(count * 10);

    GPIO_WriteBit(GPIOA, GPIO_PIN_10, (Bit_OperateType)0);
    GPIO_WriteBit(GPIOA, GPIO_PIN_9, (Bit_OperateType)0);
}

/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_led_init()
{
    uint8_t ret;
    if (_drv_init_flag == 1) {
        return LED_FUNC_RET_ERR;
    }
    
    _led_work_mode(GPIOA, GPIO_PIN_10 | GPIO_PIN_9, GPIO_AF2_TIM1, LED_GPIO_WORK_MODE);

    _drv_init_flag = 1;

    return LED_FUNC_RET_OK;
}

uint8_t hl_drv_led_deinit()
{
    GPIO_InitType gpio_init_struct;

    if (_drv_init_flag == 0) {
        return LED_FUNC_RET_ERR;
    }
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);

    GPIO_InitStruct(&gpio_init_struct);
    GPIO_InitPeripheral(GPIOA, &gpio_init_struct);

    GPIO_DeInit(GPIOA);

    _drv_init_flag = 0;
    return LED_FUNC_RET_OK;
}


uint8_t hl_drv_led_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size)
{
    if (_drv_init_flag == 0) {
        printf("[%s][line:%d] cmd(%d) unkown!!!", __FILE__, __LINE__, 1);
        return LED_FUNC_RET_ERR;
    }
    switch (op_cmd) {
        case HL_DRV_LED_BOOT_START_MODE:
            _led_bootlaoder_show1(*((uint8_t *)arg));
            break;
        case HL_DRV_LED_BOOT_END_MODE:
            _led_bootlaoder_show2(*((uint8_t *)arg));
            break;
        default:
            printf("[%s][line:%d] cmd(%d) unkown!!!", __FILE__, __LINE__, 1);
            break;
    }
    return LED_FUNC_RET_OK;
}

/*
 * EOF
 */