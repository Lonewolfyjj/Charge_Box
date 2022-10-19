/**
 * @file hl_drv_charge.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
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
 * <tr><td>2022-10-18     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "hl_drv_sgm41513.h"
#include "hl_hal_soft_i2c_api.h"
#include "hl_hal_soft_i2c.h"
#include "stdint.h"
#include "n32l40x.h"
/* typedef -------------------------------------------------------------------*/

/* define --------------------------------------------------------------------*/

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

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

soft_i2c_info i2c_str1 = {
        10,              //sda_pin_num
        9,              //scl_pin_num
        GPIO_PIN_10,     //gpio_pin_sda
        GPIO_PIN_9,     //gpio_pin_scl
        GPIOA_BASE,     //gpiox_base
        RCC_APB2_PERIPH_GPIOA,
        GPIOA,          //gpiox
        hl_delay_timer_delay_us //微秒级延时函数名
    };
soft_i2c_dev_addr_info i2c_addr1 = {
    0x35,           //dev_readaddr
    0x34,           //dev_writeaddr                                                             
    0x00            //pro_destination_addr
};

void hl_drv_sgm41513_write_reg(rt_uint8_t w_addr, rt_uint8_t *reg_data)
{
    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_2, SGM41513_WRITE_ADDR, w_addr, reg_data);
}

void hl_drv_sgm41513_read_reg(rt_uint8_t r_addr, rt_uint8_t *reg_data)
{
    //hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_2, SGM41513_READ_ADDR, r_addr, reg_data);

    i2c_addr1.pro_destination_addr = r_addr;
    hl_hal_soft_i2c_read(&i2c_str1, &i2c_addr1, reg_data, 1);
}


static rt_uint8_t hl_drv_sgm41513_get_charger_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (rt_uint8_t *)&reg_val);

    if ( reg_val.CHRG_STAT == 0x01 || reg_val.CHRG_STAT == 0x02) {
        return CHARGER_RUN;   /* 正在充电 */
    } else if (reg_val.CHRG_STAT == 0x03) {
        return CHARGER_FULL;   /* 充满电 */
    } else {
        return NOT_CHARGER;   /* 充电禁止 */
    }
    
}

static void hl_drv_sgm41513_set_charger_status(rt_uint8_t enable_state)
{
    hl_sgm41513_reg01_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (rt_uint8_t *)&reg_val);
    if (enable_state == CHARGER_CLOSE) {    /* 充电不开启 */
        reg_val.CHG_CONFIG = CHARGER_CLOSE;
    } else {                    /* 充电开启 */
        reg_val.CHG_CONFIG = CHARGER_OPEN;
    }
    hl_drv_sgm41513_write_reg(REG08_ADDR, (rt_uint8_t *)&reg_val);
}



void hl_drv_sgm41513_init()
{
    hl_sgm41513_reg00_t	reg00;
	hl_sgm41513_reg01_t reg01;
	hl_sgm41513_reg02_t	reg02;
	hl_sgm41513_reg03_t reg03;
	hl_sgm41513_reg04_t reg04;
	hl_sgm41513_reg05_t reg05;
	hl_sgm41513_reg06_t reg06;
	hl_sgm41513_reg07_t reg07;

    //hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_2);
    uint8_t data = 0;
    /* 延时定时器初始化并启动 */
    hl_delay_timer_tim_init();
    
    /* I2C引脚初始化，并使能I2C引脚对应的GPIO组外设时钟 */
    hl_hal_soft_i2c_init(&i2c_str1);

    reg00.IINDPM = 31; // 3200mA
	reg00.EN_ICHG_MON = 0;
	reg00.EN_HIZ = 0;
	
	reg01.MIN_BAT_SEL = 0;
	reg01.SYS_MIN = 5;
	reg01.CHG_CONFIG = 1;
	reg01.OTG_CONFIG = 0;
	reg01.WD_RST = 0;
	reg01.PFM_DIS = 0;
	
	reg02.ICHG = 63; // 1280mA
	reg02.Q1_FULLON = 0; //?
	reg02.BOOST_LIM = 1;

	reg03.ITERM = 0; // 20mA    
	reg03.IPRECHG = 16;	// 320mA

	reg04.VRECHG = 0; 
	reg04.TOPOFF_TIMER = 0;
	reg04.VREG = 12; // 4.24V 

	reg05.JEITA_ISET = 0; 
	reg05.TREG = 1;
	reg05.CHG_TIMER = 1;
	reg05.EN_TIMER = 1; 
	reg05.WATCHDOG = 0;
	reg05.EN_TERM = 1;

	reg06.VINDPM = 6; //4.5V
	reg06.BOOSTV = 2;
	reg06.OVP = 1;

	reg07.VDPM_BAT_TRACK = 0; 
	reg07.BATFET_RST_EN = 1;
	reg07.BATFET_DLY = 1;
	reg07.JEITA_VSET = 1; 
	reg07.BATFET_DIS = 0;
	reg07.TMR2X_EN = 1;
	reg07.IINDET_EN = 1;
    
    
    //hl_drv_sgm41513_write_reg();


}

void hl_drv_sgm41513_deinit()
{

}


void hl_drv_sgm41513_ctrl(uint8_t op, void* arg, int32_t arg_size)
{

}

/*
 * EOF
 */