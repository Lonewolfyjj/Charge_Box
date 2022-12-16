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
#include "hl_app_msg.h"
#include "hl_app_task.h"

#define DBG_SECTION_NAME "app_mng"
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_mng_handle_st
{
    bool             init_flag;
    bool             start_flag;
    struct rt_thread mng_thread;
    int              thread_exit_flag;
} hl_app_mng_handle_st;

/* define --------------------------------------------------------------------*/

#define APP_MNG_THREAD_STACK_SIZE 512

/* variables -----------------------------------------------------------------*/

static uint8_t mng_thread_stack[APP_MNG_THREAD_STACK_SIZE] = { 0 };

static hl_app_mng_handle_st _mng_app = {
    .init_flag        = false,
    .start_flag       = false,
    .mng_thread       = { 0 },
    .thread_exit_flag = 0,
};

/* Private function(only *.c)  -----------------------------------------------*/

static void _app_mng_thread_entry(void* arg)
{
    hl_app_msg_st msg;
    int           ret;

    hl_app_task_init();

    while (_mng_app.thread_exit_flag == 0) {
        ret = hl_app_msg_recv(&msg, 0);
        if (ret == HL_APP_MSG_FUNC_OK) {
            hl_app_task_msg_proc(&msg);
        }

        hl_app_task_proc();

        rt_thread_mdelay(10);
    }

    _mng_app.thread_exit_flag = -1;
}

/* Exported functions --------------------------------------------------------*/

int hl_app_mng_init(void)
{
    if (_mng_app.init_flag == true) {
        LOG_E("mng app already inited!");
        return HL_APP_MNG_FUNC_ERR;
    }

    LOG_I("mng app init success!");

    _mng_app.init_flag = true;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_deinit(void)
{
    if (_mng_app.init_flag == false) {
        LOG_E("mng app not init yet!");
        return HL_APP_MNG_FUNC_ERR;
    }

    hl_app_mng_stop();

    LOG_I("mng app deinit success!");

    _mng_app.init_flag = false;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_start(void)
{
    rt_err_t rt_err;

    if (_mng_app.init_flag == false) {
        LOG_E("mng app not init yet!");
        return HL_APP_MNG_FUNC_ERR;
    }

    if (_mng_app.start_flag == true) {
        LOG_W("mng app already start!");
        return HL_APP_MNG_FUNC_OK;
    }

    _mng_app.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_mng_app.mng_thread), "app_mng_thread", _app_mng_thread_entry, RT_NULL,
                            mng_thread_stack, sizeof(mng_thread_stack), 5, 32);
    if (rt_err == RT_ERROR) {
        LOG_E("mng thread init failed");
        return HL_APP_MNG_FUNC_ERR;
    }

    rt_thread_startup(&(_mng_app.mng_thread));

    LOG_I("mng app start success!");

    _mng_app.start_flag = true;

    return HL_APP_MNG_FUNC_OK;
}

int hl_app_mng_stop(void)
{
    if (_mng_app.init_flag == false) {
        LOG_E("mng app not init yet!");
        return HL_APP_MNG_FUNC_ERR;
    }

    if (_mng_app.start_flag == false) {
        LOG_W("mng app already stop!");
        return HL_APP_MNG_FUNC_OK;
    }

    _mng_app.thread_exit_flag = 1;

    LOG_I("wait mng thread exit");

    while (_mng_app.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    LOG_I("mng app stop success!");

    _mng_app.start_flag = false;

    return HL_APP_MNG_FUNC_OK;
}

/*
 * EOF
 */