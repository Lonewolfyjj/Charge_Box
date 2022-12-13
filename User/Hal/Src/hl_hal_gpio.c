/**
 * @file hl_hal_gpio.c
 * @author libo (rd46@hollyland-tech.com)
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
 * <tr><td>2022-10-18     <td>v1.0     <td>libo     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/*
 * EOF
 */
/* Includes ------------------------------------------------------------------*/
#include "hl_hal_gpio.h"
#include "n32l40x.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct _HL_GPIO_CONFIG_T
{
    rt_base_t pin;
    rt_base_t mode;
} HL_GPIO_CONFIG_T;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// notice: The array element sequence must be the same as HL_GPIO PORT_E
static const HL_GPIO_CONFIG_T gpio_config_table[USER_GPIO_NUMBER] = {
    { GET_PIN(A, 1), PIN_MODE_INPUT },   // GPIO_HALL_TX2
    { GET_PIN(A, 0), PIN_MODE_INPUT },   // GPIO_HALL_BOX
    { GET_PIN(B, 3), PIN_MODE_INPUT },   // GPIO_HALL_TX1
    { GET_PIN(A, 7), PIN_MODE_INPUT },   // GPIO_HALL_RX
    { GET_PIN(A, 15), PIN_MODE_INPUT },  // GPIO_CH_INT_N
    { GET_PIN(B, 2), PIN_MODE_INPUT },   // GPIO_GAUGE_INT
    { GET_PIN(A, 12), PIN_MODE_INPUT },  // GPIO_USB_DP_WAKE_UP
    { GET_PIN(B, 5), PIN_MODE_INPUT },   // GPIO_UART_RX_WAKE_UP

    { GET_PIN(A, 8), PIN_MODE_OUTPUT },   // GPIO_1V8_EN
    { GET_PIN(B, 0), PIN_MODE_OUTPUT },   // GPIO_BOOST_EN
    { GET_PIN(C, 15), PIN_MODE_OUTPUT },  // GPIO_RX_POW_EN
    { GET_PIN(C, 14), PIN_MODE_OUTPUT },  // GPIO_TX1_POW_EN
    { GET_PIN(C, 13), PIN_MODE_OUTPUT },  // GPIO_TX2_POW_EN
};

rt_err_t hl_hal_gpio_clock_init(void)
{
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
    return RT_EOK;
}

rt_err_t hl_hal_gpio_init(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_mode(gpio_config_table[GPIO_NUM].pin, gpio_config_table[GPIO_NUM].mode);
    return RT_EOK;
}

rt_err_t hl_hal_gpio_deinit(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_mode(gpio_config_table[GPIO_NUM].pin, PIN_MODE_INPUT);
    return RT_EOK;
}

void hl_hal_gpio_high(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_write(gpio_config_table[GPIO_NUM].pin, 1);
}

void hl_hal_gpio_low(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_write(gpio_config_table[GPIO_NUM].pin, 0);
}

int hl_hal_gpio_read(HL_GPIO_PORT_E GPIO_NUM)
{
    return rt_pin_read(gpio_config_table[GPIO_NUM].pin);
}

rt_err_t hl_hal_gpio_attach_irq(HL_GPIO_PORT_E gpio_index, rt_uint32_t mode, void (*hdr)(void* args), void* args)
{
    RT_ASSERT(gpio_index < USER_GPIO_NUMBER);
    return rt_pin_attach_irq(gpio_config_table[gpio_index].pin, mode, hdr, args);
}

rt_err_t hl_hal_gpio_detach_irq(HL_GPIO_PORT_E gpio_index)
{
    RT_ASSERT(gpio_index < USER_GPIO_NUMBER);
    return rt_pin_detach_irq(gpio_config_table[gpio_index].pin);
}

rt_err_t hl_hal_gpio_irq_enable(HL_GPIO_PORT_E gpio_index, rt_uint32_t enabled)
{
    RT_ASSERT(gpio_index < USER_GPIO_NUMBER);
    return rt_pin_irq_enable(gpio_config_table[gpio_index].pin, enabled);
}
