/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2017-10-20      ZYH          add mode open drain and input pull down
 */

#ifndef PIN_H__
#define PIN_H__

#include <rtthread.h>

#define __N32L40X_PORT(port)  GPIO##port##_BASE

#define GET_PIN(PORTx,PIN) (rt_base_t)((16 * ( ((rt_base_t)__N32L40X_PORT(PORTx) - (rt_base_t)GPIOA_BASE)/(0x0400UL) )) + PIN)
   
#ifdef __cplusplus
extern "C" {
#endif

/* pin device and operations for RT-Thread */
/*
struct rt_device_pin
{
    struct rt_device parent;
    const struct rt_pin_ops *ops;
};
*/

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04
#define PIN_MODE_AF             0x05    // add by lzh 2022/07/12

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1

struct rt_device_pin_mode
{
    rt_uint16_t pin;
    rt_uint16_t mode;
};
struct rt_device_pin_status
{
    rt_uint16_t pin;
    rt_uint16_t status;
};
struct rt_pin_irq_hdr
{
    rt_int16_t        pin;
    rt_uint16_t       mode;
    void (*hdr)(void *args);
    void             *args;
};

void rt_pin_mode(rt_base_t pin, rt_base_t mode);
void rt_pin_write(rt_base_t pin, rt_base_t value);
int  rt_pin_read(rt_base_t pin);
rt_err_t rt_pin_attach_irq(rt_int32_t pin, rt_uint32_t mode,
                             void (*hdr)(void *args), void  *args);
rt_err_t rt_pin_detach_irq(rt_int32_t pin);
rt_err_t rt_pin_irq_enable(rt_base_t pin, rt_uint32_t enabled);
/* Get pin number by name,such as PA0,PB12 */
rt_base_t rt_pin_get(const char *name);

#ifdef __cplusplus
}
#endif

#endif
