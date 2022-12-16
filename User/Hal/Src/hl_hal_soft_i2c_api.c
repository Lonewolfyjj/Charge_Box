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

static void hl_hal_us_delay(uint16_t us);

static const soft_i2c_info i2c1_info = {
    9,           //sda_pin_num
    8,           //scl_pin_num
    GPIO_PIN_9,  //gpio_pin_sda
    GPIO_PIN_8,  //gpio_pin_scl
    GPIOB_BASE,  //gpiox_base
    RCC_APB2_PERIPH_GPIOB,
    GPIOB,           //gpiox
    hl_hal_us_delay  //微秒级延时函数名
};

static const soft_i2c_info i2c2_info = {
    10,           //sda_pin_num
    9,            //scl_pin_num
    GPIO_PIN_10,  //gpio_pin_sda
    GPIO_PIN_9,   //gpio_pin_scl
    GPIOA_BASE,   //gpiox_base
    RCC_APB2_PERIPH_GPIOA,
    GPIOA,           //gpiox
    hl_hal_us_delay  //微秒级延时函数名
};

static const soft_i2c_info i2c3_info = {
    2,           //sda_pin_num
    3,           //scl_pin_num
    GPIO_PIN_2,  //gpio_pin_sda
    GPIO_PIN_3,  //gpio_pin_scl
    GPIOA_BASE,  //gpiox_base
    RCC_APB2_PERIPH_GPIOA,
    GPIOA,           //gpiox
    hl_hal_us_delay  //微秒级延时函数名
};

static struct rt_mutex i2c1_mutex = { 0 };
static struct rt_mutex i2c2_mutex = { 0 };
static struct rt_mutex i2c3_mutex = { 0 };

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

    i2cx = HL_HAL_SOFT_I2C_NUMB_3;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag == true) {
                break;
            }

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init((soft_i2c_info*)&i2c1_info);
            rt_mutex_init(&i2c1_mutex, "i2c1_mutex", RT_IPC_FLAG_PRIO);
            i2c1_init_flag = true;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag == true) {
                break;
            }

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init((soft_i2c_info*)&i2c2_info);
            rt_mutex_init(&i2c2_mutex, "i2c2_mutex", RT_IPC_FLAG_PRIO);

            i2c2_init_flag = true;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag == true) {
                break;
            }

            /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
            hl_hal_soft_i2c_init((soft_i2c_info*)&i2c3_info);
            rt_mutex_init(&i2c3_mutex, "i2c3_mutex", RT_IPC_FLAG_PRIO);

            i2c3_init_flag = true;
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_deinit(hl_hal_soft_i2c_numb_e i2cx)
{

    i2cx = HL_HAL_SOFT_I2C_NUMB_3;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit((soft_i2c_info*)&i2c1_info);
            rt_mutex_detach(&i2c1_mutex);

            i2c1_init_flag = false;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit((soft_i2c_info*)&i2c2_info);
            rt_mutex_detach(&i2c2_mutex);

            i2c2_init_flag = false;
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag != true) {
                break;
            }

            hl_hal_soft_i2c_deinit((soft_i2c_info*)&i2c3_info);
            rt_mutex_detach(&i2c3_mutex);

            i2c3_init_flag = false;
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_read(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData,
                             uint16_t len)
{

    i2cx = HL_HAL_SOFT_I2C_NUMB_3;

    soft_i2c_dev_addr_info i2c_addr;

    i2c_addr.dev_writeaddr        = (dev_addr << 1) | 0x00;
    i2c_addr.dev_readaddr         = (dev_addr << 1) | 0x01;
    i2c_addr.pro_destination_addr = PointReg;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c1_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read((soft_i2c_info*)&i2c1_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c1_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c2_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read((soft_i2c_info*)&i2c2_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c2_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c3_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_read((soft_i2c_info*)&i2c3_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c3_mutex);
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

int hl_hal_soft_i2c_api_write(hl_hal_soft_i2c_numb_e i2cx, uint8_t dev_addr, uint8_t PointReg, uint8_t* pData,
                              uint16_t len)
{

    i2cx = HL_HAL_SOFT_I2C_NUMB_3;

    soft_i2c_dev_addr_info i2c_addr;

    i2c_addr.dev_writeaddr        = (dev_addr << 1) | 0x00;
    i2c_addr.dev_readaddr         = (dev_addr << 1) | 0x01;
    i2c_addr.pro_destination_addr = PointReg;

    switch (i2cx) {
        case HL_HAL_SOFT_I2C_NUMB_1: {
            if (i2c1_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c1_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write((soft_i2c_info*)&i2c1_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c1_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_2: {
            if (i2c2_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c2_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write((soft_i2c_info*)&i2c2_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c2_mutex);
        } break;
        case HL_HAL_SOFT_I2C_NUMB_3: {
            if (i2c3_init_flag != true) {
                return SOFT_I2C_API_FUNC_RET_ERR;
            }

            rt_mutex_take(&i2c3_mutex, RT_WAITING_FOREVER);
            hl_hal_soft_i2c_write((soft_i2c_info*)&i2c3_info, &i2c_addr, pData, len);
            rt_mutex_release(&i2c3_mutex);
        } break;
        default:
            break;
    }

    return SOFT_I2C_API_FUNC_RET_OK;
}

/*
 * EOF
 */