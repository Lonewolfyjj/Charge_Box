/**
 * @file hl_app_mng.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-03
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
 * <tr><td>2022-11-03     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_mng.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_mng_handle_st
{
    bool             init_flag;
    bool             start_flag;
    struct rt_thread mng_thread;
    int              thread_exit_flag;
} hl_app_mng_handle_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

#define APP_MNG_THREAD_STACK_SIZE 512

/* variables -----------------------------------------------------------------*/

static uint8_t mng_thread_stack[APP_MNG_THREAD_STACK_SIZE] = { 0 };

static hl_app_mng_handle_st _mng_app = {
    .init_flag        = false,
    .start_flag       = false,
    .mng_thread       = { 0 },
    .thread_exit_flag = 0,
};

static hl_app_mng_st _mng_st = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/

static void _msg_proc(hl_app_msg_st* msg)
{
    DBG_LOG("mng app recv msg: id %d, cmd %d\n", msg->msg_id, msg->cmd);
    
    hl_app_mng_charger_proc(msg);
    hl_app_mng_ui_proc(msg);
    hl_app_mng_extcom_proc(msg);
    hl_app_mng_upgrade_proc(msg);
}

static void _app_mng_thread_entry(void* arg)
{
    hl_app_msg_st msg;
    int           ret;

    while (_mng_app.thread_exit_flag == 0) {
        ret = hl_app_msg_recv(&msg, 0);
        if (ret == HL_APP_MSG_FUNC_ERR) {
            rt_thread_mdelay(10);
            continue;
        }

        _msg_proc(&msg);
    }

    _mng_app.thread_exit_flag = -1;
}

/* Exported functions --------------------------------------------------------*/

int hl_app_mng_init(void)
{
    if (_mng_app.init_flag == true) {
        DBG_LOG("mng app already inited!\n");
        return HL_APP_MNG_FUNC_ERR;
    }

    DBG_LOG("mng app init success!\n");

    _mng_app.init_flag = true;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_deinit(void)
{
    if (_mng_app.init_flag == false) {
        DBG_LOG("mng app not init yet!\n");
        return HL_APP_MNG_FUNC_ERR;
    }

    hl_app_mng_stop();

    DBG_LOG("mng app deinit success!\n");

    _mng_app.init_flag = false;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_start(void)
{
    rt_err_t rt_err;

    if (_mng_app.init_flag == false) {
        DBG_LOG("mng app not init yet!\n");
        return HL_APP_MNG_FUNC_ERR;
    }

    if (_mng_app.start_flag == true) {
        DBG_LOG("mng app already start!\n");
        return HL_APP_MNG_FUNC_OK;
    }

    _mng_app.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_mng_app.mng_thread), "hl_app_mng_thread", _app_mng_thread_entry, RT_NULL,
                            mng_thread_stack, sizeof(mng_thread_stack), 6, 32);
    if (rt_err == RT_ERROR) {
        DBG_LOG("mng thread init failed\n");
        return HL_APP_MNG_FUNC_ERR;
    }

    rt_thread_startup(&(_mng_app.mng_thread));

    DBG_LOG("mng app start success!\n");

    _mng_app.start_flag = true;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_stop(void)
{
    if (_mng_app.init_flag == false) {
        DBG_LOG("mng app not init yet!\n");
        return HL_APP_MNG_FUNC_ERR;
    }

    if (_mng_app.start_flag == false) {
        DBG_LOG("mng app already stop!\n");
        return HL_APP_MNG_FUNC_OK;
    }

    _mng_app.thread_exit_flag = 1;

    DBG_LOG("wait mng thread exit\n");

    while (_mng_app.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    DBG_LOG("mng app stop success!\n");

    _mng_app.start_flag = false;

    return HL_APP_MNG_FUNC_OK;
}

hl_app_mng_st* hl_app_mng_get(void)
{
    return &_mng_st;
}

/*
 * EOF
 */