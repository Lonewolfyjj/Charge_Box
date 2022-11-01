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
        case 0: 
            break;
        case 0: 
            break;
        case 0: 
            break;
        case 0: 
            break;
        default:
            break;
    }

    return HL_MOD_UI_FUNC_OK;

}


/*
 * EOF
 */