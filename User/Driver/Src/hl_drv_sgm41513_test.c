/**
 * @file hl_drv_charger_test.c
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
#include <rtthread.h>
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/



void hl_drv_sgm41513_test()
{
    hl_sgm41513_info_t info;
    rt_uint8_t data = 0;
    hl_drv_sgm41513_init();

    hl_drv_sgm41513_read_reg(REG00_ADDR, (rt_uint8_t *)&info.reg00);
    rt_kprintf("EN_HIZ:%x, EN_ICHG_MON:%x, IINDPM:%x\n", info.reg00.EN_HIZ, info.reg00.EN_ICHG_MON, info.reg00.IINDPM);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG01_ADDR, (rt_uint8_t *)&info.reg01);
    rt_kprintf("PFM_DIS:%x, WD_RST:%x, OTG_CONFIG:%x, CHG_CONFIG:%x, SYS_MIN:%x, MIN_BAT_SEL:%x\n", info.reg01.PFM_DIS, info.reg01.WD_RST, 
                                        info.reg01.OTG_CONFIG, info.reg01.CHG_CONFIG, info.reg01.SYS_MIN, info.reg01.MIN_BAT_SEL);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG02_ADDR, (rt_uint8_t *)&info.reg02);
    rt_kprintf("BOOST_LIM:%x, Q1_FULLON:%x, ICHG:%x\n", info.reg02.BOOST_LIM, info.reg02.Q1_FULLON, info.reg02.ICHG);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG03_ADDR, (rt_uint8_t *)&info.reg03);
    rt_kprintf("IPRECHG:%x, ITERM:%x\n", info.reg03.IPRECHG, info.reg03.ITERM);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG04_ADDR, (rt_uint8_t *)&info.reg04);
    rt_kprintf("VREG:%x, TOPOFF_TIMER:%x, VRECHG:%x\n", info.reg04.VREG, info.reg04.TOPOFF_TIMER, info.reg04.VRECHG);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG05_ADDR, (rt_uint8_t *)&info.reg05);
    rt_kprintf("EN_TERM:%x, ITERM_TIMER:%x, WATCHDOG:%x, EN_TIMER:%x, CHG_TIMER:%x, TREG:%x, JEITA_ISET:%x\n", 
    info.reg05.EN_TERM, info.reg05.ITERM_TIMER, info.reg05.WATCHDOG, info.reg05.EN_TIMER, 
    info.reg05.CHG_TIMER, info.reg05.TREG, info.reg05.JEITA_ISET);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG06_ADDR, (rt_uint8_t *)&info.reg06);
    rt_kprintf("OVP:%x, BOOSTV:%x, VINDPM:%x\n", info.reg06.OVP, info.reg06.BOOSTV, info.reg06.VINDPM);
    rt_kprintf("\n*************************************************************\n");

    hl_drv_sgm41513_read_reg(REG07_ADDR, (rt_uint8_t *)&info.reg07);
    rt_kprintf("IINDET_EN:%x, TMR2X_EN:%x, BATFET_RST_EN:%x, JEITA_VSET:%x, BATFET_DLY:%x, BATFET_RST_EN:%x, VDPM_BAT_TRACK:%x\n",
    info.reg07.IINDET_EN, info.reg07.TMR2X_EN, info.reg07.BATFET_RST_EN, info.reg07.JEITA_VSET, info.reg07.BATFET_DLY,
    info.reg07.BATFET_RST_EN, info.reg07.VDPM_BAT_TRACK);
    rt_kprintf("\n*************************************************************\n");

    while (1) {
        hl_drv_sgm41513_read_reg(REG08_ADDR, (rt_uint8_t *)&info.charge_stu);
        rt_kprintf("VBUS_STAT:%x, CHRG_STAT:%x, PG_STAT:%x, THERM_STAT:%x, VSYS_STAT:%x\n", 
        info.charge_stu.VBUS_STAT, info.charge_stu.CHRG_STAT, info.charge_stu.PG_STAT, info.charge_stu.THERM_STAT, info.charge_stu.VSYS_STAT);
        rt_kprintf("\n*************************************************************\n");

        hl_drv_sgm41513_read_reg(REG09_ADDR, (rt_uint8_t *)&info.fault_stu);
        rt_kprintf("WATCHDOG_FAULT:%x, BOOST_FAULT:%x, CHRG_FAULT:%x, BAT_FAULT:%x, NTC_FAULT:%x\n", 
        info.fault_stu.WATCHDOG_FAULT, info.fault_stu.BOOST_FAULT, info.fault_stu.CHRG_FAULT, info.fault_stu.BAT_FAULT, info.fault_stu.NTC_FAULT);
        rt_kprintf("\n*************************************************************\n");

        hl_drv_sgm41513_read_reg(REG0A_ADDR, (rt_uint8_t *)&info.vol_stu);
        rt_kprintf("VBUS_GD:%x, VINDPM_STAT:%x, IINDPM_STAT:%x, TOPOFF_ACTIVE:%x, ACOV_STAT:%x, VINDPM_INT_MASK:%x, IINDPM_INT_MASK:%x\n",
        info.vol_stu.VBUS_GD, info.vol_stu.VINDPM_STAT, info.vol_stu.IINDPM_STAT, info.vol_stu.TOPOFF_ACTIVE, 
        info.vol_stu.ACOV_STAT, info.vol_stu.VINDPM_INT_MASK, info.vol_stu.IINDPM_INT_MASK);
        rt_kprintf("\n*************************************************************\n");

        hl_drv_sgm41513_read_reg(REG0B_ADDR, (rt_uint8_t *)&info.reg0b);
        rt_kprintf("reg_RST:%x, PN:%x, SGMPART:%x, DEV_REV:%x\n", info.reg0b.reg_RST, info.reg0b.PN, info.reg0b.SGMPART, info.reg0b.DEV_REV);
        rt_kprintf("\n*************************************************************\n");
        

        hl_drv_sgm41513_read_reg(REG08_ADDR, &data);
        rt_kprintf("--usb--bit 7-5 : %d, %d, %d\n", (data >> 7) & 0x1, (data >> 6) & 0x1, (data >> 5) & 0x1);
        rt_kprintf("--usb--bit 4-3 : %d, %d\n", (data >> 4) & 0x01, (data >> 3) & 0x1);
        rt_kprintf("--usb--bit 2 : %d\n", (data >> 2) & 0x01);
        rt_kprintf("--usb--bit 1 : %d\n", (data >> 1) & 0x01);
        rt_kprintf("--usb--bit 0 : %d\n", data & 0x01);
        rt_kprintf("\n*************************************************************\n");
        rt_thread_mdelay(1000);
    }
    
}

MSH_CMD_EXPORT(hl_drv_sgm41513_test, sgm41513 driver test);
/*
 * EOF
 */