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
#include "hl_util_hup.h"
#include "hl_util_fifo.h"
#include "hl_hal_usb_cdc.h"
#include "hl_hal_console.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_mod_extcom_object_st
{
    hl_util_hup_t      hup;
    hl_util_fifo_t     fifo;
    struct rt_timer    timer;
    bool               timeout_flag;
    hl_hal_uart_numb_e uart_num;
} hl_mod_extcom_object_st;

typedef struct _hl_mod_extcom_st
{
    bool                     init_flag;
    bool                     start_flag;
    bool                     use_usb_cdc_flag;
    void*                    msg_hd;
    struct rt_thread         extcom_thread;
    int                      thread_exit_flag;
    hl_mod_extcom_object_st* p_objects;
} hl_mod_extcom_st;

/* define --------------------------------------------------------------------*/

#define DBG_LOG rt_kprintf

#define HL_MOD_EXTCOM_HUP_RX_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_TX1_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_TX2_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_BOX_BUFSZ 256

#define HL_MOD_EXTCOM_FIFO_RX_BUFSZ 256
#define HL_MOD_EXTCOM_FIFO_TX1_BUFSZ 256
#define HL_MOD_EXTCOM_FIFO_TX2_BUFSZ 256

#define EXTCOM_THREAD_STACK_SIZE 512

#define HUP_MAX_READ_BUFSZ 256
#define HUP_MAX_SEND_BUFSZ 256

#define HUP_PROT_SIZE 6

/* variables -----------------------------------------------------------------*/

static uint8_t hup_buf_rx[HL_MOD_EXTCOM_HUP_RX_BUFSZ]   = { 0 };
static uint8_t hup_buf_tx1[HL_MOD_EXTCOM_HUP_TX1_BUFSZ] = { 0 };
static uint8_t hup_buf_tx2[HL_MOD_EXTCOM_HUP_TX2_BUFSZ] = { 0 };
static uint8_t hup_buf_box[HL_MOD_EXTCOM_HUP_BOX_BUFSZ] = { 0 };

static uint8_t fifo_buf_rx[HL_MOD_EXTCOM_FIFO_RX_BUFSZ]   = { 0 };
static uint8_t fifo_buf_tx1[HL_MOD_EXTCOM_FIFO_TX1_BUFSZ] = { 0 };
static uint8_t fifo_buf_tx2[HL_MOD_EXTCOM_FIFO_TX2_BUFSZ] = { 0 };

static hl_mod_extcom_object_st objects[NUM_OF_HL_MOD_EXTCOM_OBJECT] = { 0 };

static uint8_t extcom_thread_stack[EXTCOM_THREAD_STACK_SIZE] = { 0 };

static hl_mod_extcom_st _extcom_mod = {
    .init_flag        = false,
    .start_flag       = false,
    .use_usb_cdc_flag = false,
    .msg_hd           = RT_NULL,
    .extcom_thread    = RT_NULL,
    .thread_exit_flag = 0,
    .p_objects        = RT_NULL,
};

/* Private function(only *.c)  -----------------------------------------------*/

static int _object_send_data(hl_mod_extcom_object_e object, char cmd, char* buf, int len)
{
    if (len + HUP_PROT_SIZE > HUP_MAX_SEND_BUFSZ) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    int      size;
    uint32_t write_size;
    uint8_t  buf_send[HUP_MAX_SEND_BUFSZ] = { 0 };

    size = hl_util_hup_encode(objects[object].hup.hup_handle.role, cmd, buf_send, sizeof(buf_send), buf, len);
    if (size == -1) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (object == HL_MOD_EXTCOM_OBJECT_BOX) {
        if (_extcom_mod.use_usb_cdc_flag == true) {
            hl_hal_usb_cdc_write(buf_send, size, &write_size);
        } else {
            return HL_MOD_EXTCOM_FUNC_ERR;
        }
    } else {
        hl_hal_uart_send(objects[object].uart_num, buf_send, size);
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

static void rx_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    switch (hup_frame.cmd) {
        default:
            break;
    }

    _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, hup_frame.cmd, RT_NULL, 0);
}

static void tx1_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    switch (hup_frame.cmd) {
        default:
            break;
    }

    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, hup_frame.cmd, RT_NULL, 0);
}

static void tx2_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    switch (hup_frame.cmd) {
        default:
            break;
    }

    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, hup_frame.cmd, RT_NULL, 0);
}

static void box_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    switch (hup_frame.cmd) {
        default:
            break;
    }

    _object_send_data(HL_MOD_EXTCOM_OBJECT_BOX, hup_frame.cmd, RT_NULL, 0);
}

