/**
 * @file hl_app_main.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-01
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
 * <tr><td>2022-11-01     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_main.h"
#include "hl_app_msg.h"
#include "hl_app_mng.h"

#include "hl_mod_typedef.h"
#include "hl_mod_extcom.h"
#include "hl_mod_pm.h"
#include "hl_mod_ui.h"

#define DBG_SECTION_NAME "app_main"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_main_st
{
    bool init_flag;
    bool start_flag;
} hl_app_main_st;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

static hl_app_main_st _main_app = {
    .init_flag  = false,
    .start_flag = false,
};

/* Private function(only *.c)  -----------------------------------------------*/

static int _mod_init(void)
{
    int                  ret;
    hl_mod_msg_handle_st msg_hd;

    msg_hd.msg_id   = HL_APP_MSG_ID_EXTCOM_MOD;
    msg_hd.msg_send = hl_app_msg_send;

    ret = hl_mod_extcom_init(&msg_hd);
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    msg_hd.msg_id = HL_APP_MSG_ID_PM_MOD;

    ret = hl_mod_pm_init(&msg_hd);
    if (ret == HL_MOD_PM_FUNC_RET_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    msg_hd.msg_id = HL_APP_MSG_ID_UI_MOD;

    ret = hl_mod_ui_init(&msg_hd);
    if (ret == HL_MOD_UI_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    return HL_APP_MAIN_FUNC_OK;
}

static int _mod_start(void)
{
    int ret;

    ret = hl_mod_extcom_start();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_pm_start();
    if (ret == HL_MOD_PM_FUNC_RET_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_ui_start();
    if (ret == HL_MOD_UI_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    return HL_APP_MAIN_FUNC_OK;
}

static int _mod_deinit(void)
{
    int ret;

    ret = hl_mod_extcom_deinit();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_pm_deinit();
    if (ret == HL_MOD_PM_FUNC_RET_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_ui_deinit();
    if (ret == HL_MOD_UI_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    return HL_APP_MAIN_FUNC_OK;
}

static int _mod_stop(void)
{
    int ret;

    ret = hl_mod_extcom_stop();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_pm_stop();
    if (ret == HL_MOD_PM_FUNC_RET_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_mod_ui_stop();
    if (ret == HL_MOD_UI_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    return HL_APP_MAIN_FUNC_OK;
}

/* Exported functions --------------------------------------------------------*/

int hl_app_main_init(void)
{
    int ret;

    if (_main_app.init_flag == true) {
        LOG_E("main app already inited!");
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_app_msg_init();
    if (ret == HL_APP_MSG_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_app_mng_init();
    if (ret == HL_APP_MNG_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = _mod_init();
    if (ret == HL_APP_MAIN_FUNC_REE) {
        return HL_APP_MAIN_FUNC_REE;
    }

    LOG_I("main app init success!");

    _main_app.init_flag = true;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_deinit(void)
{
    int ret;

    if (_main_app.init_flag == false) {
        LOG_E("main app not init yet!");
        return HL_APP_MAIN_FUNC_REE;
    }

    hl_app_main_stop();

    hl_app_msg_deinit();

    ret = _mod_deinit();
    if (ret == HL_APP_MAIN_FUNC_REE) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_app_mng_deinit();
    if (ret == HL_APP_MNG_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    LOG_I("main app deinit success!");

    _main_app.init_flag = false;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_start(void)
{
    rt_err_t rt_err;
    int      ret;

    if (_main_app.init_flag == false) {
        LOG_E("main app not init yet!");
        return HL_APP_MAIN_FUNC_REE;
    }

    if (_main_app.start_flag == true) {
        LOG_W("main app already start!");
        return HL_APP_MAIN_FUNC_OK;
    }

    ret = hl_app_mng_start();
    if (ret == HL_APP_MNG_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = _mod_start();
    if (ret == HL_APP_MAIN_FUNC_REE) {
        return HL_APP_MAIN_FUNC_REE;
    }

    LOG_I("main app start success!");

    _main_app.start_flag = true;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_stop(void)
{
    int ret;

    if (_main_app.init_flag == false) {
        LOG_E("main app not init yet!");
        return HL_APP_MAIN_FUNC_REE;
    }

    if (_main_app.start_flag == false) {
        LOG_W("main app already stop!");
        return HL_APP_MAIN_FUNC_OK;
    }

    ret = _mod_stop();
    if (ret == HL_APP_MAIN_FUNC_REE) {
        return HL_APP_MAIN_FUNC_REE;
    }

    ret = hl_app_mng_stop();
    if (ret == HL_APP_MNG_FUNC_ERR) {
        return HL_APP_MAIN_FUNC_REE;
    }

    LOG_I("main app stop success!");

    _main_app.start_flag = false;

    return HL_APP_MAIN_FUNC_OK;
}

/*
 * EOF
 */