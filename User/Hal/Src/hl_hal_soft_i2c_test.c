/**
 * @file hl_hal_soft_i2c_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-17
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
 * <tr><td>2022-10-17     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "hl_hal_soft_i2c.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
static void hl_delay_timer_tim_init()
{
    TIM_TimeBaseInitType tim_time_base_struct;
    uint16_t prescaler_value = 24 - 1;

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
    RCC_ConfigPclk1(RCC_HCLK_DIV4);
    tim_time_base_struct.Period    = 65535;
    tim_time_base_struct.Prescaler = 0;
    tim_time_base_struct.ClkDiv    = 0;
    tim_time_base_struct.CntMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(TIM2, &tim_time_base_struct);
    TIM_ConfigPrescaler(TIM2, prescaler_value, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    TIM_ConfigInt(TIM2, TIM_INT_UPDATE, ENABLE);
    TIM_Enable(TIM2, ENABLE);
}
  
static void hl_delay_timer_delay_us(uint16_t usec)
{
    uint16_t cnt_set = TIM_GetCnt(TIM2);
    uint16_t cnt_now = 0;
    while(1){
        cnt_now = TIM_GetCnt(TIM2);
        cnt_now = (65536 + (cnt_now - cnt_set)) % 65536;
        if (cnt_now >= usec) {
            break;
        }
    }
}

 void hl_hal_soft_i2c_test()
{   
    soft_i2c_info i2c_str1 = {
        2,              //sda_pin_num
        3,              //scl_pin_num
        GPIO_PIN_2,     //gpio_pin_sda
        GPIO_PIN_3,     //gpio_pin_scl
        GPIOA_BASE,     //gpiox_base
        GPIOA,          //gpiox
        hl_delay_timer_delay_us //微秒级延时函数名
    };
    soft_i2c_dev_addr_info i2c_addr1 = {
        0xc9,           //dev_readaddr
        0xc8,           //dev_writeaddr                                                             
        0x00            //pro_destination_addr
    };
    uint8_t data = 0;
    /* 延时定时器初始化并启动 */
    hl_delay_timer_tim_init();
    
    /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟，只能在外面使能 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    hl_hal_soft_i2c_init(&i2c_str1);

    /* 检测I2C引脚对应连接的从机设备地址，并打印出来 */
    hl_hal_soft_i2c_test_dev_addr(&i2c_str1);

    /* 读取从机设备的ID, 寄存器ID地址为0x00 */
    i2c_addr1.pro_destination_addr = 0x00;
    hl_hal_soft_i2c_read(&i2c_str1, &i2c_addr1, &data, 1);
    rt_kprintf("read AW2016 ID :0x%x\n", data);

    /* 往从机设备寄存器0x01写入数据，再将它读出来*/
    data = 0x01;
    i2c_addr1.pro_destination_addr = 0x01;
    hl_hal_soft_i2c_write(&i2c_str1, &i2c_addr1, &data, 1);
    data = 0;
    hl_hal_soft_i2c_read(&i2c_str1, &i2c_addr1, &data, 1);
    rt_kprintf("read AW2016 reg_0x01 :0x%x\n", data);
}
/* Exported functions --------------------------------------------------------*/

MSH_CMD_EXPORT(hl_hal_soft_i2c_test, soft I2C test);
/*
 * EOF
 */

