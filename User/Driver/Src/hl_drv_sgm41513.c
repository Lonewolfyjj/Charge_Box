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
#include "n32l40x.h"
/* typedef -------------------------------------------------------------------*/

/* define --------------------------------------------------------------------*/

#define	REG00_ADDR		0x00
#define	REG01_ADDR		0x01
#define	REG02_ADDR		0x02
#define	REG03_ADDR		0x03
#define	REG04_ADDR		0x04
#define	REG05_ADDR		0x05
#define	REG06_ADDR		0x06
#define	REG07_ADDR		0x07
#define	REG08_ADDR		0x08
#define	REG09_ADDR		0x09
#define	REG0A_ADDR		0x0A
#define	REG0B_ADDR		0x0B
#define	REG0E_ADDR		0x0E

#define LOG_I    rt_kprintf

#define DBG_SECTION_NAME "drv_sgm41513"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

#define REG_BIT_VAL(n)  (((n) == 0) ? (0):(1))

/* variables -----------------------------------------------------------------*/

static bool sgm41513_init_status = false;

/* Private function(only *.c)  -----------------------------------------------*/

static void hl_drv_sgm41513_write_reg(uint8_t w_addr, uint8_t *reg_data)
{
    hl_hal_soft_i2c_api_write(HL_HAL_SOFT_I2C_NUMB_2, SGM41513_WRITE_ADDR, w_addr, reg_data, 1);
}

static void hl_drv_sgm41513_read_reg(uint8_t r_addr, uint8_t *reg_data)
{
    hl_hal_soft_i2c_api_read(HL_HAL_SOFT_I2C_NUMB_2, SGM41513_WRITE_ADDR, r_addr, reg_data, 1);
}


static void set_input_current_limit(uint8_t val)
{
    hl_sgm41513_reg00_t reg_val;
    hl_drv_sgm41513_read_reg(REG00_ADDR, (uint8_t *)&reg_val);
    reg_val.IINDPM = val;
    hl_drv_sgm41513_write_reg(REG00_ADDR, (uint8_t *)&reg_val); 
}

/* 设置充电状态 */
/* val : CHARGE_CLOSE-0  CHARGE_OPEN-1 */
static void set_charge_status(uint8_t val)
{
    hl_sgm41513_reg01_t reg_val;
    hl_drv_sgm41513_read_reg(REG01_ADDR, (uint8_t *)&reg_val);
    reg_val.CHG_CONFIG = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG01_ADDR, (uint8_t *)&reg_val);   
}

static void set_fast_charge_current(uint8_t val)
{
    hl_sgm41513_reg02_t reg_val;
    hl_drv_sgm41513_read_reg(REG02_ADDR, (uint8_t *)&reg_val);
    reg_val.ICHG = val;
    hl_drv_sgm41513_write_reg(REG02_ADDR, (uint8_t *)&reg_val);
}

static void set_vbus_fet_enable_status(uint8_t val)
{
    hl_sgm41513_reg02_t reg_val;
    hl_drv_sgm41513_read_reg(REG02_ADDR, (uint8_t *)&reg_val);
    reg_val.Q1_FULLON = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG02_ADDR, (uint8_t *)&reg_val);
}

static void set_boost_mode_current(uint8_t val)
{
    hl_sgm41513_reg02_t reg_val;
    hl_drv_sgm41513_read_reg(REG02_ADDR, (uint8_t *)&reg_val);
    reg_val.BOOST_LIM = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG02_ADDR, (uint8_t *)&reg_val); 
}

/* 设置终止充电的电流值 */
static void set_termination_charge_current(uint8_t val)
{
    hl_sgm41513_reg03_t reg_val;
    hl_drv_sgm41513_read_reg(REG03_ADDR, (uint8_t *)&reg_val);
    reg_val.ITERM = val;
    hl_drv_sgm41513_write_reg(REG03_ADDR, (uint8_t *)&reg_val); 
}

