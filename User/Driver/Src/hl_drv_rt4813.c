/**
 * @file hl_drv_rt4813.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-05
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
 * <tr><td>2022-12-05     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_rt4813.h"
#include "hl_hal_soft_i2c_api.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_SECTION_NAME "drv_rt4813"
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

#define	RT4813_REG01_ADDR		0x01
#define	RT4813_REG02_ADDR		0x03
#define	RT4813_REG03_ADDR		0x04

/* variables -----------------------------------------------------------------*/

static uint8_t _init_flag = 0;

/* Private function(only *.c)  -----------------------------------------------*/

static void _rt4813_write_reg(uint8_t w_addr, uint8_t *reg_data)
{
    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_2, RT4813_WRITE_ADDR, w_addr, reg_data, 1);
}

static void _rt4813_read_reg(uint8_t r_addr, uint8_t *reg_data)
{
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_2, RT4813_WRITE_ADDR, r_addr, reg_data, 1);
}

static void _rt4813_reg01_ctrl(uint8_t op_cmd, uint8_t val)
{
    hl_rt4813_reg01_t reg_val;
    _rt4813_read_reg(RT4813_REG01_ADDR, (uint8_t *)&reg_val);
    switch (op_cmd) {
        case HL_DRV_RT4813_IPCHG_CTRL:
            reg_val.IPCHG = val;
            break;
        case HL_DRV_RT4813_EN_ILMI_CTRL:
            reg_val.ILIM_OFF = val;
            break;
        default:
            break;
    }
    _rt4813_write_reg(RT4813_REG01_ADDR, (uint8_t *)&reg_val);
}

static void _rt4813_reg02_ctrl(uint8_t op_cmd, uint8_t val)
{
    hl_rt4813_reg02_t reg_val;
    _rt4813_read_reg(RT4813_REG02_ADDR, (uint8_t *)&reg_val);
    switch (op_cmd) {
        case HL_DRV_RT4813_ILIM_AVG_CTRL:
            reg_val.ILIM_AVG = val;
            break;
        case HL_DRV_RT4813_ILIM_SS_CTRL:
            reg_val.ILIM_SS = val;
            break;
        default:
            break;
    }
    _rt4813_write_reg(RT4813_REG02_ADDR, (uint8_t *)&reg_val);
}

static void _rt4813_reg03_ctrl(uint8_t op_cmd, uint8_t val)
{
    hl_rt4813_reg03_t reg_val;
    _rt4813_read_reg(RT4813_REG03_ADDR, (uint8_t *)&reg_val);
    switch (op_cmd) {
        case HL_DRV_RT4813_EN_DISCHARGE_CTRL:
            reg_val.EN_Discharge = val;
            break;
        case HL_DRV_RT4813_EN_IAVGCL_CTRL:
            reg_val.EN_IAVGCL = val;
            break;
        default:
            break;
    }
    _rt4813_write_reg(RT4813_REG03_ADDR, (uint8_t *)&reg_val);
}


/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_rt4813_init()
{
    hl_rt4813_reg01_t reg_val;

    if (_init_flag == 1) {
        LOG_E("rt4813 is already inited!\n");
        return RT4813_FUNC_RET_ERR;
    }
    /* SYS_IIC_SCL---PA3    SYS_IIC_SDA---PA2 */
    hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_3);

    _rt4813_read_reg(RT4813_REG01_ADDR, (uint8_t *)&reg_val);
    
    if (RT4813_LG_DRV_ABILITY != reg_val.DRV_SEL) {
        LOG_E("[error] rt4813 init!\n");
        return RT4813_FUNC_RET_ERR;
    }

    LOG_I("rt4813 init success!\n");

    _init_flag = 1;

    return RT4813_FUNC_RET_OK;
}

uint8_t hl_drv_rt4813_deinit()
{
    if (_init_flag == 0) {
        LOG_E("rt4813 is not inited!\n");
        return RT4813_FUNC_RET_ERR;
    }

    hl_hal_soft_i2c_api_deinit(HL_HAL_SOFT_I2C_NUMB_3);

    LOG_I("rt4813 deinit success!\n");
    _init_flag = 0;
    return RT4813_FUNC_RET_OK;
}

uint8_t hl_drv_rt4813_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size)
{
    if (_init_flag == 0) {
        LOG_E("rt4813 is not inited!\n");
        return RT4813_FUNC_RET_ERR;
    } else if (sizeof(uint8_t) != arg_size) {
        LOG_E("size err, rt4813_ctrl arg need <uint8_t> type pointer!\n");
        return RT4813_FUNC_RET_ERR;
    }
    uint8_t reg_val = *(uint8_t *)arg;
    switch (op_cmd) {
        case HL_DRV_RT4813_IPCHG_CTRL:
            _rt4813_reg01_ctrl(HL_DRV_RT4813_IPCHG_CTRL, reg_val);
            break;
        case HL_DRV_RT4813_EN_ILMI_CTRL:
            _rt4813_reg01_ctrl(HL_DRV_RT4813_EN_ILMI_CTRL, reg_val);
            break;
        case HL_DRV_RT4813_LG_DRV_ABILITY_GET:
            _rt4813_read_reg(RT4813_REG01_ADDR, (uint8_t *)arg);
            break;  
        case HL_DRV_RT4813_ILIM_AVG_CTRL:
            _rt4813_reg02_ctrl(HL_DRV_RT4813_ILIM_AVG_CTRL, reg_val);
            break;
        case HL_DRV_RT4813_ILIM_SS_CTRL:
            _rt4813_reg02_ctrl(HL_DRV_RT4813_ILIM_SS_CTRL, reg_val);
            break;
        case HL_DRV_RT4813_EN_DISCHARGE_CTRL:
            _rt4813_reg03_ctrl(HL_DRV_RT4813_EN_DISCHARGE_CTRL, reg_val);
            break;
        case HL_DRV_RT4813_EN_IAVGCL_CTRL:
            _rt4813_reg03_ctrl(HL_DRV_RT4813_EN_IAVGCL_CTRL, reg_val);
            break;
        default:
            break;
    }
    return RT4813_FUNC_RET_OK;
}


/*
 * EOF
 */