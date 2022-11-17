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

#include "hl_mod_typedef.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_extcom_hup_cmd_e
{
    HL_HUP_CMD_PROBE         = 0x01,
    HL_HUP_CMD_GET_BAT_INFO  = 0x02,
    HL_HUP_CMD_GET_PAIR_INFO = 0x05,
    HL_HUP_CMD_SET_PAIR_INFO = 0x06,
    HL_HUP_CMD_GET_MAC_ADDR  = 0x07,
} hl_mod_extcom_hup_cmd_e;

typedef struct _hl_mod_extcom_temp_flag_st
{
    uint8_t tx1_bat_info_get_flag : 1;
    uint8_t tx2_bat_info_get_flag : 1;
    uint8_t rx_bat_info_get_flag : 1;
    uint8_t tx1_mac_addr_get_flag : 1;
    uint8_t tx2_mac_addr_get_flag : 1;
    uint8_t rx_mac_addr_get_flag : 1;
    uint8_t tx1_pair_mac_addr_get_flag : 1;
    uint8_t tx2_pair_mac_addr_get_flag : 1;
    uint8_t tx1_pair_mac_addr_set_flag : 1;
    uint8_t tx2_pair_mac_addr_set_flag : 1;
    uint8_t rx_pair_mac_addr_tx1_set_flag : 1;
    uint8_t rx_pair_mac_addr_tx2_set_flag : 1;
    uint8_t tx1_pair_ok_flag : 1;
    uint8_t tx2_pair_ok_flag : 1;
} hl_mod_extcom_temp_flag_st;

typedef struct _hl_mod_extcom_object_st
{
    hl_util_hup_t      hup;
    hl_util_fifo_t     fifo;
    struct rt_timer    timer;
    bool               timer_active_flag;
    bool               timeout_flag;
    bool               connect_flag;
    hl_hal_uart_numb_e uart_num;
} hl_mod_extcom_object_st;