static int _hup_init(void)
{
    int ret;

    objects[HL_MOD_EXTCOM_OBJECT_RX].hup.hup_handle.frame_data_len = sizeof(hup_buf_rx);
    objects[HL_MOD_EXTCOM_OBJECT_RX].hup.hup_handle.role           = EM_HUP_ROLE_MASTER;
    objects[HL_MOD_EXTCOM_OBJECT_RX].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_RX].hup), hup_buf_rx, RT_NULL, rx_hup_success_handle_func);
    if (ret == -1) {
        DBG_LOG("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.frame_data_len = sizeof(hup_buf_tx1);
    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.role           = EM_HUP_ROLE_MASTER;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].hup), hup_buf_tx1, RT_NULL, tx1_hup_success_handle_func);
    if (ret == -1) {
        DBG_LOG("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.frame_data_len = sizeof(hup_buf_tx2);
    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.role           = EM_HUP_ROLE_MASTER;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].hup), hup_buf_tx2, RT_NULL, tx2_hup_success_handle_func);
    if (ret == -1) {
        DBG_LOG("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.frame_data_len = sizeof(hup_buf_box);
    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.role           = EM_HUP_ROLE_SLAVE;
    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_BOX].hup), hup_buf_box, RT_NULL, box_hup_success_handle_func);
    if (ret == -1) {
        DBG_LOG("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

static int _fifo_init(void)
{
    hl_util_fifo_init(&(objects[HL_MOD_EXTCOM_OBJECT_RX].fifo), fifo_buf_rx, sizeof(fifo_buf_rx));
    hl_util_fifo_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].fifo), fifo_buf_tx1, sizeof(fifo_buf_tx1));
    hl_util_fifo_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].fifo), fifo_buf_tx2, sizeof(fifo_buf_tx2));

    return HL_MOD_EXTCOM_FUNC_OK;
}

static void _rx_uart_callback(uint8_t data)
{
    if (_extcom_mod.start_flag == false) {
        return;
    }

    hl_util_fifo_write(&(objects[HL_MOD_EXTCOM_OBJECT_RX].fifo), &data, 1);
}

static void _tx1_uart_callback(uint8_t data)
{
    if (_extcom_mod.start_flag == false) {
        return;
    }

    hl_util_fifo_write(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].fifo), &data, 1);
}

static void _tx2_uart_callback(uint8_t data)
{
    if (_extcom_mod.start_flag == false) {
        return;
    }

    hl_util_fifo_write(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].fifo), &data, 1);
}

static int _uart_init(void)
{
    int ret;

    objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num  = HL_HAL_UART_NUMB_2;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num = HL_HAL_UART_NUMB_3;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num = HL_HAL_UART_NUMB_1;

    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num, 115200);
    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num, 115200);
    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num, 115200);

    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num, _rx_uart_callback);
    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num, _tx1_uart_callback);
    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num, _tx2_uart_callback);

    return HL_HAL_USB_CDC_FUNC_OK;
}

static void object_timer_timeout_handle(void* arg)
{
    hl_mod_extcom_object_st* p_object = (hl_mod_extcom_object_st*)arg;

    p_object->timeout_flag = true;

    DBG_LOG("object %p timeout!\n", p_object);
}

static int _timer_init(void)
{
    rt_timer_init(&(objects[HL_MOD_EXTCOM_OBJECT_RX].timer), "object_rx_timer", object_timer_timeout_handle,
                  &(objects[HL_MOD_EXTCOM_OBJECT_RX]), 1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

    rt_timer_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].timer), "object_tx1_timer", object_timer_timeout_handle,
                  &(objects[HL_MOD_EXTCOM_OBJECT_TX1]), 1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

    rt_timer_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].timer), "object_tx2_timer", object_timer_timeout_handle,
                  &(objects[HL_MOD_EXTCOM_OBJECT_TX2]), 1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

    rt_timer_init(&(objects[HL_MOD_EXTCOM_OBJECT_BOX].timer), "object_box_timer", object_timer_timeout_handle,
                  &(objects[HL_MOD_EXTCOM_OBJECT_BOX]), 1000, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);

    objects[HL_MOD_EXTCOM_OBJECT_RX].timeout_flag  = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].timeout_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].timeout_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_BOX].timeout_flag = false;

    return HL_HAL_USB_CDC_FUNC_OK;
}

static int _timer_start(hl_mod_extcom_object_e object, rt_tick_t timeout)
{
    rt_timer_control(&(objects[object].timer), RT_TIMER_CTRL_SET_TIME, &timeout);

    rt_timer_start(&(objects[object].timer));
}

static int _timer_stop(hl_mod_extcom_object_e object)
{
    rt_timer_stop(&(objects[object].timer));
}

