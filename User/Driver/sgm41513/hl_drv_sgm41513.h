/**
 * @file hl_drv_sgm41513.h
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
#ifndef _HL_DRV_SGM41513_H_
#define _HL_DRV_SGM41513_H_
/* Includes ------------------------------------------------------------------*/
#include "rtdef.h"

/* typedef -------------------------------------------------------------------*/
// reg00 TYPE PORV:  0001 0111
typedef struct _hl_sgm41513_reg00_t
{
	rt_uint8_t IINDPM		:5;	// Input Current Limit Value (n: 5 bits):= 100 + 100n (mA) Default: 2400mA (10111), not typical
						// IINDPM[4]  	1 = 1600mA
						// IINDPM[3]	1 = 800mA
						// IINDPM[2]	1 = 400mA
						// IINDPM[1]	1 = 200mA
						// IINDPM[0]	1 = 100mA

	rt_uint8_t EN_ICHG_MON:2;	// Enable STAT Pin Function
						// 00 = Enable following chargeing state (default)
						// 01 = Enable following
						// 10 = Disable (float pin)
						// 11 = Disable (float pin)	
						
	rt_uint8_t EN_HIZ		:1;	// Enable HIZ Mode  
						// 0 = Disable (default)
						// 1 = Enable					
} hl_sgm41513_reg00_t;

// reg01 TYPE PORV:  0001 1010
typedef struct _hl_sgm41513_reg01_t
{								
	rt_uint8_t MIN_BAT_SEL:1; // Minimum Battery Voltage for OTG Mode
						// 0 = 2.8V VBAT falling (default)
						// 1 = 2.5V VBAT falling

	rt_uint8_t SYS_MIN	:3; // Minimum System Voltage
						// 000 = 2.6V	001 = 2.8V	 010 = 3V	011 = 3.2V
						// 100 = 3.4V	101 = 3.5V (default)  110 = 3.6V  111 = 3.7V

	rt_uint8_t CHG_CONFIG :1; // Enable Battery Charging
						// 0 = Charge disable
						// 1 = Charge enable (default)

	rt_uint8_t OTG_CONFIG :1; // Enable OTG
						// 0 = OTG disable (default)
						// 1 = OTG enable

	rt_uint8_t WD_RST 	:1; // I2C Watchdog Timer Reset
						// 0 = Normal (default)
						// 1 = Reset
						
	rt_uint8_t PFM_DIS	:1; // Enable PFM Mode
						// 0 = Enable (default)
						// 1 = Disable		
} hl_sgm41513_reg01_t;

// reg02 TYPE PORV:  1011 0100
typedef struct _hl_sgm41513_reg02_t
{
	rt_uint8_t ICHG		:6;	// Fast Charge Current Value (n: 6 bits): = 60n (mA) (n ≤ 50) Default: 1980mA (110100)
						// ICHG[5] 1 = 540mA
						// ICHG[4] 1 = 130mA
						// ICHG[3] 1 = 40mA
						// ICHG[2] 1 = 20mA
						// ICHG[1] 1 = 10mA
						// ICHG[0] 1 = 5mA

	rt_uint8_t Q1_FULLON	:1; // VBUS FET SWITCH (Q1)
						// 0 = Use higher R-DSON if I-INDPM < 700mA (for better accuracy)
						// 1 = Use lower R-DSON always (fully ON for better efficiency)

	rt_uint8_t BOOST_LIM	:1; // Boost Mode Current Limit
						// 0 = 0.5A
						// 1 = 1.2A (default)
} hl_sgm41513_reg02_t;

// reg03 TYPE PORV:  1010 1010
typedef struct _hl_sgm41513_reg03_t
{
	rt_uint8_t ITERM		:4;	// Termination Current Limit (n: 4 bits) Default: 120mA (1010)
						// ITERM[3] 1 = 80mA
						// ITERM[2] 1 = 30mA
						// ITERM[1] 1 = 15mA
						// ITERM[0] 1 = 10mA

	rt_uint8_t IPRECHG	:4; // Pre-Charge Current Limit (n: 4 bits) Default: 120mA (1010)
						// IPRECHG[3] 1 = 80mA
						// IPRECHG[2] 1 = 30mA
						// IPRECHG[1] 1 = 15mA
						// IPRECHG[0] 1 = 10mA
} hl_sgm41513_reg03_t;

