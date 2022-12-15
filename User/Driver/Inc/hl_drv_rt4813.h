/**
 * @file hl_drv_rt4813.h
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

#ifndef __HL_DRV_RT4813_H__
#define __HL_DRV_RT4813_H__

/* Includes ------------------------------------------------------------------*/

#include "stdint.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_rt4813_reg01_t
{
    uint8_t SSFM 		:1; // Spread spectrum setting.(扩频设置)
                        // O : 扩频禁用(默认)
                        // 1 : 扩频启用
    
    uint8_t DRV_SEL	    :3; // LG dirver driving capability:LG(Gate loop inductance栅极回路电感)驱动的驱动能力
                        //000 : Slowest
                        // |
                        //111 : Fastest (default)
	
	uint8_t IPCHG  	    :2; // Pre-charge current setting.（预充电电流设置）
                        // 00 : 0.5A
                        // 01 : 1A (default)
                        // 10 : 1.5A
                        // 11 : 2A

	uint8_t ILIM_OFF    :1; // Boost valley current limit setting.(升压电流限制设置)
                        // 0 : Boost current limit enable (default)
                        // 1 : Boost current limit disable
	
    uint8_t Reserved    :1;                    
	
} hl_rt4813_reg01_t;


typedef struct _hl_rt4813_reg02_t
{
    uint8_t ILIM_AVG 	:4; // Soft-start period boost current limit setting.(软启动周期升压电流限制设置)
                        // 0000 : 1500mA(默认)
                        //   |
                        // 1111 : 6000mA
    
    uint8_t ILIM_SS	    :4; // Average Output Current limit setting.(平均输出电流限制设置)
                        // 0000 : 3100mA(默认)
                        //   |
                        // 1111 : 550mA                  
	
} hl_rt4813_reg02_t;


typedef struct _hl_rt4813_reg03_t
{
    uint8_t EN_Discharge 	:1; // Enable discharge.(启用放电功能)
                            // 0 : Disable
                            // 1 : Enable(默认)
    
    uint8_t EN_IAVGCL	    :1; // Enable average output current limit.(启用平均输出电流限制)
                            // 0 : Disable
                            // 1 : Enable(默认) 

    uint8_t FSW	            :2; // Boost frequency setting.(升压频率设置)
                            // 00 : 2MHz
                            // 01 : Do not allowed
                            // 10 : 1MHz
                            // 11 : 500kHz (默认) 
    	
    uint8_t Reserved        :4;              
	
} hl_rt4813_reg03_t;

typedef enum _hl_drv_rt4813_ctrl_op_e {
    //设置预充电的电流值, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_IPCHG_CTRL = 1,
    //启用升压电流限制, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_EN_ILMI_CTRL,
    //获取LG的驱动能力值, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_LG_DRV_ABILITY_GET, 

    //设置软启动周期升压电流的限制值, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_ILIM_AVG_CTRL,
    //设置平均输出电流的限制值, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_ILIM_SS_CTRL,

    //启用放电功能, type of parameter is <uint8_t> pointer
	HL_DRV_RT4813_EN_DISCHARGE_CTRL,
    //启用平均输出电流限制, type of parameter is <uint8_t> pointer
    HL_DRV_RT4813_EN_IAVGCL_CTRL

}hl_drv_rt4813_ctrl_op_e;

/* define --------------------------------------------------------------------*/

/* I2C Slave 7bit Address */
#define RT4813_WRITE_ADDR	0x39
#define RT4813_READ_ADDR	0x3A

#define RT4813_LG_DRV_ABILITY   0x07

#define RT4813_FUNC_RET_ERR     1
#define RT4813_FUNC_RET_OK      0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

uint8_t hl_drv_rt4813_init(void);

uint8_t hl_drv_rt4813_deinit(void);

uint8_t hl_drv_rt4813_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size);

#endif
/*
 * EOF
 */