/**
 * @file hl_app_msg.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-11-02
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
 * <tr><td>2022-11-02     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_app_msg.h"

#define DBG_SECTION_NAME "app_msg"
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_app_msg_handle_st
{
    bool                   init_flag;
    struct rt_messagequeue msg_queue;
} hl_app_msg_handle_st;

/* define --------------------------------------------------------------------*/

#define MAX_MSG_NUM 20

/* variables -----------------------------------------------------------------*/

static hl_app_msg_handle_st _msg_app = {
    .init_flag = false,
    .msg_queue = { 0 },
};

static hl_app_msg_st _msg_pool[MAX_MSG_NUM] = { 0 };

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_app_msg_init(void)
{
    rt_err_t rt_err;

    if (_msg_app.init_flag == true) {
        LOG_E("msg already inited!");
        return HL_APP_MSG_FUNC_ERR;
    }

    rt_err = rt_mq_init(&(_msg_app.msg_queue), "app_msg_queue", (void*)_msg_pool, sizeof(hl_app_msg_st),
                        sizeof(_msg_pool), RT_IPC_FLAG_PRIO);
    if (rt_err != RT_EOK) {
        LOG_E("init message queue failed.");
        return -1;
    }

    LOG_I("msg init success!");

    _msg_app.init_flag = true;

    return HL_APP_MSG_FUNC_OK;
}

int hl_app_msg_deinit(void)
{
    if (_msg_app.init_flag == false) {
        LOG_E("msg not init yet!");
        return HL_APP_MSG_FUNC_ERR;
    }

    rt_mq_detach(&(_msg_app.msg_queue));

    LOG_I("msg deinit success!");

    _msg_app.init_flag = false;

    return HL_APP_MSG_FUNC_OK;
}

int hl_app_msg_send(int msg_id, uint8_t cmd, void* param, uint16_t len)
{
    rt_err_t      rt_err;
    hl_app_msg_st msg;

    if (_msg_app.init_flag == false) {
        LOG_E("msg not init yet!");
        return HL_APP_MSG_FUNC_ERR;
    }

    msg.msg_id = msg_id;
    msg.cmd    = cmd;
    msg.param  = param;
    msg.len    = len;

    rt_err = rt_mq_send(&(_msg_app.msg_queue), &msg, sizeof(msg));
    if (rt_err != RT_EOK) {
        return HL_APP_MSG_FUNC_ERR;
    }

    return HL_APP_MSG_FUNC_OK;
}

int hl_app_msg_recv(hl_app_msg_st* msg, int timeout)
{
    rt_err_t rt_err;

    if (msg == RT_NULL) {
        return HL_APP_MSG_FUNC_ERR;
    }

    if (_msg_app.init_flag == false) {
        LOG_E("msg not init yet!");
        return HL_APP_MSG_FUNC_ERR;
    }

    rt_err = rt_mq_recv(&(_msg_app.msg_queue), (void*)msg, sizeof(hl_app_msg_st), timeout);
    if (rt_err != RT_EOK) {
        return HL_APP_MSG_FUNC_ERR;
    }

    return HL_APP_MSG_FUNC_OK;
}

/*
 * EOF
 */