/* 设置预充电的电流值 */
static void set_pre_charge_current(uint8_t val)
{
    hl_sgm41513_reg03_t reg_val;
    hl_drv_sgm41513_read_reg(REG03_ADDR, (uint8_t *)&reg_val);
    reg_val.IPRECHG = val;
    hl_drv_sgm41513_write_reg(REG03_ADDR, (uint8_t *)&reg_val); 
}

/* 设置电池循环充电的电压阈值 */
static void set_recharge_voltage_threshold(uint8_t val)
{
    hl_sgm41513_reg04_t reg_val;
    hl_drv_sgm41513_read_reg(REG04_ADDR, (uint8_t *)&reg_val);
    reg_val.VRECHG = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG04_ADDR, (uint8_t *)&reg_val); 
    
}

static void set_charge_voltage_limit(uint8_t val)
{
    hl_sgm41513_reg04_t reg_val;
    hl_drv_sgm41513_read_reg(REG04_ADDR, (uint8_t *)&reg_val);
    reg_val.VREG = val;
    hl_drv_sgm41513_write_reg(REG04_ADDR, (uint8_t *)&reg_val);
}

static void set_charge_full_delay_time(uint8_t val)
{
    hl_sgm41513_reg04_t reg_val;
    hl_drv_sgm41513_read_reg(REG04_ADDR, (uint8_t *)&reg_val);
    reg_val.TOPOFF_TIMER = val;
    hl_drv_sgm41513_write_reg(REG04_ADDR, (uint8_t *)&reg_val);
    
}

static void set_jeita_charge_current(uint8_t val)
{
    hl_sgm41513_reg05_t reg_val;
    hl_drv_sgm41513_read_reg(REG05_ADDR, (uint8_t *)&reg_val);
    reg_val.JEITA_ISET = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG05_ADDR, (uint8_t *)&reg_val);
}    
 
static void set_charge_safe_timer_time_val(uint8_t val)
{
    hl_sgm41513_reg05_t reg_val;
    hl_drv_sgm41513_read_reg(REG05_ADDR, (uint8_t *)&reg_val);
    reg_val.CHG_TIMER = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG05_ADDR, (uint8_t *)&reg_val);

}
/* 设置热调节阈值 */
static void set_thremal_regulation_threshold(uint8_t val)
{
    hl_sgm41513_reg05_t reg_val;
    hl_drv_sgm41513_read_reg(REG05_ADDR, (uint8_t *)&reg_val);
    reg_val.TREG = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG05_ADDR, (uint8_t *)&reg_val);
}

/* 充电终止功能是否启用 0-disable 1-enbale */
static void set_charge_termination_enable(uint8_t val)
{
    hl_sgm41513_reg05_t reg_val;
    hl_drv_sgm41513_read_reg(REG05_ADDR, (uint8_t *)&reg_val);
    reg_val.EN_TERM = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG05_ADDR, (uint8_t *)&reg_val);
}

/* 设置电压动态电源管理阈值 */
static void set_vindpm_threshold(uint8_t val)
{
    hl_sgm41513_reg06_t reg_val;
    hl_drv_sgm41513_read_reg(REG06_ADDR, (uint8_t *)&reg_val);
    reg_val.VINDPM = val;
    hl_drv_sgm41513_write_reg(REG06_ADDR, (uint8_t *)&reg_val);
}

/* 设置Boost 模式下的电压调节值 */
static void set_boost_mode_voltage_regulation_val(uint8_t val)
{
    hl_sgm41513_reg06_t reg_val;
    hl_drv_sgm41513_read_reg(REG06_ADDR, (uint8_t *)&reg_val);
    reg_val.BOOSTV = val;
    hl_drv_sgm41513_write_reg(REG06_ADDR, (uint8_t *)&reg_val);
}

/* 设置过压保护阈值 */
static void set_vac_pin_over_voltage_protect_thershold(uint8_t val)
{
    hl_sgm41513_reg06_t reg_val;
    hl_drv_sgm41513_read_reg(REG06_ADDR, (uint8_t *)&reg_val);
    reg_val.OVP = val;
    hl_drv_sgm41513_write_reg(REG06_ADDR, (uint8_t *)&reg_val);
}