typedef struct _hl_mod_extcom_st
{
    bool                       init_flag;
    bool                       start_flag;
    bool                       use_usb_cdc_flag;
    bool                       tx1_probe_flag;
    bool                       tx2_probe_flag;
    bool                       rx_probe_flag;
    hl_mod_extcom_temp_flag_st temp_flag;
    uint8_t                    tx1_bat_info;
    uint8_t                    tx2_bat_info;
    uint8_t                    rx_bat_info;
    void*                      msg_hd;
    struct rt_thread           extcom_thread;
    int                        thread_exit_flag;
    hl_mod_extcom_object_st*   p_objects;
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

static char _tx1_mac_addr[6]      = { 0 };
static char _tx2_mac_addr[6]      = { 0 };
static char _rx_mac_addr[6]       = { 0 };
static char _tx1_pair_mac_addr[6] = { 0 };
static char _tx2_pair_mac_addr[6] = { 0 };

static uint8_t hup_buf_rx[HL_MOD_EXTCOM_HUP_RX_BUFSZ]   = { 0 };
static uint8_t hup_buf_tx1[HL_MOD_EXTCOM_HUP_TX1_BUFSZ] = { 0 };
static uint8_t hup_buf_tx2[HL_MOD_EXTCOM_HUP_TX2_BUFSZ] = { 0 };
static uint8_t hup_buf_box[HL_MOD_EXTCOM_HUP_BOX_BUFSZ] = { 0 };

static uint8_t fifo_buf_rx[HL_MOD_EXTCOM_FIFO_RX_BUFSZ]   = { 0 };
static uint8_t fifo_buf_tx1[HL_MOD_EXTCOM_FIFO_TX1_BUFSZ] = { 0 };
static uint8_t fifo_buf_tx2[HL_MOD_EXTCOM_FIFO_TX2_BUFSZ] = { 0 };

static hl_mod_extcom_object_st objects[NUM_OF_HL_MOD_EXTCOM_OBJECT] = { 0 };

static uint8_t extcom_thread_stack[EXTCOM_THREAD_STACK_SIZE] = { 0 };

static hl_mod_msg_handle_st _extcom_msg_hd = { 0 };

static hl_mod_extcom_st _extcom_mod = {
    .init_flag        = false,
    .start_flag       = false,
    .use_usb_cdc_flag = false,
    .tx1_probe_flag   = false,
    .tx2_probe_flag   = false,
    .rx_probe_flag    = false,
    .msg_hd           = RT_NULL,
    .extcom_thread    = RT_NULL,
    .thread_exit_flag = 0,
    .p_objects        = RT_NULL,
};

/* Private function(only *.c)  -----------------------------------------------*/

static int _timer_start(hl_mod_extcom_object_e object, rt_tick_t timeout);
static int _timer_stop(hl_mod_extcom_object_e object);

static int _mod_msg_send(uint8_t cmd, void* param, uint16_t len)
{
    int ret;

    if (_extcom_mod.msg_hd != RT_NULL && _extcom_msg_hd.msg_send != RT_NULL) {
        ret = _extcom_msg_hd.msg_send(_extcom_msg_hd.msg_id, cmd, param, len);
        if (ret == 0) {
            return HL_MOD_EXTCOM_FUNC_OK;
        }
    }

    return HL_MOD_EXTCOM_FUNC_ERR;
}

static int _object_send_data(hl_mod_extcom_object_e object, char cmd, char* buf, int len, int ms)
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

    if (ms != 0) {
        _timer_start(object, ms);
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

    _timer_stop(HL_MOD_EXTCOM_OBJECT_RX);

    switch (hup_frame.cmd) {
        case HL_HUP_CMD_PROBE: {
            if (hup_frame.data_addr[0] == 0) {
                objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag = true;
                _mod_msg_send(HL_MOD_EXTCOM_MSG_RX_ONLINE_STATE, &(objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag),
                              sizeof(bool));
            }
        } break;
        case HL_HUP_CMD_GET_MAC_ADDR: {
            rt_memcpy(_rx_mac_addr, hup_frame.data_addr, sizeof(_rx_mac_addr));
            _extcom_mod.temp_flag.rx_mac_addr_get_flag = true;
        } break;
        case HL_HUP_CMD_SET_PAIR_INFO: {
            if (hup_frame.data_addr[0] == 1) {
                _extcom_mod.temp_flag.rx_pair_mac_addr_tx1_set_flag = true;
            } else if (hup_frame.data_addr[0] == 2) {
                _extcom_mod.temp_flag.rx_pair_mac_addr_tx2_set_flag = true;
            }
        } break;
        case HL_HUP_CMD_GET_BAT_INFO: {
            _extcom_mod.rx_bat_info                    = hup_frame.data_addr[0];
            _extcom_mod.temp_flag.rx_bat_info_get_flag = true;
            _mod_msg_send(HL_MOD_EXTCOM_MSG_RX_BAT_STATE, &(_extcom_mod.rx_bat_info), sizeof(_extcom_mod.rx_bat_info));
        } break;
        default:
            break;
    }
}

static void tx1_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    _timer_stop(HL_MOD_EXTCOM_OBJECT_TX1);

    switch (hup_frame.cmd) {
        case HL_HUP_CMD_PROBE: {
            if (hup_frame.data_addr[0] == 1) {
                objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag = true;
                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX1_ONLINE_STATE, &(objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag),
                              sizeof(bool));
            }
        } break;
        case HL_HUP_CMD_GET_MAC_ADDR: {
            rt_memcpy(_tx1_mac_addr, hup_frame.data_addr, sizeof(_tx1_mac_addr));
            _extcom_mod.temp_flag.tx1_mac_addr_get_flag = true;
        } break;
        case HL_HUP_CMD_GET_PAIR_INFO: {
            rt_memcpy(_tx1_pair_mac_addr, hup_frame.data_addr, sizeof(_tx1_pair_mac_addr));
            _extcom_mod.temp_flag.tx1_pair_mac_addr_get_flag = true;
        } break;
        case HL_HUP_CMD_SET_PAIR_INFO: {
            if (hup_frame.data_addr[0] == 0) {
                _extcom_mod.temp_flag.tx1_pair_mac_addr_set_flag = true;
            }
        } break;
        case HL_HUP_CMD_GET_BAT_INFO: {
            _extcom_mod.tx1_bat_info                    = hup_frame.data_addr[0];
            _extcom_mod.temp_flag.tx1_bat_info_get_flag = true;
            _mod_msg_send(HL_MOD_EXTCOM_MSG_TX1_BAT_STATE, &(_extcom_mod.tx1_bat_info),
                          sizeof(_extcom_mod.tx1_bat_info));
        } break;
        default:
            break;
    }
}

