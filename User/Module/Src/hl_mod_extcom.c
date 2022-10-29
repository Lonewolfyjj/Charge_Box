/**
 * @file hl_mod_extcom.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-29
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
 * <tr><td>2022-10-29     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_extcom.h"
#include "hl_hal_uart.h"
#include "hl_hal_usb_cdc.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_mod_extcom_st
{
    bool  init_flag;
    bool  start_flag;
    void* msg_hd;
} hl_mod_extcom_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

/* variables -----------------------------------------------------------------*/

static hl_mod_extcom_st _extcom_mod = {
    .init_flag  = false,
    .start_flag = false,
    .msg_hd     = RT_NULL,
};

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_extcom_init(void* msg_hd)
{
    if (_extcom_mod.init_flag == true) {
        DBG_LOG("extcom mod already inited!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    DBG_LOG("extcom mod init success\n");

    _extcom_mod.msg_hd    = msg_hd;
    _extcom_mod.init_flag = true;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_deinit(void)
{
    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    hl_mod_extcom_stop();

    DBG_LOG("extcom mod deinit success\n");

    _extcom_mod.init_flag = false;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_start(void)
{
    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (_extcom_mod.start_flag == true) {
        DBG_LOG("extcom mod already start!\n");
        return HL_MOD_EXTCOM_FUNC_OK;
    }

    DBG_LOG("extcom mod start success!\n");

    _extcom_mod.start_flag = true;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_stop(void)
{
    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (_extcom_mod.start_flag == false) {
        DBG_LOG("extcom mod not start!\n");
        return HL_MOD_EXTCOM_FUNC_OK;
    }

    DBG_LOG("extcom mod stop success!\n");

    _extcom_mod.start_flag = false;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_ctrl(int op, void* arg, int arg_size)
{
    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

/*
 * EOF
 */