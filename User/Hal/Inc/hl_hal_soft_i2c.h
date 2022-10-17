/**
 * @file hl_hal_soft_i2c.h
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
#ifndef _HL_HAL_SOFT_I2C_H_
#define _HL_HAL_SOFT_I2C_H_

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stdint.h"
#include "n32l40x.h"
#include <rtthread.h>

/* typedef -------------------------------------------------------------------*/
typedef struct {
    uint8_t sda_pin_num;
    uint8_t scl_pin_num;
    uint16_t gpio_pin_sdax;
    uint16_t gpio_pin_sclx;
    uint32_t gpiox_base;
    uint32_t rcc_periph_gpiox;
    GPIO_Module *gpiox;
    void (*delay_us)(uint16_t usec);
}soft_i2c_info;

typedef struct {
    uint8_t dev_readaddr;
    uint8_t dev_writeaddr;
    uint8_t pro_destination_addr;    
}soft_i2c_dev_addr_info;

/* define --------------------------------------------------------------------*/
#define IIC_TIME                    2

#define GPIOX_ODR_OFFSET_Addr       0x14
#define GPIOX_IDR_OFFSET_Addr       0x10

#define PIN_OUT(pin_count, gpiox_base)  (*((unsigned int *)(0x42000000 + ((gpiox_base + GPIOX_ODR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))
#define PIN_IN(pin_count, gpiox_base)   (*((unsigned int *)(0x42000000 + ((gpiox_base + GPIOX_IDR_OFFSET_Addr - 0x40000000) * 8 + pin_count) * 4)))

#define IIC_SCL(pin_count, gpiox_base)     PIN_OUT(pin_count, gpiox_base)
#define IIC_SDA(pin_count, gpiox_base)     PIN_OUT(pin_count, gpiox_base)
#define READ_SDA(pin_count, gpiox_base)    PIN_IN(pin_count, gpiox_base)

#define ACK     0
#define NOACK   1
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int hl_hal_soft_i2c_init(soft_i2c_info *info);
void hl_hal_soft_i2c_deinit(soft_i2c_info *info);
int hl_hal_soft_i2c_read(soft_i2c_info *info, soft_i2c_dev_addr_info *pro_addr, uint8_t *rdata, uint16_t len);
int hl_hal_soft_i2c_write(soft_i2c_info *info, soft_i2c_dev_addr_info *pro_addr, uint8_t *wdata, uint16_t len);
int hl_hal_soft_i2c_test_dev_addr(soft_i2c_info *info);

void hl_hal_soft_i2c_test();

#endif
/*
 * EOF
 */