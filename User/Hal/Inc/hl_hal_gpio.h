/*
 * Copyright (c) 2006-2021, Hollyland Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-21     luzhanghao       the first version
 */
#ifndef _HAL_INC_HL_HAL_GPIO_H_
#define _HAL_INC_HL_HAL_GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include "rtdef.h"
#include "pin.h"

/* Exported types ------------------------------------------------------------*/
typedef enum _HL_GPIO_PORT_E
{
    GPIO_HALL_TX2 = 0,
    GPIO_HALL_BOX,
    GPIO_HALL_TX1,
    GPIO_HALL_RX,
    GPIO_CH_INT_N,
    GPIO_GAUGE_INT,

    GPIO_1V8_EN,
    GPIO_BOOST_EN,
    GPIO_RX_POW_EN,
    GPIO_TX1_POW_EN,
    GPIO_TX2_POW_EN,

    USER_GPIO_NUMBER,
} HL_GPIO_PORT_E;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief 时钟初始化
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_clock_init(void);

/**
 * @brief gpio初始化
 * @param [in] gpio_index 
 * @return rt_err_t 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_init(HL_GPIO_PORT_E gpio_index);

/**
 * @brief gpio去初始化
 * @param [in] gpio_index 
 * @return rt_err_t 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_deinit(HL_GPIO_PORT_E gpio_index);

/**
 * @brief 拉高gpio
 * @param [in] gpio_index 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
void hl_hal_gpio_high(HL_GPIO_PORT_E gpio_index);

/**
 * @brief 拉低gpio
 * @param [in] gpio_index 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
void hl_hal_gpio_low(HL_GPIO_PORT_E gpio_index);

/**
 * @brief 读取gpio电平
 * @param [in] gpio_index 
 * @return int 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
int hl_hal_gpio_read(HL_GPIO_PORT_E gpio_index);

/**
 * @brief 绑定中断回调函数
 * @param [in] gpio_index 
 * @param [in] mode 
 * @param [in] hdr 
 * @param [in] args 
 * @return rt_err_t 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_attach_irq(HL_GPIO_PORT_E gpio_index, rt_uint32_t mode, void (*hdr)(void *args), void *args);

/**
 * @brief 去除绑定
 * @param [in] gpio_index 
 * @return rt_err_t 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_detach_irq(HL_GPIO_PORT_E gpio_index);

/**
 * @brief 使能中断
 * @param [in] gpio_index 
 * @param [in] enabled 
 * @return rt_err_t 
 * @date 2022-10-18
 * @author libo (rd46@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-18      <td>libo     <td>新建
 * </table>
 */
rt_err_t hl_hal_gpio_irq_enable(HL_GPIO_PORT_E gpio_index, rt_uint32_t enabled);


#endif /* _HAL_INC_HL_HAL_GPIO_H_ */
