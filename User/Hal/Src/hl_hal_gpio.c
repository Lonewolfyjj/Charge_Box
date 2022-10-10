/*
 * Copyright (c) 2006-2021, Hollyland Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-12     luzhanghao       the first version
 */
/* Includes ------------------------------------------------------------------*/
#include "hl_hal_gpio.h"
#include "pin.h"
#include "n32l40x.h"

/* Private typedef -----------------------------------------------------------*/
// gpio config struct
typedef struct _HL_GPIO_CONFIG_T
{
    rt_base_t    pin;
    rt_base_t    mode;
    rt_uint8_t   Alternate;
}HL_GPIO_CONFIG_T;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// notice: The array element sequence must be the same as HL_GPIO PORT_E
static const HL_GPIO_CONFIG_T gpio_config_table[USER_GPIO_NUMBER] =
{
    {GET_PIN(A,10),  PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_USB_CC  
    {GET_PIN(C,4),   PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_CHG_STAT  
    {GET_PIN(B,4),   PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_VBUS_DET
    
    {GET_PIN(B,13),  PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_TX1_OCB
    {GET_PIN(C,7),   PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_TX2_OCB
    {GET_PIN(C,8),   PIN_MODE_INPUT,          GPIO_NO_AF},	// GPIO_RX_OCB
    
    {GET_PIN(A,0),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_PWR_5V_EN
	{GET_PIN(B,14),  PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_TX1_PWR_EN
	{GET_PIN(C,6),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_TX2_PWR_EN
	{GET_PIN(C,9),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_RX_PWR_EN
    
    {GET_PIN(A,1),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_PWR_1V8_EN
    {GET_PIN(A,15),  PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_SN1_EN
    {GET_PIN(B,3),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_SN2_EN
	
    {GET_PIN(C,1),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_USB_OTG
    {GET_PIN(B,0),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_LED_R
    {GET_PIN(B,1),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_LED_G
    {GET_PIN(C,3),   PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_LED_B
    {GET_PIN(B,11),  PIN_MODE_OUTPUT,          GPIO_NO_AF},	// GPIO_CHARGE_ENABLE
};

/* Private functions  --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init a gpio with a specified enum value.
 *
 * @param gpio_index is the enum value of specified gpio.
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
rt_err_t hl_hal_gpio_init(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_mode(gpio_config_table[GPIO_NUM].pin, gpio_config_table[GPIO_NUM].mode);
    return RT_EOK;
}

/**
 * @brief This function deinit a gpio with a specified enum value.
 *
 * @param gpio_index is the enum value of specified gpio.
 *
 * @return the error code, RT_EOK on deinitialization successfully.
 */
rt_err_t hl_hal_gpio_deinit(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_mode(gpio_config_table[GPIO_NUM].pin, PIN_MODE_INPUT);
    return RT_EOK;
}

/**
 * @brief This function set a gpio with a specified enum value.
 *
 * @param gpio_index is the enum value of specified gpio.
 */   
void hl_hal_gpio_high(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_write(gpio_config_table[GPIO_NUM].pin, 0);
}

/**
 * @brief This function reset a gpio with a specified enum value.
 *
 * @param gpio_index is the enum value of specified gpio.
 */
void hl_hal_gpio_low(HL_GPIO_PORT_E GPIO_NUM)
{
    rt_pin_write(gpio_config_table[GPIO_NUM].pin, 1);
}

/**
 * @brief This function read a gpio with a specified enum value.
 *
 * @param gpio_index is the enum value of specified gpio.
 *
 * @return the specified gpio pin level(0 or 1).
 */
int hl_hal_gpio_read(HL_GPIO_PORT_E GPIO_NUM)
{
    return rt_pin_read(gpio_config_table[GPIO_NUM].pin);
}

