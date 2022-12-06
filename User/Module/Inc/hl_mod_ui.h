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

#include "rtthread.h"
#include "stdint.h"
#include "stdbool.h"

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
    HL_MOD_UI_CH_MOD1,		    //盒外左边第一颗指示灯快闪，其余灯灭
    HL_MOD_UI_CH_MOD2,	        //盒外左边第一颗指示灯常亮，第二颗指示灯快闪，其余灯灭
    HL_MOD_UI_CH_MOD3,	        //盒外左边第一二颗指示灯常亮，第三颗指示灯快闪，其余灯灭
    HL_MOD_UI_CH_MOD4,	        //盒外左边第一二三颗指示灯常亮，第四颗指示灯快闪
            
    HL_MOD_UI_FULL_MOD,		    //盒外左边第一二三四颗指示灯常亮
            
    HL_MOD_UI_DIS_CH_MOD1,	    //盒外左边第一颗指示灯常亮，其余灯灭
    HL_MOD_UI_DIS_CH_MOD2,	    //盒外左边第一二颗指示灯常亮，其余灯灭
    HL_MOD_UI_DIS_CH_MOD3,	    //盒外左边第一二三颗指示灯常亮，其余灯灭
            
    HL_MOD_UI_ERROR_MOD,		//充电盒外四颗灯同时闪烁
    HL_MOD_UI_CLOSE_MOD,	    //充电盒外四颗灯全灭
            
    HL_MOD_UI_SIGN_LIGHT,		//半透明体盖子白色指示灯常亮
    HL_MOD_UI_SIGN_DIS_LIGHT,	//半透明体盖子白色指示灯不亮
            
    HL_MOD_UI_TX1_LIGHT,		//盒内TX1指示灯常亮
    HL_MOD_UI_TX2_LIGHT,	    //盒内TX2指示灯常亮
    HL_MOD_UI_RX_LIGHT,	        //盒内RX指示灯常亮
    HL_MOD_UI_TX1_DIS_LIGHT,	//盒内TX1指示灯不亮
    HL_MOD_UI_TX2_DIS_LIGHT,	//盒内TX2指示灯不亮
    HL_MOD_UI_RX_DIS_LIGHT,	    //盒内RX指示灯不亮
            
    HL_MOD_UI_ALL_CLOSE_MOD,	//充电盒上所有灯都不亮灯
            
    HL_MOD_UI_LOWPOWER_MOD,		//进入低功耗
    HL_MOD_UI_WAKE_UP_MOD		//低功耗唤醒
}hl_mod_ui_ctrl_op_e;


/* define --------------------------------------------------------------------*/

#define HL_MOD_UI_FUNC_ERR  1
#define HL_MOD_UI_FUNC_OK   0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_ui_init(void *msg_hd);
int hl_mod_ui_deinit(void);
int hl_mod_ui_ctrl(int op, void *arg, int arg_size);

#endif
/*
 * EOF
 */