static void set_jeita_charge_voltage(uint8_t val)
{
    hl_sgm41513_reg07_t reg_val;
    hl_drv_sgm41513_read_reg(REG07_ADDR, (uint8_t *)&reg_val);
    reg_val.JEITA_VSET = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG07_ADDR, (uint8_t *)&reg_val);
}

static void set_battery_fet_enbale(uint8_t val)
{
    hl_sgm41513_reg07_t reg_val;
    hl_drv_sgm41513_read_reg(REG07_ADDR, (uint8_t *)&reg_val);
    reg_val.BATFET_DIS = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG07_ADDR, (uint8_t *)&reg_val);
}

static void set_iindet_en(uint8_t val)
{
    hl_sgm41513_reg07_t reg_val;
    hl_drv_sgm41513_read_reg(REG07_ADDR, (uint8_t *)&reg_val);
    reg_val.IINDET_EN = REG_BIT_VAL(val);
    hl_drv_sgm41513_write_reg(REG07_ADDR, (uint8_t *)&reg_val);
}

static void reset_all_reg_val(void)
{
    hl_sgm41513_reg0b_t reg_val;
    hl_drv_sgm41513_read_reg(REG0B_ADDR, (uint8_t *)&reg_val);
    reg_val.REG_RST = 1;
    hl_drv_sgm41513_write_reg(REG0B_ADDR, (uint8_t *)&reg_val);
}
static uint8_t get_sys_voltage_regulation_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (uint8_t *)&reg_val);
    if (reg_val.VSYS_STAT) {
        return EXIST_SYS_VOL_REGULATION;
    } else {
        return NOT_SYS_VOL_REGULATION;
    }
}

static uint8_t get_thermal_regulation_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (uint8_t *)&reg_val);
    if (reg_val.THERM_STAT) {
        return EXIST_THERMAL_REGULATION;
    } else {
        return NOT_THERMAL_REGULATION;
    }
}

static uint8_t get_input_power_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (uint8_t *)&reg_val);
    if (reg_val.PG_STAT) {
        return EXIST_INPUT_POWER;
    } else {
        return NOT_INPUT_POWER;
    }
}

static uint8_t get_charge_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (uint8_t *)&reg_val);
    return reg_val.CHRG_STAT;
}

static uint8_t get_vbus_status()
{
    hl_sgm41513_charge_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG08_ADDR, (uint8_t *)&reg_val);
    return reg_val.VBUS_STAT;
}

static uint8_t get_battery_temp_status()
{
    uint8_t temp_status;
    hl_sgm41513_fault_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG09_ADDR, (uint8_t *)&reg_val);
    switch (reg_val.NTC_FAULT) {
        case 0:
            temp_status = NORMAL;
            break;
        case 2:
            temp_status = TEMP_WARM;
            break;
        case 3:
            temp_status = TEMP_COOL;
            break;
        case 5:
            temp_status = TEMP_COLD;
            break;
        case 6:
            temp_status = TEMP_HOT;
            break;
        default:
            break;
    }
    return temp_status;
    
}

static uint8_t get_battery_error_status()
{
    hl_sgm41513_fault_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG09_ADDR, (uint8_t *)&reg_val);
    if (reg_val.BAT_FAULT) {
        return BATTERY_OVER_VOL;
    } else {
        return NORMAL;
    }
    
}

static uint8_t get_charge_error_status()
{
    uint8_t charge_status;
    hl_sgm41513_fault_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG09_ADDR, (uint8_t *)&reg_val);
    switch (reg_val.NTC_FAULT) {
        case NORMAL:
            charge_status = NORMAL;
            break;
        case CHARGE_INPUT_ERROR:
            charge_status = CHARGE_INPUT_ERROR;
            break;
        case CHARGE_THERMAL_SHUTDOWN:
            charge_status = CHARGE_THERMAL_SHUTDOWN;
            break;
        case CHARGE_SAFT_TIMER_EXPIRED:
            charge_status = CHARGE_SAFT_TIMER_EXPIRED;
            break;
        default:
            break;
    }
    return charge_status;
    
}