static void tx2_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    _timer_stop(HL_MOD_EXTCOM_OBJECT_TX2);

    switch (hup_frame.cmd) {
        case HL_HUP_CMD_PROBE: {
            if (hup_frame.data_addr[0] == 1) {
                objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag = true;
                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX2_ONLINE_STATE, &(objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag),
                              sizeof(bool));
            }
        } break;
        case HL_HUP_CMD_GET_MAC_ADDR: {
            rt_memcpy(_tx2_mac_addr, hup_frame.data_addr, sizeof(_tx2_mac_addr));
            _extcom_mod.temp_flag.tx2_mac_addr_get_flag = true;
        } break;
        case HL_HUP_CMD_GET_PAIR_INFO: {
            rt_memcpy(_tx2_pair_mac_addr, hup_frame.data_addr, sizeof(_tx2_pair_mac_addr));
            _extcom_mod.temp_flag.tx2_pair_mac_addr_get_flag = true;
        } break;
        case HL_HUP_CMD_SET_PAIR_INFO: {
            if (hup_frame.data_addr[0] == 0) {
                _extcom_mod.temp_flag.tx2_pair_mac_addr_set_flag = true;
            }
        } break;
        case HL_HUP_CMD_GET_BAT_INFO: {
            _extcom_mod.tx2_bat_info                    = hup_frame.data_addr[0];
            _extcom_mod.temp_flag.tx2_bat_info_get_flag = true;
            _mod_msg_send(HL_MOD_EXTCOM_MSG_TX2_BAT_STATE, &(_extcom_mod.tx2_bat_info),
                          sizeof(_extcom_mod.tx2_bat_info));
        } break;
        default:
            break;
    }
}

static void box_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

    _timer_stop(HL_MOD_EXTCOM_OBJECT_BOX);

    switch (hup_frame.cmd) {
        default:
            break;
    }
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

    p_object->timer_active_flag = false;

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

    objects[HL_MOD_EXTCOM_OBJECT_RX].timer_active_flag  = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].timer_active_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].timer_active_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_BOX].timer_active_flag = false;

    return HL_HAL_USB_CDC_FUNC_OK;
}

static int _timer_start(hl_mod_extcom_object_e object, rt_tick_t timeout)
{
    rt_timer_control(&(objects[object].timer), RT_TIMER_CTRL_SET_TIME, &timeout);

    objects[object].timer_active_flag = true;

    rt_timer_start(&(objects[object].timer));
}

static int _timer_stop(hl_mod_extcom_object_e object)
{
    objects[object].timer_active_flag = false;

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

static bool _timer_active_check(hl_mod_extcom_object_e object)
{
    return objects[object].timer_active_flag;
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

    objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag  = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag = false;
    objects[HL_MOD_EXTCOM_OBJECT_BOX].connect_flag = false;

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

static void _dev_probe_poll(void)
{
    static uint8_t tx1_probe_count = 0;
    static uint8_t tx2_probe_count = 0;
    static uint8_t rx_probe_count  = 0;

    if (_extcom_mod.tx1_probe_flag == true && objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        if (tx1_probe_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_PROBE, RT_NULL, 0, 0);
            tx1_probe_count = 100;
        } else {
            tx1_probe_count--;
        }
    } else if (_extcom_mod.tx1_probe_flag == false) {
        objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag = false;
        tx1_probe_count                                = 0;
    }

    if (_extcom_mod.tx2_probe_flag == true && objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        if (tx2_probe_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_PROBE, RT_NULL, 0, 0);
            tx2_probe_count = 100;
        } else {
            tx2_probe_count--;
        }
    } else if (_extcom_mod.tx2_probe_flag == false) {
        objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag = false;
        tx2_probe_count                                = 0;
    }

    if (_extcom_mod.rx_probe_flag == true && objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == false) {
        if (rx_probe_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PROBE, RT_NULL, 0, 0);
            rx_probe_count = 100;
        } else {
            rx_probe_count--;
        }
    } else if (_extcom_mod.rx_probe_flag == false) {
        objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag = false;
        rx_probe_count                                = 0;
    }
}

