/**
 * @file hl_drv_aw2016a.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief LED驱动源文件
 * @version 1.0
 * @date 2022-09-06
 * 
 * @copyright Copyright (c) 2022 hollyland
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date           <th>Version  <th>Author         <th>Description
 * <tr><td>2022-09-06     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_aw2016a.h"
#include "hl_hal_soft_i2c_api.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

#define AW2016_IIC_BUS_0_NAME "i2c2"
#define AW2016_IIC_BUS_1_NAME "i2c3"

/* chip info */
#define AW2016A_IIC_DEV_ADDR 0x64

/* AW2016A reg base addr Macro */
#define AW2016A_REG_RSTR 0x00
#define AW2016A_REG_GCR1 0x01
#define AW2016A_REG_ISR 0x02
#define AW2016A_REG_PATST 0x03
#define AW2016A_REG_GCR2 0x04
#define AW2016A_REG_LCTR 0x30
#define AW2016A_REG_LCFG1 0x31
#define AW2016A_REG_LCFG2 0x32
#define AW2016A_REG_LCFG3 0x33
#define AW2016A_REG_PWM1 0x34
#define AW2016A_REG_PWM2 0x35
#define AW2016A_REG_PWM3 0x36
#define AW2016A_REG_LED1_T1_T2 0x37
#define AW2016A_REG_LED1_T3_T4 0x38
#define AW2016A_REG_LED1_T0_REPEAT 0x39
#define AW2016A_REG_LED2_T1_T2 0x3A
#define AW2016A_REG_LED2_T3_T4 0x3B
#define AW2016A_REG_LED2_T0_REPEAT 0x3C
#define AW2016A_REG_LED3_T1_T2 0x3D
#define AW2016A_REG_LED3_T3_T4 0x3E
#define AW2016A_REG_LED3_T0_REPEAT 0x3F

/* variables -----------------------------------------------------------------*/

static uint8_t _init_flag = 0;

/* Private function(only *.c)  -----------------------------------------------*/

static inline int aw_read(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t PointReg, uint8_t* pData)
{
    if (hl_hal_soft_i2c_api_read(*p_i2c_bus, AW2016A_IIC_DEV_ADDR, PointReg, pData) == SOFT_I2C_API_FUNC_RET_OK) {
        return AW2016A_FUNC_RET_OK;
    } else {
        DBG_LOG("i2c read err!\n");
        return AW2016A_FUNC_RET_ERR;
    }
}

static inline int aw_write(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t PointReg, uint8_t* pData)
{
    if (hl_hal_soft_i2c_api_write(*p_i2c_bus, AW2016A_IIC_DEV_ADDR, PointReg, pData) == SOFT_I2C_API_FUNC_RET_OK) {
        return AW2016A_FUNC_RET_OK;
    } else {
        DBG_LOG("i2c write err!\n");
        return AW2016A_FUNC_RET_ERR;
    }
}

static int get_chip_id(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_chip_id)
{
    int ret;

    ret = aw_read(p_i2c_bus, AW2016A_REG_RSTR, p_chip_id);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

/**
 * 
 * @brief reset internal logic and register 
 * @param [in] p_i2c_bus 
 * @return int 
 * @date 2022-09-09
 * @author lilin (lin.li@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-09-09      <td>lilin     <td>新建
 * </table>
 */
static int soft_reset(hl_hal_soft_i2c_numb_e* p_i2c_bus)
{
    int     ret;
    uint8_t data = 0x55;

    ret = aw_write(p_i2c_bus, AW2016A_REG_RSTR, &data);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_charge_indicator(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* disable)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_GCR1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val = (reg_val & (~(1 << 1))) | (*disable << 1);

    ret = aw_write(p_i2c_bus, AW2016A_REG_GCR1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_work_mode(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_work_mode)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_GCR1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val = (reg_val & (~(1 << 0))) | *p_work_mode;

    ret = aw_write(p_i2c_bus, AW2016A_REG_GCR1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_global_max_output_current(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_max_current)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_GCR2, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val = (reg_val & (~(0x03 << 0))) | *p_max_current;

    ret = aw_write(p_i2c_bus, AW2016A_REG_GCR2, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int open_led_channel(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_led_chan)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_LCTR, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val |= (*p_led_chan);

    ret = aw_write(p_i2c_bus, AW2016A_REG_LCTR, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int close_led_channel(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_led_chan)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_LCTR, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val &= (~(*p_led_chan));

    ret = aw_write(p_i2c_bus, AW2016A_REG_LCTR, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_pattern_mode(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_led_chan)
{
    int     ret;
    uint8_t reg_val;

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val |= (1 << 4);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val |= (1 << 4);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val |= (1 << 4);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_pattern_mode_param(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_breath_param_st* p_param)
{
    int     ret;
    uint8_t LEDx_T0;  //t1 t2
    uint8_t LEDx_T1;  //t3 t4
    uint8_t LEDx_T2;  //t0 repeat

    LEDx_T0 = ((p_param->t1 << 4) | p_param->t2);
    LEDx_T1 = ((p_param->t3 << 4) | p_param->t4);
    LEDx_T2 = ((p_param->t0 << 4) | p_param->repeat);

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_LED1_T1_T2, &LEDx_T0);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED1_T3_T4, &LEDx_T1);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED1_T0_REPEAT, &LEDx_T2);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_LED2_T1_T2, &LEDx_T0);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED2_T3_T4, &LEDx_T1);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED2_T0_REPEAT, &LEDx_T2);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_LED3_T1_T2, &LEDx_T0);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED3_T3_T4, &LEDx_T1);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        ret = aw_write(p_i2c_bus, AW2016A_REG_LED3_T0_REPEAT, &LEDx_T2);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_manual_mode(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* p_led_chan)
{
    int     ret;
    uint8_t reg_val;

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 4));

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 4));

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (*p_led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 4));

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_output_current(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_output_current_st* p_param)
{
    int     ret;
    uint8_t reg_val;

    if (p_param->current > 15) {
        p_param->current = 15;
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val = (reg_val & (~(0x0F << 0))) | p_param->current;

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val = (reg_val & (~(0x0F << 0))) | p_param->current;

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val = (reg_val & (~(0x0F << 0))) | p_param->current;

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_pwm_level(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_pwm_level_st* p_param)
{
    int ret;

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_PWM1, &(p_param->pwm_level));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_PWM2, &(p_param->pwm_level));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_write(p_i2c_bus, AW2016A_REG_PWM3, &(p_param->pwm_level));
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int dump_register_value(hl_hal_soft_i2c_numb_e* p_i2c_bus, uint8_t* reg_addr)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, *reg_addr, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    DBG_LOG("reg:%02x,value:%02x\n", *reg_addr, reg_val);

    return AW2016A_FUNC_RET_OK;
}