static uint8_t get_boost_mode_error_status()
{
    hl_sgm41513_fault_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG09_ADDR, (uint8_t *)&reg_val);
    if (reg_val.BOOST_FAULT) {
        return BOOST_MODE_ERROR;
    } else {
        return NORMAL;
    }
}

static uint8_t get_watchdog_error_status()
{
    hl_sgm41513_fault_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG09_ADDR, (uint8_t *)&reg_val);
    if (reg_val.WATCHDOG_FAULT) {
        return WATCHDOG_ERROR;
    } else {
        return NORMAL;
    }
}

static uint8_t get_input_over_voltage_status()
{
    hl_sgm41513_voltage_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG0A_ADDR, (uint8_t *)&reg_val);
    if (reg_val.ACOV_STAT) {
        return INPUT_OVER_VOL;
    } else {
        return NORMAL;
    }
    
}

static uint8_t get_vindpm_status()
{
    hl_sgm41513_voltage_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG0A_ADDR, (uint8_t *)&reg_val);
    return reg_val.VINDPM_STAT;
    
}

static uint8_t get_iindpm_status()
{
    hl_sgm41513_voltage_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG0A_ADDR, (uint8_t *)&reg_val);
    return reg_val.IINDPM_STAT;
    
}

static uint8_t get_vbus_connect_status()
{
    hl_sgm41513_voltage_status_t reg_val;
    hl_drv_sgm41513_read_reg(REG0A_ADDR, (uint8_t *)&reg_val);
    return reg_val.VBUS_GD;
}

static uint8_t get_chip_part_id()
{
    hl_sgm41513_reg0b_t reg_val;
    hl_drv_sgm41513_read_reg(REG0B_ADDR, (uint8_t *)&reg_val);
    return reg_val.PN;
    
}

static uint8_t get_vbus_input_detection_status()
{
    hl_sgm41513_reg0e_t reg_val;
    hl_drv_sgm41513_read_reg(REG0E_ADDR, (uint8_t *)&reg_val);
    return reg_val.INPUT_DET_DONE;
    
}

