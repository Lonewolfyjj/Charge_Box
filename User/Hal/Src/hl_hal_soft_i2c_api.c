/**
 * @file hl_hal_soft_i2c_api.c
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
#include "hl_hal_soft_i2c.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

static soft_i2c_info i2c1_info = { 0 };
static soft_i2c_info i2c2_info = { 0 };
static soft_i2c_info i2c3_info = { 0 };

static rt_mutex_t i2c1_mutex = RT_NULL;
static rt_mutex_t i2c2_mutex = RT_NULL;
static rt_mutex_t i2c3_mutex = RT_NULL;

static bool i2c1_init_flag = false;
static bool i2c2_init_flag = false;
static bool i2c3_init_flag = false;

/* Private function(only *.c)  -----------------------------------------------*/

/**
 * This function will delay for some us.
 *
 * @param us the delay time of us
 */
static void hl_hal_us_delay(uint16_t us)
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    told  = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) {
                break;
            }
        }
    }
}

/* Exported functions --------------------------------------------------------*/

int hl_hal_soft_i2c_api_init(hl_hal_soft_i2c_numb_e i2cx)
{
    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag == true) {
                break;
            }

            i2c1_info.sda_pin_num      = 9;
            i2c1_info.scl_pin_num      = 8;
            i2c1_info.gpio_pin_sdax    = GPIO_PIN_9;
            i2c1_info.gpio_pin_sclx    = GPIO_PIN_8;
            i2c1_info.gpiox_base       = GPIOB_BASE;
            i2c1_info.rcc_periph_gpiox = RCC_APB2_PERIPH_GPIOB;
            i2c1_info.gpiox            = GPIOB;
            i2c1_info.delay_us         = hl_hal_us_delay;

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init(&i2c1_info);
            i2c1_mutex = rt_mutex_create("i2c1_mutex", RT_IPC_FLAG_PRIO);

            i2c1_init_flag = true;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag == true) {
                break;
            }

            i2c2_info.sda_pin_num      = 10;
            i2c2_info.scl_pin_num      = 9;
            i2c2_info.gpio_pin_sdax    = GPIO_PIN_10;
            i2c2_info.gpio_pin_sclx    = GPIO_PIN_9;
            i2c2_info.gpiox_base       = GPIOA_BASE;
            i2c2_info.rcc_periph_gpiox = RCC_APB2_PERIPH_GPIOA;
            i2c2_info.gpiox            = GPIOA;
            i2c2_info.delay_us         = hl_hal_us_delay;

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init(&i2c2_info);
            i2c2_mutex = rt_mutex_create("i2c2_mutex", RT_IPC_FLAG_PRIO);

            i2c2_init_flag = true;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag == true) {
                break;
            }
            
            i2c3_info.sda_pin_num      = 2;
            i2c3_info.scl_pin_num      = 3;
            i2c3_info.gpio_pin_sdax    = GPIO_PIN_2;
            i2c3_info.gpio_pin_sclx    = GPIO_PIN_3;
            i2c3_info.gpiox_base       = GPIOA_BASE;
            i2c3_info.rcc_periph_gpiox = RCC_APB2_PERIPH_GPIOA;
            i2c3_info.gpiox            = GPIOA;
            i2c3_info.delay_us         = hl_hal_us_delay;

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init(&i2c3_info);
            i2c3_mutex = rt_mutex_create("i2c3_mutex", RT_IPC_FLAG_PRIO);

            i2c3_init_flag = true;
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_deinit(hl_hal_soft_i2c_numb_e i2cx)
{
    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit(&i2c1_info);
            rt_mutex_delete(i2c1_mutex);

            i2c1_init_flag = false;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit(&i2c2_info);
            rt_mutex_delete(i2c2_mutex);

            i2c2_init_flag = false;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit(&i2c3_info);
            rt_mutex_delete(i2c3_mutex);

            i2c3_init_flag = false;
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_read(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData)
{
    soft_i2c_dev_addr_info i2c_addr;

    i2c_addr.dev_writeaddr        = (dev_addr << 1) | 0x00;
    i2c_addr.dev_readaddr         = (dev_addr << 1) | 0x01;
    i2c_addr.pro_destination_addr = PointReg;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c1_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read(&i2c1_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c1_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c2_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read(&i2c2_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c2_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c3_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read(&i2c3_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c3_mutex);
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_write(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData)
{
    soft_i2c_dev_addr_info i2c_addr;

    i2c_addr.dev_writeaddr        = (dev_addr << 1) | 0x00;
    i2c_addr.dev_readaddr         = (dev_addr << 1) | 0x01;
    i2c_addr.pro_destination_addr = PointReg;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c1_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write(&i2c1_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c1_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c2_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write(&i2c2_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c2_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {            
            if (i2c3_init_flag != true) {
                break;
            }

            rt_mutex_take(i2c3_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write(&i2c3_info, &i2c_addr, pData, 1);
            rt_mutex_release(i2c3_mutex);
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

/*
 * EOF
 */