static void _dev_get_mac_poll(void)
{
    static uint8_t              tx1_count = 0;
    static uint8_t              tx2_count = 0;
    static uint8_t              rx_count  = 0;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == true && flag->tx1_mac_addr_get_flag == false) {
        if (tx1_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0, 0);
            tx1_count = 100;
        } else {
            tx1_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        flag->tx1_mac_addr_get_flag = false;
        tx1_count                   = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == true && flag->tx2_mac_addr_get_flag == false) {
        if (tx2_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0, 0);
            tx2_count = 100;
        } else {
            tx2_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        flag->tx2_mac_addr_get_flag = false;
        tx2_count                   = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true && flag->rx_mac_addr_get_flag == false) {
        if (rx_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0, 0);
            rx_count = 100;
        } else {
            rx_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == false) {
        flag->rx_mac_addr_get_flag = false;
        rx_count                   = 0;
    }
}

static void _dev_get_pair_mac_poll(void)
{
    static uint8_t              tx1_count = 0;
    static uint8_t              tx2_count = 0;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == true && flag->tx1_pair_mac_addr_get_flag == false) {
        if (tx1_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_PAIR_INFO, RT_NULL, 0, 0);
            tx1_count = 100;
        } else {
            tx1_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        flag->tx1_pair_mac_addr_get_flag = false;
        tx1_count                        = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == true && flag->tx2_pair_mac_addr_get_flag == false) {
        if (tx2_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_PAIR_INFO, RT_NULL, 0, 0);
            tx2_count = 100;
        } else {
            tx2_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        flag->tx2_pair_mac_addr_get_flag = false;
        tx2_count                        = 0;
    }
}

static void _dev_set_pair_mac_poll(void)
{
    static uint8_t              tx1_count  = 0;
    static uint8_t              tx2_count  = 0;
    static uint8_t              rx_count_1 = 0;
    static uint8_t              rx_count_2 = 0;
    hl_mod_extcom_temp_flag_st* flag;

    char rx_pair_mac_set[7];

    flag = &_extcom_mod.temp_flag;

    if (flag->rx_mac_addr_get_flag == true && flag->tx1_mac_addr_get_flag == true
        && flag->tx1_pair_mac_addr_get_flag == true) {
        if (flag->tx1_pair_ok_flag == false && rt_memcmp(_rx_mac_addr, _tx1_pair_mac_addr, sizeof(_rx_mac_addr)) == 0) {
            flag->tx1_pair_ok_flag = true;
        }

        if (flag->tx1_pair_ok_flag == false) {
            if (flag->tx1_pair_mac_addr_set_flag == false) {
                if (tx1_count == 0) {
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_SET_PAIR_INFO, _rx_mac_addr,
                                      sizeof(_rx_mac_addr), 0);
                    tx1_count = 100;
                } else {
                    tx1_count--;
                }
            }

            if (flag->rx_pair_mac_addr_tx1_set_flag == false) {
                if (rx_count_1 == 0) {
                    rx_pair_mac_set[0] = 1;
                    rt_memcpy(rx_pair_mac_set + 1, _tx1_mac_addr, sizeof(_tx1_mac_addr));
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_PAIR_INFO, rx_pair_mac_set,
                                      sizeof(rx_pair_mac_set), 0);
                    rx_count_1 = 100;
                } else {
                    rx_count_1--;
                }
            }

            if (flag->tx1_pair_mac_addr_set_flag == true && flag->rx_pair_mac_addr_tx1_set_flag == true) {
                flag->tx1_pair_ok_flag = true;
            }
        }
    } else {
        flag->tx1_pair_mac_addr_set_flag    = false;
        flag->rx_pair_mac_addr_tx1_set_flag = false;
        flag->tx1_pair_ok_flag              = false;
    }

    if (flag->rx_mac_addr_get_flag == true && flag->tx2_mac_addr_get_flag == true
        && flag->tx2_pair_mac_addr_get_flag == true) {
        if (flag->tx2_pair_ok_flag == false && rt_memcmp(_rx_mac_addr, _tx2_pair_mac_addr, sizeof(_rx_mac_addr)) == 0) {
            flag->tx2_pair_ok_flag = true;
        }

        if (flag->tx2_pair_ok_flag == false) {
            if (flag->tx2_pair_mac_addr_set_flag == false) {
                if (tx2_count == 0) {
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_SET_PAIR_INFO, _rx_mac_addr,
                                      sizeof(_rx_mac_addr), 0);
                    tx2_count = 100;
                } else {
                    tx2_count--;
                }
            }

            if (flag->rx_pair_mac_addr_tx2_set_flag == false) {
                if (rx_count_2 == 0) {
                    rx_pair_mac_set[0] = 2;
                    rt_memcpy(rx_pair_mac_set + 1, _tx2_mac_addr, sizeof(_tx2_mac_addr));
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_PAIR_INFO, rx_pair_mac_set,
                                      sizeof(rx_pair_mac_set), 0);
                    rx_count_2 = 100;
                } else {
                    rx_count_2--;
                }
            }

            if (flag->tx2_pair_mac_addr_set_flag == true && flag->rx_pair_mac_addr_tx2_set_flag == true) {
                flag->tx2_pair_ok_flag = true;
            }
        }
    } else {
        flag->tx2_pair_mac_addr_set_flag    = false;
        flag->rx_pair_mac_addr_tx2_set_flag = false;
        flag->tx2_pair_ok_flag              = false;
    }
}

