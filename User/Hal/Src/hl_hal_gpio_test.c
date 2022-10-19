/**
 * @file hl_hal_gpio_test.c
 * @author luzhanghao (rd14@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-09-16
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
 * <tr><td>2022-09-16     <td>v1.0     <td>luzhanghao     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
#if 1
/* Includes ------------------------------------------------------------------*/
#include <string.h>
//#include <rtdevice.h>
#include "hl_hal_gpio.h"
#include "finsh_api.h"
#include "rtthread.h"
#include <stdlib.h>
#include "rtdef.h"
#include "pin.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
 HL_GPIO_PORT_E irq_arg_table[USER_GPIO_NUMBER];  // 中断参数列表，存放中断枚举编号

/* Private function(only *.c)  -----------------------------------------------*/
void hl_hal_gpio_test_irq_process(void *args)
{
    HL_GPIO_PORT_E gpio_pin_e = *(HL_GPIO_PORT_E *)args;

    rt_kprintf("irq enum = %d\r\n", gpio_pin_e);
}


/* Exported functions --------------------------------------------------------*/
// hl_hal_gpio_test [enum] [reset | set | read | irq] 
int hl_hal_gpio_test(int argc, char** argv)
{
    HL_GPIO_PORT_E gpio_pin_e = GPIO_HALL_TX2;
    int           gpio_level;

    if (argc == 3) {
        gpio_pin_e = (HL_GPIO_PORT_E) atoi(argv[1]);
        if (gpio_pin_e >= USER_GPIO_NUMBER) {
            rt_kprintf("wrong parameter emum, hl_hal_gpio_test [enum] [reset | set | read | irq]  enum must less than USER_GPIO_COUNT(%d) \r\n", USER_GPIO_NUMBER);
            return -1;
        }

        hl_hal_gpio_clock_init();

        if (!strcmp("reset", argv[2])) {                     // 初始化gpio，并输出低电平
            hl_hal_gpio_init(gpio_pin_e);
            hl_hal_gpio_low(gpio_pin_e);
            rt_kprintf("gpio reset completed\r\n");
        } else if (!strcmp("set", argv[2])) {                // 初始化gpio，并输出高电平
            hl_hal_gpio_init(gpio_pin_e);
            hl_hal_gpio_high(gpio_pin_e);
            rt_kprintf("gpio set completed\r\n");
        } else if (!strcmp("read", argv[2])) {               // 初始化gpio，并读取其电平
            hl_hal_gpio_init(gpio_pin_e);
            gpio_level = hl_hal_gpio_read(gpio_pin_e);
            rt_kprintf("gpio read completed, gpio level = %d \r\n", gpio_level);
        } else if (!strcmp("irq", argv[2])) {                // 初始化gpio，并打开中断，绑定中断函数
            irq_arg_table[gpio_pin_e] = gpio_pin_e;
            hl_hal_gpio_init(gpio_pin_e);
            hl_hal_gpio_attach_irq(gpio_pin_e, PIN_IRQ_MODE_FALLING, hl_hal_gpio_test_irq_process, &irq_arg_table[gpio_pin_e]);
            hl_hal_gpio_irq_enable(gpio_pin_e, PIN_IRQ_ENABLE);
            rt_kprintf("gpio set irq completed\r\n");
        } else {
            rt_kprintf("wrong parameter subcommand, hl_hal_gpio_test [enum] [reset | set | read | irq],   subcommand must equal to [reset | set | read | irq] \r\n");
        }
    } else {
        rt_kprintf("wrong parameter, please type: hl_hal_gpio_test [enum] [reset | set | read | irq] \r\n");
        return -1;
    }

    return 0;
}

MSH_CMD_EXPORT(hl_hal_gpio_test, gpio test cmd);

int hl_hal_gpio_pwr_on(void)
{
    hl_hal_gpio_clock_init();

    hl_hal_gpio_init(GPIO_1V8_EN);
    hl_hal_gpio_init(GPIO_BOOST_EN);
    hl_hal_gpio_init(GPIO_RX_POW_EN);
    hl_hal_gpio_init(GPIO_TX1_POW_EN);
    hl_hal_gpio_init(GPIO_TX2_POW_EN);

    hl_hal_gpio_high(GPIO_1V8_EN);
    hl_hal_gpio_low(GPIO_BOOST_EN);
    hl_hal_gpio_high(GPIO_RX_POW_EN);
    hl_hal_gpio_high(GPIO_TX1_POW_EN);
    hl_hal_gpio_high(GPIO_TX2_POW_EN);
    rt_kprintf("A6902 box hl_hal_gpio_pwr_on enable power!\n");

    rt_thread_mdelay(10);
    return 0;
}

INIT_PREV_EXPORT(hl_hal_gpio_pwr_on);

#endif