static void hl_drv_sgm41513_printf_reg_val(void)
{
    hl_sgm41513_info_t info;
    uint8_t data;
    hl_drv_sgm41513_read_reg(REG00_ADDR, (rt_uint8_t *)&info.reg00);
    LOG_I("EN_HIZ:%x, EN_ICHG_MON:%x, IINDPM:%x\n", info.reg00.EN_HIZ, info.reg00.EN_ICHG_MON, info.reg00.IINDPM);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG01_ADDR, (rt_uint8_t *)&info.reg01);
    LOG_I("PFM_DIS:%x, WD_RST:%x, OTG_CONFIG:%x, CHG_CONFIG:%x, SYS_MIN:%x, MIN_BAT_SEL:%x\n", info.reg01.PFM_DIS, info.reg01.WD_RST, 
                                        info.reg01.OTG_CONFIG, info.reg01.CHG_CONFIG, info.reg01.SYS_MIN, info.reg01.MIN_BAT_SEL);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG02_ADDR, (rt_uint8_t *)&info.reg02);
    LOG_I("BOOST_LIM:%x, Q1_FULLON:%x, ICHG:%x\n", info.reg02.BOOST_LIM, info.reg02.Q1_FULLON, info.reg02.ICHG);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG03_ADDR, (rt_uint8_t *)&info.reg03);
    LOG_I("IPRECHG:%x, ITERM:%x\n", info.reg03.IPRECHG, info.reg03.ITERM);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG04_ADDR, (rt_uint8_t *)&info.reg04);
    LOG_I("VREG:%x, TOPOFF_TIMER:%x, VRECHG:%x\n", info.reg04.VREG, info.reg04.TOPOFF_TIMER, info.reg04.VRECHG);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG05_ADDR, (rt_uint8_t *)&info.reg05);
    LOG_I("EN_TERM:%x, ITERM_TIMER:%x, WATCHDOG:%x, EN_TIMER:%x, CHG_TIMER:%x, TREG:%x, JEITA_ISET:%x\n", 
    info.reg05.EN_TERM, info.reg05.ITERM_TIMER, info.reg05.WATCHDOG, info.reg05.EN_TIMER, 
    info.reg05.CHG_TIMER, info.reg05.TREG, info.reg05.JEITA_ISET);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG06_ADDR, (rt_uint8_t *)&info.reg06);
    LOG_I("OVP:%x, BOOSTV:%x, VINDPM:%x\n", info.reg06.OVP, info.reg06.BOOSTV, info.reg06.VINDPM);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG07_ADDR, (rt_uint8_t *)&info.reg07);
    LOG_I("IINDET_EN:%x, TMR2X_EN:%x, BATFET_RST_EN:%x, JEITA_VSET:%x, BATFET_DLY:%x, BATFET_RST_EN:%x, VDPM_BAT_TRACK:%x\n",
    info.reg07.IINDET_EN, info.reg07.TMR2X_EN, info.reg07.BATFET_RST_EN, info.reg07.JEITA_VSET, info.reg07.BATFET_DLY,
    info.reg07.BATFET_RST_EN, info.reg07.VDPM_BAT_TRACK);
    LOG_I("\n*************************************************************\n");
 
    hl_drv_sgm41513_read_reg(REG08_ADDR, (rt_uint8_t *)&info.charge_stu);
    LOG_I("VBUS_STAT:%x, CHRG_STAT:%x, PG_STAT:%x, THERM_STAT:%x, VSYS_STAT:%x\n", 
    info.charge_stu.VBUS_STAT, info.charge_stu.CHRG_STAT, info.charge_stu.PG_STAT, info.charge_stu.THERM_STAT, info.charge_stu.VSYS_STAT);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG09_ADDR, (rt_uint8_t *)&info.fault_stu);
    LOG_I("WATCHDOG_FAULT:%x, BOOST_FAULT:%x, CHRG_FAULT:%x, BAT_FAULT:%x, NTC_FAULT:%x\n", 
    info.fault_stu.WATCHDOG_FAULT, info.fault_stu.BOOST_FAULT, info.fault_stu.CHRG_FAULT, info.fault_stu.BAT_FAULT, info.fault_stu.NTC_FAULT);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG0A_ADDR, (rt_uint8_t *)&info.vol_stu);
    LOG_I("VBUS_GD:%x, VINDPM_STAT:%x, IINDPM_STAT:%x, TOPOFF_ACTIVE:%x, ACOV_STAT:%x, VINDPM_INT_MASK:%x, IINDPM_INT_MASK:%x\n",
    info.vol_stu.VBUS_GD, info.vol_stu.VINDPM_STAT, info.vol_stu.IINDPM_STAT, info.vol_stu.TOPOFF_ACTIVE, 
    info.vol_stu.ACOV_STAT, info.vol_stu.VINDPM_INT_MASK, info.vol_stu.IINDPM_INT_MASK);
    LOG_I("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG0B_ADDR, (rt_uint8_t *)&info.reg0b);
    LOG_I("REG_RST:%x, PN:%x, SGMPART:%x, DEV_REV:%x\n", info.reg0b.REG_RST, info.reg0b.PN, info.reg0b.SGMPART, info.reg0b.DEV_REV);
    LOG_I("\n*************************************************************\n");
    
    hl_drv_sgm41513_read_reg(REG08_ADDR, &data);
    LOG_I("--usb--bit 7-5 : %d, %d, %d\n", (data >> 7) & 0x1, (data >> 6) & 0x1, (data >> 5) & 0x1);
    LOG_I("--usb--bit 4-3 : %d, %d\n", (data >> 4) & 0x01, (data >> 3) & 0x1);
    LOG_I("--usb--bit 2 : %d\n", (data >> 2) & 0x01);
    LOG_I("--usb--bit 1 : %d\n", (data >> 1) & 0x01);
    LOG_I("--usb--bit 0 : %d\n", data & 0x01);
    LOG_I("\n*************************************************************\n");
}

