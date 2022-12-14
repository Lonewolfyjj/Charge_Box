 /**
 * @file hl_mod_ui.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-31
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
 * <tr><td>2022-10-31     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _HAL_MOD_UI_H_
#define _HAL_MOD_UI_H_

/* Includes ------------------------------------------------------------------*/

#include "hl_drv_pwm_led.h"
#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_ui_charge_state_e {
    HL_MOD_UI_NO_CHARGING = 0,
    HL_MOD_UI_CHARGING,
    HL_MOD_UI_CHARG_FULL,
    HL_MOD_UI_CHARG_UNKNOW,
}hl_mod_ui_charge_state_e;

typedef enum _hl_mod_ui_fault_state_e {
    HL_MOD_UI_NO_FAULT = 0,
    HL_MOD_UI_FAULT,
}hl_mod_ui_fault_state_e;

typedef enum _hl_mod_ui_upgrade_state_e {
    HL_MOD_UI_NO_UPGRADE = 0,
    HL_MOD_UI_UPGRADING,
    HL_MOD_UI_UPGRADE_OK,
}hl_mod_ui_upgrade_state_e;



/**
 * @brief hl_mod_ui_ctrl函数接口，op的参数类型
 * @date 2022-11-03
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-11-03      <td>yijiujun     <td>新建
 * </table>
 */
typedef enum _hl_mod_ui_ctrl_op_e {
    HL_MOD_UI_CH_MOD1,		    //盒外左边第一颗指示灯呼吸，其余灯灭
    HL_MOD_UI_CH_MOD2,	        //盒外左边第一颗指示灯常亮，第二颗指示灯呼吸，其余灯灭
    HL_MOD_UI_CH_MOD3,	        //盒外左边第一二颗指示灯常亮，第三颗指示灯呼吸，其余灯灭
    HL_MOD_UI_CH_MOD4,	        //盒外左边第一二三颗指示灯常亮，第四颗指示灯呼吸
            
    HL_MOD_UI_FULL_MOD,		    //盒外左边第一二三四颗指示灯常亮
            
    HL_MOD_UI_DIS_CH_MOD1,	    //盒外左边第一颗指示灯常亮，其余灯灭
    HL_MOD_UI_DIS_CH_MOD2,	    //盒外左边第一二颗指示灯常亮，其余灯灭
    HL_MOD_UI_DIS_CH_MOD3,	    //盒外左边第一二三颗指示灯常亮，其余灯灭

    HL_MOD_UI_RED_BREATH,		//红色指示灯呼吸
    HL_MOD_UI_REG_LIGHT,		//红色指示灯常亮

    HL_MOD_UI_ERROR_MOD,		//充电盒外四颗灯同时闪烁
    HL_MOD_UI_CLOSE_MOD,	    //充电盒外四颗灯全灭

    HL_MOD_UI_TX1_BREATH,		//盒内TX1指示灯呼吸
    HL_MOD_UI_TX2_BREATH,	    //盒内TX2指示灯呼吸
    HL_MOD_UI_RX_BREATH,	    //盒内RX指示灯呼吸

    HL_MOD_UI_TX1_LIGHT,		//盒内TX1指示灯常亮
    HL_MOD_UI_TX2_LIGHT,	    //盒内TX2指示灯常亮
    HL_MOD_UI_RX_LIGHT,	        //盒内RX指示灯常亮

    HL_MOD_UI_TX1_DIS_LIGHT,	//盒内TX1指示灯不亮
    HL_MOD_UI_TX2_DIS_LIGHT,	//盒内TX2指示灯不亮
    HL_MOD_UI_RX_DIS_LIGHT,	    //盒内RX指示灯不亮
            
    HL_MOD_UI_ALL_CLOSE_MOD,	//充电盒上所有灯都不亮灯
            
    HL_MOD_UI_LOWPOWER_MOD,		//进入低功耗
    HL_MOD_UI_WAKE_UP_MOD,		//低功耗唤醒

    //设置BOX的充电状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_charge_state_e>
    HL_MOD_UI_SET_BOX_CHARGE_STATE,

    //设置TX1的充电状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_charge_state_e>
    HL_MOD_UI_SET_TX1_CHARGE_STATE,

    //设置TX2的充电状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_charge_state_e>
    HL_MOD_UI_SET_TX2_CHARGE_STATE,

    //设置RX的充电状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_charge_state_e>
    HL_MOD_UI_SET_RX_CHARGE_STATE,

    //设置电池电量值, type of parameter is <uint8_t> pointer
    HL_MOD_UI_SET_BAT_SOC_VAL,

    //设置故障状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_fault_state_e>
    HL_MOD_UI_SET_FAULT_STATE,

    //设置升级状态, type of parameter is <uint8_t> pointer, see <hl_mod_ui_upgrade_state_e>
    HL_MOD_UI_SET_UPGRADE_STATE,

    //设置超时标志位, type of parameter is <RT_NULL> pointer,
    HL_MOD_UI_SET_TIMEOUT_FLAG,

    //清除超时标志位, type of parameter is <RT_NULL> pointer,
    HL_MOD_UI_CLEAR_TIMEOUT_FLAG

}hl_mod_ui_ctrl_op_e;



typedef struct _hl_ui_mod_info_st {
    uint8_t     box_charge_state;
    uint8_t     tx1_charge_state;
    uint8_t     tx2_charge_state;
    uint8_t     rx_charge_state;

    uint8_t     soc_val;
    uint8_t     fault_state;
    uint8_t     upgrade_state;
    uint8_t     timeout_flag;

}hl_ui_mod_info_st;

/* define --------------------------------------------------------------------*/

#define HL_MOD_UI_FUNC_ERR  1
#define HL_MOD_UI_FUNC_OK   0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_ui_init(void *msg_hd);

int hl_mod_ui_start(void);

int hl_mod_ui_stop(void);

int hl_mod_ui_deinit(void);

int hl_mod_ui_ctrl(int op, void *arg, int arg_size);

#endif
/*
 * EOF
 */