// reg04 TYPE PORV:  0101 1000
/* 如果充满电的电池电压低于Vreg - VRECHG设置，则将开始充电循环 */
typedef struct _hl_sgm41513_reg04_t
{
	rt_uint8_t VRECHG		:1;	// Battery Recharge Threshold（电池充电阈值）
						// 0 = 100mV below Vreg[4:0] (default)
						// 1 = 200mV below Vreg[4:0]

	rt_uint8_t TOPOFF_TIMER:2;// Top-Off Timer(检测到终止条件后增加的充电延长时间.)
						// 00 = Disabled (default)如果禁用，一旦满足终止条件，充电将立即终止
						// 01 = 15 minutes
						// 10 = 30 minutes
						// 11 = 45 minutes

	rt_uint8_t VREG		:5; // Charge Voltage Limit (n: 5 bits): Default: 4.208V (01011)
						//	 = 3856 + 32n (mV) if n ≤ 24, n≠15; 
						//	 = 4.352V          if n = 15
						// Vreg[4]	1 = 512mV
						// Vreg[3]	1 = 256mV
						// Vreg[2]	1 = 128mV
						// Vreg[1]	1 = 64mV
						// Vreg[0]	1 = 32mV
} hl_sgm41513_reg04_t;

// reg05 TYPE PORV:  1011 1111
typedef struct _hl_sgm41513_reg05_t
{				
	rt_uint8_t JEITA_ISET :1; // JEITA Charging Current
						// 0 = 50% of I-CHG
						// 1 = 20% of I-CHG (default)

	rt_uint8_t TREG		:1; // Thermal regulation Threshold(热调节阈值)
						// 0 = 80℃
						// 1 = 120℃ (default)

	rt_uint8_t CHG_TIMER	:1; // Charge Safety Timer Setting
						// 0 = 4hrs
						// 1 = 6hrs (default)
						
	rt_uint8_t EN_TIMER	:1; // Charge Safety Timer Enable（当启用时，预充电和快速充电周期将包括在定时中）
						// 0 = Disable
						// 1 = Enable (default)

	rt_uint8_t WATCHDOG	:2; // Watchdog Timer Setting
						// 00 = Disable watchdog timer
						// 01 = 40s (default)
						// 10 = 80s
						// 11 = 160s

	rt_uint8_t ITERM_TIMER	:1; // ITERM Deglitch Timer Setting 
                        // 0 = 200ms (default)
                        // 1 = 16ms


	rt_uint8_t EN_TERM	:1;	// Charging Termination Enable（充电终止启用）
						// 0 = Disable
						// 1 = Enable (default)		
} hl_sgm41513_reg05_t;

// reg06 TYPE PORV: 1110 0110
typedef struct _hl_sgm41513_reg06_t
{
	rt_uint8_t VINDPM	:4;	// VINDPM Threshold (n: 4 bits): Offset: 3.9V + 0.1n (V) Default: 4.5V (0110)
						// VINDPM[3]	1 = 800mV   （输入电压动态电源管理）
						// VINDPM[2]	1 = 400mV
						// VINDPM[1]	1 = 200mV
						// VINDPM[0]	1 = 100mV

	rt_uint8_t BOOSTV:	 2; // Boost Mode Voltage regulation 电压调节
						// 00 = 4.85V
						// 01 = 5.00V
						// 10 = 5.15V (default)
						// 11 = 5.30V

	rt_uint8_t OVP		:2; // VAC Pin OVP Threshold 电源引脚的过压保护阈值
						// 00 = 5.5V
						// 01 = 6.5V (5V input) (default)
						// 10 = 10.5V (9V input)
						// 11 = 14V (12V input)

} hl_sgm41513_reg06_t;