static uint8_t _sgm41513_init_set()
{
    hl_sgm41513_reg00_t	st_reg00;
	hl_sgm41513_reg01_t st_reg01;
	hl_sgm41513_reg02_t	st_reg02;
	hl_sgm41513_reg03_t st_reg03;
	hl_sgm41513_reg04_t st_reg04;
	hl_sgm41513_reg05_t st_reg05;
	hl_sgm41513_reg06_t st_reg06;
	hl_sgm41513_reg07_t st_reg07;

    st_reg00.IINDPM = 0x17;     // 2400mA  Input Current Limit Value(default)
	st_reg00.EN_ICHG_MON = 0;   // 0 = 充电状态使能(default)
	st_reg00.EN_HIZ = 0;        //Enable HIZ Mode(default)
	
	st_reg01.MIN_BAT_SEL = 0;   //OTG Mode下的，Minimum Battery Voltage （0）2.8v(default)  （1）2.5v
	st_reg01.SYS_MIN = 5;       // 3.5v Minimum System Voltage(default)
	st_reg01.CHG_CONFIG = 1;    // 1 = 电池充电使能 (default)
	st_reg01.OTG_CONFIG = 0;    // 0 = OTG disable (default)
	st_reg01.WD_RST = 0;        // 0 = Normal ，I2C Watchdog Timer Reset(default)
	st_reg01.PFM_DIS = 0;       // 0 = 启用脉冲频率调制 (default)
	
	st_reg02.ICHG = 0x32;       // 1980mA (110100)快速充电电流值(default)-------------非默认，1740mA(110010)
	st_reg02.Q1_FULLON = 0;     // VBUS FET 开关(default)
	st_reg02.BOOST_LIM = 1;     //Boost Mode Current Limit  （0）0.5A  （1）1.2A(default)


	st_reg03.ITERM = 0x0a;      // 120mA 充电终止电流(default)
	st_reg03.IPRECHG = 0x0a;	// 120mA 预充电电流(default)


	st_reg04.VRECHG = 0;        //电池循环充电阈值 0:100mV(default)  1:200mV
	st_reg04.TOPOFF_TIMER = 0;  //检测到充电终止条件后，增加的充电延长时间，0就是禁用(default)
	st_reg04.VREG = 17;       // 4.208V (11)最高充电电压限制(default) ---------------------------非默认, 4.4V(17)

	st_reg05.JEITA_ISET = 0;    // 1 = 20% of I-CHG (default) 低温充电电流限制值
	st_reg05.TREG = 1;          //热调节阈值 0:80℃  1:120℃(default)
	st_reg05.CHG_TIMER = 1;     // Charge Safety Timer Setting  0:4hours  1:16hours(default)
	st_reg05.EN_TIMER = 1;      // 1 = 充电安全定时器使能(default)
	st_reg05.WATCHDOG = 0;      // 0 = Watchdog Timer Setting disable--------------------------非默认,禁用看门狗定时器
	st_reg05.EN_TERM = 1;       // 1 = 充电终止启用 (default)


	st_reg06.VINDPM = 6;        // 0110 = 4.5V (default)输入电压动态电源管理阈值
	st_reg06.BOOSTV = 2;        // 10 = 5.15V (default) Boost Mode 电压调节
	st_reg06.OVP = 1;           // 01 = 6.5V (5V input), 11 = 14V（12V input）(default)电源引脚的过压保护阈值-----------------非默认

	st_reg07.VDPM_BAT_TRACK = 0; // 0 = 动态电压跟踪（VINDPM）设置disable(defalut)
	st_reg07.BATFET_RST_EN = 1; // 1 = Enable BATFET reset (default)
	st_reg07.BATFET_DLY = 1;    // 1 = Turn off BATFET after tSM_DLY(default)
	st_reg07.JEITA_VSET = 1;    //0 = JEITA设置充电电压为4.1V(default), 1 = JEITA设置充电电压为 st_reg04.VREG
	st_reg07.BATFET_DIS = 0;    //0 = Allow BATFET (Q4) to turn on(default)
	st_reg07.TMR2X_EN = 1;      //1 = 启用半时钟率安全计时器(default)
	st_reg07.IINDET_EN = 1;     //0 = Not in input current limit detection(default)输入电流限制检测----------------非默认

    hl_drv_sgm41513_write_reg(REG00_ADDR, (uint8_t *)&st_reg00);
    hl_drv_sgm41513_write_reg(REG01_ADDR, (uint8_t *)&st_reg01);
    hl_drv_sgm41513_write_reg(REG02_ADDR, (uint8_t *)&st_reg02);
    hl_drv_sgm41513_write_reg(REG03_ADDR, (uint8_t *)&st_reg03);
    hl_drv_sgm41513_write_reg(REG04_ADDR, (uint8_t *)&st_reg04);
    hl_drv_sgm41513_write_reg(REG05_ADDR, (uint8_t *)&st_reg05);
    hl_drv_sgm41513_write_reg(REG06_ADDR, (uint8_t *)&st_reg06);
    hl_drv_sgm41513_write_reg(REG07_ADDR, (uint8_t *)&st_reg07);
}