static int set_led_light_effect(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_light_st* p_param)
{
    int                              ret;
    hl_drv_aw2016a_output_current_st output_current;
    hl_drv_aw2016a_pwm_level_st      pwm_level;

    output_current.led_chan = HL_DRV_AW2016A_LED_CHANNEL1;
    output_current.current  = p_param->r;

    ret = set_output_current(p_i2c_bus, &output_current);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    output_current.led_chan = HL_DRV_AW2016A_LED_CHANNEL2;
    output_current.current  = p_param->g;

    ret = set_output_current(p_i2c_bus, &output_current);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    output_current.led_chan = HL_DRV_AW2016A_LED_CHANNEL3;
    output_current.current  = p_param->b;

    ret = set_output_current(p_i2c_bus, &output_current);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    pwm_level.led_chan  = HL_DRV_AW2016A_LED_CHANNEL1 | HL_DRV_AW2016A_LED_CHANNEL2 | HL_DRV_AW2016A_LED_CHANNEL3;
    pwm_level.pwm_level = p_param->brightness;

    ret = set_pwm_level(p_i2c_bus, &pwm_level);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_sync_control_mode(hl_hal_soft_i2c_numb_e* p_i2c_bus, bool* p_param)
{
    int     ret;
    uint8_t reg_val;

    ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    reg_val &= (~(1 << 7));
    reg_val |= ((*p_param) << 7);

    ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_fade_out_enable(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_set_fade_in_out_st* p_param)
{
    int     ret;
    uint8_t reg_val;

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 6));
        reg_val |= ((p_param->enable) << 6);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 6));
        reg_val |= ((p_param->enable) << 6);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 6));
        reg_val |= ((p_param->enable) << 6);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

static int set_fade_in_enable(hl_hal_soft_i2c_numb_e* p_i2c_bus, hl_drv_aw2016a_set_fade_in_out_st* p_param)
{
    int     ret;
    uint8_t reg_val;

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL1) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 5));
        reg_val |= ((p_param->enable) << 5);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG1, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL2) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 5));
        reg_val |= ((p_param->enable) << 5);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG2, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    if (p_param->led_chan & HL_DRV_AW2016A_LED_CHANNEL3) {
        ret = aw_read(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }

        reg_val &= (~(1 << 5));
        reg_val |= ((p_param->enable) << 5);

        ret = aw_write(p_i2c_bus, AW2016A_REG_LCFG3, &reg_val);
        if (ret == AW2016A_FUNC_RET_ERR) {
            return AW2016A_FUNC_RET_ERR;
        }
    }

    return AW2016A_FUNC_RET_OK;
}

/* Exported functions --------------------------------------------------------*/

int hl_drv_aw2016a_init(void)
{
    int                    ret;
    uint8_t                chip_id;
    uint8_t                flag;
    hl_hal_soft_i2c_numb_e i2c2 = HL_HAL_SOFT_I2C_NUMB_2;
    hl_hal_soft_i2c_numb_e i2c3 = HL_HAL_SOFT_I2C_NUMB_3;

    if (_init_flag == 1) {
        DBG_LOG("LED is already inited!\n");
        return AW2016A_FUNC_RET_ERR;
    }

    hl_hal_soft_i2c_api_init(i2c2);
    hl_hal_soft_i2c_api_init(i2c3);

    ret = get_chip_id(&i2c2, &chip_id);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    if (chip_id != AW2016A_CHIP_ID) {
        DBG_LOG("aw2016a chip0 id err: %02x!", chip_id);
        return AW2016A_FUNC_RET_ERR;
    }

    ret = get_chip_id(&i2c3, &chip_id);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    if (chip_id != AW2016A_CHIP_ID) {
        DBG_LOG("aw2016a chip1 id err: %02x!", chip_id);
        return AW2016A_FUNC_RET_ERR;
    }

    flag = 1;

    ret = set_charge_indicator(&i2c2, &flag);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    ret = set_charge_indicator(&i2c3, &flag);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    DBG_LOG("LED init success!\n");

    _init_flag = 1;

    return AW2016A_FUNC_RET_OK;
}