// reg07 TYPE PORV: 0100 1100
typedef struct _hl_sgm41513_reg07_t
{						
	rt_uint8_t VDPM_BAT_TRACK :2; // Dynamic VINDPM Tracking
							// 00 = Disable (VINDPM set by register)
							// 01 = VBAT + 200mV
							// 10 = VBAT + 250mV
							// 11 = VBAT + 300mV

	rt_uint8_t BATFET_RST_EN:1; // Enable BATFET Reset
							// 0 = Disable BATFET reset
							// 1 = Enable BATFET reset (default)

	rt_uint8_t BATFET_DLY	:1; // BATFET Turn Off Delay Control
							// 0 = Turn off BATFET immediately
							// 1 = Turn off BATFET after tSM_DLY(default)

	rt_uint8_t JEITA_VSET	:1; // JEITA Charging Voltage
							// 0 = Set charge voltage to 4.1V (MAX) (default)
							// 1 = Set charge voltage to V-reg	

	rt_uint8_t BATFET_DIS 	:1; // Disable BATFET
							// 0 = Allow BATFET (Q4) to turn on(default)
							// 1 = Turn off BATFET (Q4) after a t-SM_DLY delay time (reg07 D[3])

	rt_uint8_t TMR2X_EN		:1; // Enable Half Clock Rate Safety Timer 启用半时钟率安全计时器
							// 0 = Disable
							// 1 = Safety timer slow down during DPM, JEITA cool, or thermal regulation (default)

	rt_uint8_t IINDET_EN	:1; // Input Current Limit Detection    输入电流限制检测
							// 0 = Not in input current limit detection(default)
							// 1 = Force input current limit detection when VBUS is present
} hl_sgm41513_reg07_t;

// reg08 TYPE PORV: xxxx xxxx
typedef struct _hl_sgm41513_reg08_t
{			
	rt_uint8_t VSYS_STAT    :1; // System Voltage regulation Status 系统电压调节状态
							// 0 = Not in VSYSMIN regulation (VBAT > VSYS_MIN)
							// 1 = In VSYSMIN regulation (VBAT < VSYS_MIN)

	rt_uint8_t THERM_STAT  	:1; // Thermal regulation Status 热调节状态
							// 0 = Not in thermal regulation
							// 1 = In thermal regulation
							
	rt_uint8_t PG_STAT	    :1; // Input Power Status (VBUS in good voltage range and not poor) 输入电源状态
							// 0 = Input power source is not good
							// 1 = Input power source is good
							
	rt_uint8_t CHRG_STAT	:2; // Charging Status 充电状态
							// 00 = Charge disable                              充电禁用
							// 01 = Pre-charge (VBAT < VBATLOW)                 预充电
							// 10 = Fast charging (constant current or voltage) 快速充电（恒定电流或电压）
							// 11 = Charging terminated                         充电终止
							
	rt_uint8_t VBUS_STAT	:3; //VBUS Status register (SGM41513) 
                            // 000 = No input
                            // 001 = USB host SDP (500mA) → PSEL HIGH
                            // 010 = Adapter 2.4A → PSEL LOW            适配器
                            // 111 = OTG
                            // Other values are reserved.
                            // Current limit value is reported in IINDPM[4:0] register							
} hl_sgm41513_charge_status_t;

// reg09 TYPE PORV: xxxx xxxx
typedef struct _hl_sgm41513_reg09_t
{					
	rt_uint8_t NTC_FAULT 		:3; // JEITA Condition Based on Battery NTC Temperature Measurement JEITA条件基于电池NTC温度测量
							// 000 = Normal                 
							// 010 = Warm
							// 011 = Cool (buck mode only)
							// 101 = Cold
							// 110 = Hot
							/* NTC故障位是实时更新，不需要读取来重置 */
	rt_uint8_t BAT_FAULT  	:1; // Battery Fault Status                             电池故障状态
							// 0 = Normal
							// 1 = Battery over-voltage (BATOVP)                    过压
							
	rt_uint8_t CHRG_FAULT	    :2; // Charging Fault Status                        充电故障状态
							// 00 = Normal
							// 01 = Input fault (VAC OVP or VBAT < VVBUS < 3.8V)    输入故障
							// 10 = Thermal shutdown                                热关机
							// 11 = Charge safety timer expired                     充电安全计时器已过期
							
	rt_uint8_t BOOST_FAULT	:1; // Boost Mode Fault Status                          启动模式故障状态
							// 0 = Normal
							// 1 = VBUS overloaded in OTG, or VBUS OVP, or battery voltage too low  过载，或电池电压过低
							//  (any condition that prevents boost starting)
							
	rt_uint8_t WATCHDOG_FAULT :1; // Watchdog Fault Status                          看门狗故障状态
							// 0 = Normal (no fault)
							// 1 = Watchdog timer expired							看门狗计时器已过期
} hl_sgm41513_fault_status_t;

