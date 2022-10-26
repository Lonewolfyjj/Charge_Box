/**
 * @file hl_hal_soft_i2c_api_test.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
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
 * <tr><td>2022-10-18     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_soft_i2c_api.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void hl_hal_soft_i2c_api_test()
{
    char reg_val;

    hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_1);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_1, 0x64, 0x00, &reg_val, 1);

    rt_kprintf("i2c1, ic_addr:%02x, chip_id:%02x\n", 0x64, reg_val, 1);

    hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_2);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_2, 0x64, 0x00, &reg_val, 1);

    rt_kprintf("i2c2, ic_addr:%02x, chip_id:%02x\n", 0x64, reg_val, 1);

    hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_3);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_3, 0x64, 0x00, &reg_val, 1);

    rt_kprintf("i2c3, ic_addr:%02x, chip_id:%02x\n", 0x64, reg_val, 1);

    reg_val = 0x66;

    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_1, 0x64, 0xA2, &reg_val, 1);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_1, 0x64, 0xA2, &reg_val, 1);

    rt_kprintf("i2c1, ic_addr:%02x, write:0x66, read:%02x\n", 0x64, reg_val, 1);

    reg_val = 0x01;

    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_2, 0x64, 0x01, &reg_val, 1);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_2, 0x64, 0x01, &reg_val, 1);

    rt_kprintf("i2c2, ic_addr:%02x, write:0x01, read:%02x\n", 0x64, reg_val, 1);

    reg_val = 0x66;
    
    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_3, 0x64, 0x34, &reg_val, 1);
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_3, 0x64, 0x34, &reg_val, 1);

    rt_kprintf("i2c3, ic_addr:%02x, write:0x66, read:%02x\n", 0x64, reg_val, 1);
}

MSH_CMD_EXPORT(hl_hal_soft_i2c_api_test, soft I2C api test);
/*
 * EOF
 */