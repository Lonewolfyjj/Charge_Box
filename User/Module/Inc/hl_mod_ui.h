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

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_ui_page_list {
    HL_MOD_UI_BATTERY_PAGE,
    HL_MOD_UI_UPGRADE_PAGE,
    HL_MOD_UI_ERROR_PAGE,
}hl_mod_ui_op_cmd_e;
typedef enum _hl_mod_ui_state_list {
    HL_MOD_UI_CHARGING,
    HL_MOD_UI_OUT_CHARGE,
    HL_MOD_UI_SOC_LEVEL_0,
    HL_MOD_UI_SOC_LEVEL_1,
    HL_MOD_UI_SOC_LEVEL_2,
    HL_MOD_UI_SOC_LEVEL_3,

    HL_MOD_UI_UPGRADING_READY,
    HL_MOD_UI_UPGRADING_RUN,
    HL_MOD_UI_UPGRADING_OK,

    HL_MOD_UI_GUAGE_ERR,
    HL_MOD_UI_CHARGER_ERR,
    HL_MOD_UI_RTC_ERR,
    HL_MOD_UI_FLASH_ERR
}hl_mod_ui_state_list_e;

typedef struct {
    uint8_t charge_state;
    uint8_t soc_level;
}hl_mod_ui_charge_soc_st;

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