int hl_drv_aw2016a_deinit(void)
{
    int                    ret;
    uint8_t                work_mode;
    hl_hal_soft_i2c_numb_e i2c2 = HL_HAL_SOFT_I2C_NUMB_2;
    hl_hal_soft_i2c_numb_e i2c3 = HL_HAL_SOFT_I2C_NUMB_3;

    if (_init_flag == 0) {
        DBG_LOG("LED is not inited!\n");
        return AW2016A_FUNC_RET_ERR;
    }

    work_mode = HL_DRV_AW2016A_STANDBY_MODE;

    ret = set_work_mode(&i2c2, &work_mode);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    ret = set_work_mode(&i2c3, &work_mode);
    if (ret == AW2016A_FUNC_RET_ERR) {
        return AW2016A_FUNC_RET_ERR;
    }

    DBG_LOG("LED deinit success!\n");

    _init_flag = 0;

    return AW2016A_FUNC_RET_OK;
}

int hl_drv_aw2016a_ctrl(uint8_t led_num, uint8_t op, void* arg, int32_t arg_size)
{
    int                    ret;
    hl_hal_soft_i2c_numb_e i2cx;

    if (_init_flag == 0) {
        DBG_LOG("LED is not inited!\n");
        return AW2016A_FUNC_RET_ERR;
    }

    if (led_num == HL_DRV_AW2016A_LED0) {
        i2cx = HL_HAL_SOFT_I2C_NUMB_2;
    } else if (led_num == HL_DRV_AW2016A_LED1) {
        i2cx = HL_HAL_SOFT_I2C_NUMB_3;
    } else {
        DBG_LOG("LED NUM is not 0 or 1!\n");
        return AW2016A_FUNC_RET_ERR;
    }

    switch (op) {
        case HL_DRV_AW2016A_GET_CHIP_ID: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = get_chip_id(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_WORK_MODE: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_work_mode(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_GLOBAL_MAX_OUTPUT_CURRENT: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_global_max_output_current(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_OPEN_LED_CHANNEL: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = open_led_channel(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_CLOSE_LED_CHANNEL: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = close_led_channel(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_PATTERN_MODE: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_pattern_mode(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_BREATH_PARAM: {
            if (arg_size != sizeof(hl_drv_aw2016a_breath_param_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_breath_param_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_pattern_mode_param(&i2cx, (hl_drv_aw2016a_breath_param_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_MANUAL_MODE: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_manual_mode(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_LED_CHANNEL_OUTPUT_CURRENT: {
            if (arg_size != sizeof(hl_drv_aw2016a_output_current_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_output_current_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_output_current(&i2cx, (hl_drv_aw2016a_output_current_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_LED_CHANNEL_PWM_LEVEL: {
            if (arg_size != sizeof(hl_drv_aw2016a_pwm_level_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_pwm_level_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_pwm_level(&i2cx, (hl_drv_aw2016a_pwm_level_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_DUMP_REGISTER_VALUE: {
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = dump_register_value(&i2cx, (uint8_t*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_LED_LIGHT_EFFECT: {
            if (arg_size != sizeof(hl_drv_aw2016a_light_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_light_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_led_light_effect(&i2cx, (hl_drv_aw2016a_light_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_SYNC_CONTROL_MODE: {
            if (arg_size != sizeof(bool)) {
                DBG_LOG("size err, ctrl arg need <bool> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_sync_control_mode(&i2cx, (bool*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_FADE_OUT_ENABLE: {
            if (arg_size != sizeof(hl_drv_aw2016a_set_fade_in_out_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_set_fade_in_out_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_fade_out_enable(&i2cx, (hl_drv_aw2016a_set_fade_in_out_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        case HL_DRV_AW2016A_SET_FADE_IN_ENABLE: {
            if (arg_size != sizeof(hl_drv_aw2016a_set_fade_in_out_st)) {
                DBG_LOG("size err, ctrl arg need <hl_drv_aw2016a_set_fade_in_out_st> type pointer!\n");
                return AW2016A_FUNC_RET_ERR;
            }

            ret = set_fade_in_enable(&i2cx, (hl_drv_aw2016a_set_fade_in_out_st*)arg);
            if (ret == AW2016A_FUNC_RET_ERR) {
                return AW2016A_FUNC_RET_ERR;
            }
        } break;
        default:
            break;
    }

    return AW2016A_FUNC_RET_OK;
}

/*
 * EOF
 */