// reg0A TYPE PORV: xxxx xx00
typedef struct _hl_sgm41513_reg0A_t
{						
	rt_uint8_t IINDPM_INT_MASK:1; // IINDPM Event Detection Mask
							// 0 = Allow IINDPM to send INT pulse
							// 1 = Mask IINDPM INT pulse
							
	rt_uint8_t VINDPM_INT_MASK:1; // VINDPM Event Detection Interrupt Mask
							// 0 = Allow VINDPM INT pulse
							// 1 = Mask VINDPM INT pulse
							
	rt_uint8_t ACOV_STAT	:1; // Input Over-Voltage Status (AC adaptor is the input source) 输入过压状态（交流适配器为输入源）
							// 0 = No over-voltage (no ACOV)						无过电压
							// 1 = Over-voltage detected (ACOV)						检测到过电压
							
	rt_uint8_t TOPOFF_ACTIVE :1; // Active Top-Off Timer Counting Status			活动启动计时器计数状态
							// 0 = Top-off timer not counting
							// 1 = Top-off timer counting	
							
	rt_uint8_t Reserved  	:1; // 
	
	rt_uint8_t IINDPM_STAT	:1; // Input Current regulation (Dynamic Power Management)
							// 0 = Not in IINDPM
							// 1 = In IINDPM
							
	rt_uint8_t VINDPM_STAT	:1; // Input Voltage regulation (Dynamic Power Management)
							// 0 = Not in VINDPM
							// 1 = In VINDPM
							
	rt_uint8_t VBUS_GD 		:1; // Good Input Source Detected						检测好的输入源
							// 0 = A good VBUS is not attached
							// 1 = A good VBUS attached								VBUS连接				
} hl_sgm41513_voltage_status_t;

// reg0B TYPE PORV: 0000 x0xx
typedef struct _hl_sgm41513_reg0B_t
{
	rt_uint8_t DEV_REV 		:2; // Revision(版本)	
	
	rt_uint8_t SGMPART  	:1; // NA
	
	rt_uint8_t PN	    	:4; // Part ID 0000 = SGM41513

	rt_uint8_t reg_RST 		:1; // register Reset
							// 0 = No effect (Keep current register settings)
							// 1 = Reset R/W bits of all registers to the default and reset safety timer
							// (It also resets itself to 0 after register reset is completed.)	
} hl_sgm41513_reg0b_t;

typedef struct {
	hl_sgm41513_reg00_t	reg00;
	hl_sgm41513_reg01_t reg01;
	hl_sgm41513_reg02_t	reg02;
	hl_sgm41513_reg03_t reg03;
	hl_sgm41513_reg04_t reg04;
	hl_sgm41513_reg05_t reg05;
	hl_sgm41513_reg06_t reg06;
	hl_sgm41513_reg07_t reg07;
	hl_sgm41513_charge_status_t charge_stu;
	hl_sgm41513_fault_status_t fault_stu;
	hl_sgm41513_voltage_status_t vol_stu;
	hl_sgm41513_reg0b_t reg0b;
}hl_sgm41513_info_t;

typedef enum _hl_drv_sgm41513_charger_status{
	NOT_CHARGER = 0x00,
	CHARGER_RUN,
	CHARGER_FULL,

}hl_drv_sgm41513_charger_status;

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

/* I2C Slave 7bit Address */
#define SGM41513_WRITE_ADDR	0x1A
#define SGM41513_READ_ADDR	0x1B

#define CHARGER_OPEN		1
#define CHARGER_CLOSE		0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_drv_sgm41513_init();
void hl_drv_sgm41513_deinit();
void hl_drv_sgm41513_write_reg(rt_uint8_t w_addr, rt_uint8_t *reg_data);
void hl_drv_sgm41513_read_reg(rt_uint8_t w_addr, rt_uint8_t *reg_data);


#endif
/*
 * EOF
 */