/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_sgm41513_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size)
{
    uint8_t *reg_val = (uint8_t *)arg;
    if (arg_size != sizeof(char)) {
        LOG_E("[size error], hl_drv_sgm41513_ctrl, ctrl arg need <char> type pointer!\n");
        return SGM41513_ERROR;
    } else if (!sgm41513_init_status) {
        LOG_E("[error] hl_drv_sgm41513_init!\n");
        return SGM41513_ERROR;
    }
    switch (op_cmd) {
        case SEGT_INPUT_CURRENT_LIMIT_VAL:
            set_input_current_limit(*reg_val);
            break;
        case SET_CHARGE_STATUS:
            set_charge_status(*reg_val);
            break;
        case SET_FAST_CHARGE_CURRENT_VAL:
            set_fast_charge_current(*reg_val);
            break;
        case SET_VBUS_FET_SWITCH_VAL:
            set_vbus_fet_enable_status(*reg_val);
            break;
        case SET_BOOST_MODE_CURRENT_LIMIT_VAL:
            set_boost_mode_current(*reg_val);
            break;
        case SET_STOP_CHARGE_CURRENT_VAL:
            set_termination_charge_current(*reg_val); 
            break;
        case SET_PRE_CHARGE_CURRENT_VAL:
            set_pre_charge_current(*reg_val);
            break;
        case SET_RECHARGE_VOL_VAL:
            set_recharge_voltage_threshold(*reg_val);
            break;
        case SET_CHARGE_VOL_LIMIT_VAL:
            set_charge_voltage_limit(*reg_val);  
            break;
        case SET_CHARGE_FULL_DELAY_TIME:
            set_charge_full_delay_time(*reg_val);  
            break;
        case SET_JEITA_CHARGE_CURRENT_VAL:
            set_jeita_charge_current(*reg_val);
            break;
        case SET_SAFT_TIMER_TIME_VAL:
            set_charge_safe_timer_time_val(*reg_val);
            break;
        case SET_THERMAL_REGULATION_MAX_VAL:
            set_thremal_regulation_threshold(*reg_val);  
            break;
        case SET_CHARGEE_TERMINATION_ENABLE:
            set_charge_termination_enable(*reg_val);  
            break;
        case SET_VINDPM_THRESHOLD_VAL:
            set_vindpm_threshold(*reg_val);
            break;
        case SET_BOOST_MODE_VOL_REGULATION:
            set_boost_mode_voltage_regulation_val(*reg_val);
            break;
        case SET_VAC_PIN_OVER_VOL_THRESHOLD_VAL:
            set_vac_pin_over_voltage_protect_thershold(*reg_val);  
            break;
        case SET_JEITA_CHARGE_VOL_VAL:
            set_jeita_charge_voltage(*reg_val);  
            break;
        case SET_BATFET_USE_SWITCH:
            set_battery_fet_enbale(*reg_val);
            break;
        case SET_IINDET_EN:
            set_iindet_en(*reg_val);
            break;
        case REST_ALL_REG_VAL:
            reset_all_reg_val();
            break;
        case GET_SYS_VOL_REGULATION_STATUS:
            *reg_val = get_sys_voltage_regulation_status();
            break;
        case GET_THERMAL_REGULATION_STATUS:
            *reg_val = get_thermal_regulation_status();
            break;
        case GET_INPUT_POWER_STATUS:
            *reg_val = get_input_power_status();
            break;
        case GET_CHARGE_STATUS:
             *reg_val = get_charge_status(); 
            break;
        case GET_VBUS_STATUS:
             *reg_val = get_vbus_status(); 
            break;
        case GET_BATTERY_TEMP_STATUS:
             *reg_val = get_battery_temp_status(); 
            break;
        case GET_BATTERY_ERROR_STATUS:
            *reg_val = get_battery_error_status();
            break;
        case GET_CHARGE_ERROR_STATUS:
            *reg_val = get_charge_error_status();
            break;
        case GET_BOOST_MODE_ERROR_STATUS:
            *reg_val = get_boost_mode_error_status();
            break;
        case GET_WATCHDOG_ERROR_STATUS:
            *reg_val = get_watchdog_error_status();
            break;
        case GET_INPUT_OVER_VOL_STATUS:
            *reg_val = get_input_over_voltage_status();
            break;
        case GET_VINDPM_STATUS:
            *reg_val = get_vindpm_status();
            break;
        case GET_IINDPM_STATUS:
            *reg_val = get_iindpm_status();
            break;
        case GET_VBUS_CONNECT_STATE:
            *reg_val = get_vbus_connect_status();
            break;
        case GET_CHIP_PART_ID:
            *reg_val = get_chip_part_id();
            break;
        case GET_VBUS_IN_DET_STATUS:
            *reg_val = get_vbus_input_detection_status();
            break;
        case PRINTF_REG_VAL:
            hl_drv_sgm41513_printf_reg_val();
            break;
        default:
            break;
    }
    return SGM41513_OK;
}

