/**
 * @file hl_hal_soft_i2c_api.h
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

#ifndef __HL_HAL_SOFT_I2C_API_H__
#define __HL_HAL_SOFT_I2C_API_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_hal_soft_i2c_numb_e
{
    HL_HAL_SOFT_I2C_NUMB_1,         //i2c1，PB8，PB9，新板不用
    HL_HAL_SOFT_I2C_NUMB_2,         //i2c2，PA9，PA10，新板不用
    HL_HAL_SOFT_I2C_NUMB_3,         //i2c3，PA3，PA2，新版：Guage、Charger、Boost、RTC
} hl_hal_soft_i2c_numb_e;

/* define --------------------------------------------------------------------*/

#define SOFT_I2C_API_FUNC_RET_OK 0
#define SOFT_I2C_API_FUNC_RET_ERR 1

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_hal_soft_i2c_api_init(hl_hal_soft_i2c_numb_e i2cx);

int hl_hal_soft_i2c_api_deinit(hl_hal_soft_i2c_numb_e i2cx);

int hl_hal_soft_i2c_api_read(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData, uint16_t len);

int hl_hal_soft_i2c_api_write(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData, uint16_t len);

#endif
/*
 * EOF
 */