static void _dev_get_bat_info_poll(void)
{
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        flag->tx1_bat_info_get_flag = false;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        flag->tx2_bat_info_get_flag = false;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == false) {
        flag->rx_bat_info_get_flag = false;
    }
}

static void _extcom_thread_entry(void* arg)
{

    _mod_msg_send(HL_MOD_EXTCOM_MSG_START, RT_NULL, 0);

    while (_extcom_mod.thread_exit_flag == 0) {
        _rx_uart_msg_poll();
        _tx1_uart_msg_poll();
        _tx2_uart_msg_poll();
        _box_uart_msg_poll();

        _dev_probe_poll();
        _dev_get_mac_poll();
        _dev_get_pair_mac_poll();
        _dev_set_pair_mac_poll();
        _dev_get_bat_info_poll();

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

    if (msg_hd != RT_NULL) {
        _extcom_msg_hd.msg_id   = ((hl_mod_msg_handle_st*)(msg_hd))->msg_id;
        _extcom_msg_hd.msg_send = ((hl_mod_msg_handle_st*)(msg_hd))->msg_send;
        _extcom_mod.msg_hd      = &_extcom_msg_hd;
    } else {
        _extcom_mod.msg_hd = RT_NULL;
    }

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

    _extcom_mod.tx1_probe_flag = false;
    _extcom_mod.tx2_probe_flag = false;
    _extcom_mod.rx_probe_flag  = false;

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
        case HL_MOD_EXTCOM_START_TX1_PROBE: {
            if (arg_size != sizeof(bool)) {
                DBG_LOG("size err, ctrl arg need <bool> type pointer!\n");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.tx1_probe_flag = *(bool*)arg;
        } break;
        case HL_MOD_EXTCOM_START_TX2_PROBE: {
            if (arg_size != sizeof(bool)) {
                DBG_LOG("size err, ctrl arg need <bool> type pointer!\n");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.tx2_probe_flag = *(bool*)arg;
        } break;
        case HL_MOD_EXTCOM_START_RX_PROBE: {
            if (arg_size != sizeof(bool)) {
                DBG_LOG("size err, ctrl arg need <bool> type pointer!\n");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.rx_probe_flag = *(bool*)arg;
        } break;
        default:
            break;
    }

    return HL_MOD_EXTCOM_FUNC_OK;
}

/*
 * EOF
 */