uint8_t hl_drv_sgm41513_init(void)
{
    uint8_t re_val = 0;
    hl_sgm41513_reg0b_t reg_val;
    if (sgm41513_init_status == true) {
        LOG_E("[error] sgm41513 drv already init!\n");
        return SGM41513_ERROR;
    }
    re_val = hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_2);
    if (SOFT_I2C_API_FUNC_RET_ERR == re_val) {
        sgm41513_init_status = false;
        LOG_E("[error] sgm41513 drv init failed 1 !\n");
        return SGM41513_ERROR;
    }
    hl_drv_sgm41513_read_reg(REG0B_ADDR, (uint8_t *)&reg_val);
    if (reg_val.PN != SGMXXX_PART_ID) {
        sgm41513_init_status = false;
        LOG_E("[error] sgm41513 drv init failed 2 !\n");
        return SGM41513_ERROR;
    }

    _sgm41513_init_set();

    LOG_I("sgm41513 drv init success!\n");
    sgm41513_init_status = true;
    return SGM41513_OK; 
}

uint8_t hl_drv_sgm41513_deinit(void)
{   
    if (sgm41513_init_status == false) {
        LOG_E("[error] sgm41513 drv not init!\n");
        return SGM41513_ERROR;
    }
    hl_hal_soft_i2c_api_deinit(HL_HAL_SOFT_I2C_NUMB_2);
    sgm41513_init_status = false;
    return SGM41513_OK;
}

/*
 * EOF
 */