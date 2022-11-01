/**
 * @file hl_mod_ui.c
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
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_ui.h"
#include "hl_drv_aw2016a.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_mod_ui_st {
    bool                     init_flag;
    void*                    msg_hd;
} hl_mod_ui_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG     rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_mod_ui_st _ui_info = {
    .init_flag  = false,
    .msg_hd     = RT_NULL
};

/* Private function(only *.c)  -----------------------------------------------*/

static void _battery_soc_state_display(hl_mod_ui_charge_soc_st *soc_st)
{
    if (soc_st->charge_state == HL_MOD_UI_CHARGING) {
        switch (soc_st->soc_level) {
            case HL_MOD_UI_SOC_LEVEL_0:

                break;
            case HL_MOD_UI_SOC_LEVEL_1:
    
                break;
            case HL_MOD_UI_SOC_LEVEL_2: 

                break;
            case HL_MOD_UI_SOC_LEVEL_3: 

                break;
            default:
                break; 
        }
    } else {
        switch (soc_st->soc_level) {
            case HL_MOD_UI_SOC_LEVEL_0:

                break;
            case HL_MOD_UI_SOC_LEVEL_1:
    
                break;
            case HL_MOD_UI_SOC_LEVEL_2: 

                break;
            case HL_MOD_UI_SOC_LEVEL_3: 

                break;
            default:
                break;            
        }
    }
}

static void _upgrading_state_display(uint8_t *up_st)
{
    switch (*up_st) {
        case HL_MOD_UI_UPGRADING_READY:

            break;
        case HL_MOD_UI_UPGRADING_RUN:
 
            break;
        case HL_MOD_UI_UPGRADING_OK: 

            break;
        default:
            break;   
    }
}

static void _error_state_display(uint8_t *err_st)
{
    switch (*err_st) {
        case HL_MOD_UI_GUAGE_ERR:

            break;
        case HL_MOD_UI_CHARGER_ERR:
 
            break;
        case HL_MOD_UI_RTC_ERR: 

            break;
        case HL_MOD_UI_FLASH_ERR: 

            break;
        default:
            break;   
    }
}



/* Exported functions --------------------------------------------------------*/

int hl_mod_ui_init(void *msg_hd)
{
    if (_ui_info.init_flag = true) {
        DBG_LOG("ui mod already inited!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    hl_drv_aw2016a_init();

    DBG_LOG("ui mod init success\n");

    _ui_info.msg_hd = msg_hd;
    _ui_info.init_flag = true;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_deinit(void)
{
    if (_ui_info.init_flag = false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }

    hl_drv_aw2016a_deinit();

    DBG_LOG("ui mod init success\n");

    _ui_info.msg_hd = RT_NULL;
    _ui_info.init_flag = false;
    return HL_MOD_UI_FUNC_OK;
}

int hl_mod_ui_ctrl(int op, void *arg, int arg_size)
{
    if (_ui_info.init_flag = false) {
        DBG_LOG("ui mod not init!\n");
        return HL_MOD_UI_FUNC_ERR;
    }
    
    switch (op) {
        case HL_MOD_UI_BATTERY_PAGE:
            if (arg_size != sizeof(hl_mod_ui_charge_soc_st)) {
                DBG_LOG("size err, ctrl arg need <hl_mod_ui_charge_soc_st> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            }
            _battery_soc_state_display((hl_mod_ui_charge_soc_st *)arg);
            break;
        case HL_MOD_UI_UPGRADE_PAGE:
            if (arg_size != sizeof(uint8_t)) {
                DBG_LOG("size err, ctrl arg need <uint8_t> type pointer!\n");
                return HL_MOD_UI_FUNC_ERR;
            }
            _upgrading_state_display((uint8_t *)arg); 
            break;
        case HL_MOD_UI_ERROR_PAGE: 
            _error_state_display((uint8_t *)arg);
            break;
        default:
            DBG_LOG("op err, hl_mod_ui_ctrl!\n");
            break;
    }

    return HL_MOD_UI_FUNC_OK;

}


/*
 * EOF
 */