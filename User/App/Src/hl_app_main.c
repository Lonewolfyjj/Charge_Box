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

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_main_st
{
    bool             init_flag;
    bool             start_flag;
    struct rt_thread main_thread;
    int              thread_exit_flag;
} hl_app_main_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

#define APP_MAIN_THREAD_STACK_SIZE 512

/* variables -----------------------------------------------------------------*/

static hl_app_main_st _main_app = {
    .init_flag        = false,
    .start_flag       = false,
    .main_thread      = { 0 },
    .thread_exit_flag = 0,
};

static uint8_t main_thread_stack[APP_MAIN_THREAD_STACK_SIZE] = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/

static void _app_main_thread_entry(void* arg)
{
    while (_main_app.thread_exit_flag == 0) {

        rt_thread_mdelay(10);
    }

    _main_app.thread_exit_flag = -1;
}

/* Exported functions --------------------------------------------------------*/

int hl_app_main_init(void)
{
    if (_main_app.init_flag == true) {
        DBG_LOG("main app already inited!\n");
        return HL_APP_MAIN_FUNC_REE;
    }

    DBG_LOG("main app init success!\n");

    _main_app.init_flag = true;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_deinit(void)
{
    if (_main_app.init_flag == false) {
        DBG_LOG("main app not init yet!\n");
        return HL_APP_MAIN_FUNC_REE;
    }

    hl_app_main_stop();

    DBG_LOG("main app deinit success!\n");

    _main_app.init_flag = false;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_start(void)
{
    rt_err_t rt_err;

    if (_main_app.init_flag == false) {
        DBG_LOG("main app not init yet!\n");
        return HL_APP_MAIN_FUNC_REE;
    }

    if (_main_app.start_flag == true) {
        DBG_LOG("main app already start!\n");
        return HL_APP_MAIN_FUNC_OK;
    }

    _main_app.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_main_app.main_thread), "hl_app_main_thread", _app_main_thread_entry, RT_NULL,
                            main_thread_stack, sizeof(main_thread_stack), 6, 32);
    if (rt_err == RT_ERROR) {
        DBG_LOG("main thread init failed\n");
        return HL_APP_MAIN_FUNC_REE;
    }

    rt_thread_startup(&(_main_app.main_thread));

    DBG_LOG("main app start success!\n");

    _main_app.start_flag = true;

    return HL_APP_MAIN_FUNC_OK;
}

int hl_app_main_stop(void)
{
    if (_main_app.init_flag == false) {
        DBG_LOG("main app not init yet!\n");
        return HL_APP_MAIN_FUNC_REE;
    }

    if (_main_app.start_flag == false) {
        DBG_LOG("main app already stop!\n");
        return HL_APP_MAIN_FUNC_OK;
    }

    _main_app.thread_exit_flag = 1;

    DBG_LOG("wait main thread exit\n");

    while (_main_app.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    DBG_LOG("main app stop success!\n");

    _main_app.start_flag = false;

    return HL_APP_MAIN_FUNC_OK;
}

/*
 * EOF
 */