static bool _timer_timeout_check(hl_mod_extcom_object_e object)
{
    if (objects[object].timeout_flag == true) {
        objects[object].timeout_flag = false;
        return true;
    } else {
        return false;
    }
}

static int _objects_init(void)
{
    int ret;

    _extcom_mod.p_objects = objects;

    ret = _hup_init();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    ret = _fifo_init();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    ret = _uart_init();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    ret = _timer_init();
    if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

static void _rx_uart_msg_poll(void)
{
    uint8_t buf[HUP_MAX_READ_BUFSZ];
    int     size;

    size = hl_util_fifo_read(&(objects[HL_MOD_EXTCOM_OBJECT_RX].fifo), buf, sizeof(buf));
    if (size <= 0) {
        return;
    }

    for (int i = 0; i < size; i++) {
        hl_util_hup_decode(&(objects[HL_MOD_EXTCOM_OBJECT_RX].hup), buf[i]);
    }
}

static void _tx1_uart_msg_poll(void)
{
    uint8_t buf[HUP_MAX_READ_BUFSZ];
    int     size;

    size = hl_util_fifo_read(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].fifo), buf, sizeof(buf));
    if (size <= 0) {
        return;
    }

    for (int i = 0; i < size; i++) {
        hl_util_hup_decode(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].hup), buf[i]);
    }
}

static void _tx2_uart_msg_poll(void)
{
    uint8_t buf[HUP_MAX_READ_BUFSZ];
    int     size;

    size = hl_util_fifo_read(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].fifo), buf, sizeof(buf));
    if (size <= 0) {
        return;
    }

    for (int i = 0; i < size; i++) {
        hl_util_hup_decode(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].hup), buf[i]);
    }
}

static void _box_uart_msg_poll(void)
{
    if (_extcom_mod.use_usb_cdc_flag == false) {
        return;
    }

    uint8_t buf[HUP_MAX_READ_BUFSZ];
    int     size;

    hl_hal_usb_cdc_read(buf, sizeof(buf), &size);

    for (int i = 0; i < size; i++) {
        hl_util_hup_decode(&(objects[HL_MOD_EXTCOM_OBJECT_BOX].hup), buf[i]);
    }
}

static void _extcom_thread_entry(void* arg)
{
    while (_extcom_mod.thread_exit_flag == 0) {
        _rx_uart_msg_poll();
        _tx1_uart_msg_poll();
        _tx2_uart_msg_poll();
        _box_uart_msg_poll();

        rt_thread_mdelay(10);
    }

    _extcom_mod.thread_exit_flag = -1;
}

static int _handle_op_enter_upgrade(bool* flag)
{
    if (flag == RT_NULL) {
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    hl_hal_console_detach(*flag);
    _extcom_mod.use_usb_cdc_flag = *flag;

    return HL_MOD_EXTCOM_FUNC_OK;
}

/* Exported functions --------------------------------------------------------*/

int hl_mod_extcom_init(void* msg_hd)
{
    if (_extcom_mod.init_flag == true) {
        DBG_LOG("extcom mod already inited!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    _objects_init();

    _extcom_mod.use_usb_cdc_flag = false;
    hl_hal_console_detach(false);

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
    rt_err_t rt_err;

    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (_extcom_mod.start_flag == true) {
        DBG_LOG("extcom mod already start!\n");
        return HL_MOD_EXTCOM_FUNC_OK;
    }

    _extcom_mod.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_extcom_mod.extcom_thread), "hl_mod_extcom_thread", _extcom_thread_entry, RT_NULL,
                            extcom_thread_stack, sizeof(extcom_thread_stack), 6, 32);
    if (rt_err == RT_ERROR) {
        DBG_LOG("extcom thread init failed\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    rt_thread_startup(&(_extcom_mod.extcom_thread));

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

    _extcom_mod.thread_exit_flag = 1;

    DBG_LOG("wait extcom thread exit\n");

    while (_extcom_mod.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    DBG_LOG("extcom mod stop success!\n");

    _extcom_mod.start_flag = false;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_ctrl(hl_mod_extcom_op_e op, void* arg, int arg_size)
{
    int ret;

    if (_extcom_mod.init_flag == false) {
        DBG_LOG("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    switch (op) {
        case HL_MOD_EXTCOM_ENTER_UPGRADE: {
            if (arg_size != sizeof(bool)) {
                DBG_LOG("size err, ctrl arg need <bool> type pointer!\n");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            ret = _handle_op_enter_upgrade((bool*)arg);
            if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
                return HL_MOD_EXTCOM_FUNC_ERR;
            }
        } break;

        default:
            break;
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

/*
 * EOF
 */