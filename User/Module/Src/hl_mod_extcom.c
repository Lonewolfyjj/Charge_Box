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

#define DBG_SECTION_NAME "extcom"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_extcom_hup_cmd_e
{
    HL_HUP_CMD_PROBE             = 0x01,
    HL_HUP_CMD_GET_BAT_INFO      = 0x02,
    HL_HUP_CMD_SET_BAT_INFO      = 0x03,
    HL_HUP_CMD_TX_IN_BOX_STATE   = 0x04,
    HL_HUP_CMD_GET_PAIR_INFO     = 0x05,
    HL_HUP_CMD_SET_PAIR_INFO     = 0x06,
    HL_HUP_CMD_GET_MAC_ADDR      = 0x07,
    HL_HUP_CMD_PAIR_START        = 0x08,
    HL_HUP_CMD_PAIR_STOP         = 0x09,
    HL_HUP_CMD_GET_CHARGE_STATE  = 0x0A,
    HL_HUP_CMD_SET_CHARGE_STATE  = 0x0B,
    HL_HUP_CMD_SET_BOX_LID_STATE = 0x0E,
} hl_mod_extcom_hup_cmd_e;

typedef struct _hl_mod_extcom_temp_flag_st
{
    uint8_t tx1_bat_info_get_flag : 1;
    uint8_t tx2_bat_info_get_flag : 1;
    uint8_t rx_bat_info_get_flag : 1;
    uint8_t box_bat_info_get_flag : 1;
    uint8_t tx1_charge_state_get_flag : 1;
    uint8_t tx2_charge_state_get_flag : 1;
    uint8_t rx_charge_state_get_flag : 1;
    uint8_t box_charge_state_get_flag : 1;
    uint8_t tx1_mac_addr_get_flag : 1;
    uint8_t tx2_mac_addr_get_flag : 1;
    uint8_t rx_mac_addr_get_flag : 1;
    uint8_t tx1_pair_mac_addr_get_flag : 1;
    uint8_t tx2_pair_mac_addr_get_flag : 1;
    uint8_t tx1_pair_mac_addr_set_flag : 1;
    uint8_t tx2_pair_mac_addr_set_flag : 1;
    uint8_t tx1_pair_mac_addr_set_timeout_flag : 1;
    uint8_t tx2_pair_mac_addr_set_timeout_flag : 1;
    uint8_t rx_pair_mac_addr_tx1_set_flag : 1;
    uint8_t rx_pair_mac_addr_tx2_set_flag : 1;
    uint8_t tx1_pair_check_flag : 1;
    uint8_t tx2_pair_check_flag : 1;
    uint8_t tx1_need_pair_flag : 1;
    uint8_t tx2_need_pair_flag : 1;
    uint8_t tx1_pair_start_flag : 1;
    uint8_t tx2_pair_start_flag : 1;
    uint8_t tx1_pair_ok_flag : 1;
    uint8_t tx2_pair_ok_flag : 1;
    uint8_t tx1_pair_stop_flag : 1;
    uint8_t tx2_pair_stop_flag : 1;
    uint8_t rx_set_tx1_online_state_flag : 1;
    uint8_t rx_set_tx2_online_state_flag : 1;
    uint8_t rx_set_tx1_lose_state_flag : 1;
    uint8_t rx_set_tx2_lose_state_flag : 1;
    uint8_t rx_set_box_lid_state_flag : 1;
    uint8_t rx_set_tx1_bat_info_flag : 1;
    uint8_t rx_set_tx2_bat_info_flag : 1;
    uint8_t rx_set_box_bat_info_flag : 1;
    uint8_t rx_set_tx1_charge_state_flag : 1;
    uint8_t rx_set_tx2_charge_state_flag : 1;
    uint8_t rx_set_box_charge_state_flag : 1;
} hl_mod_extcom_temp_flag_st;

typedef struct _hl_mod_extcom_object_st
{
    hl_util_hup_t      hup;
    hl_util_fifo_t     fifo;
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
    uint8_t                    box_bat_info;
    uint8_t                    tx1_bat_info;
    uint8_t                    tx2_bat_info;
    uint8_t                    rx_bat_info;
    uint8_t                    box_charge_state;
    uint8_t                    tx1_charge_state;
    uint8_t                    tx2_charge_state;
    uint8_t                    rx_charge_state;
    uint8_t                    box_lid_state;
    void*                      msg_hd;
    struct rt_thread           extcom_thread;
    int                        thread_exit_flag;
    hl_mod_extcom_object_st*   p_objects;
} hl_mod_extcom_st;

/* define --------------------------------------------------------------------*/
#define HL_MOD_EXTCOM_HUP_RX_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_TX1_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_TX2_BUFSZ 256
#define HL_MOD_EXTCOM_HUP_BOX_BUFSZ 256

#define HL_MOD_EXTCOM_FIFO_RX_BUFSZ 256
#define HL_MOD_EXTCOM_FIFO_TX1_BUFSZ 256
#define HL_MOD_EXTCOM_FIFO_TX2_BUFSZ 256

#define EXTCOM_THREAD_STACK_SIZE 612

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
            if (_extcom_mod.rx_bat_info != hup_frame.data_addr[0]) {
                _extcom_mod.rx_bat_info                    = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.rx_bat_info_get_flag = true;
                _mod_msg_send(HL_MOD_EXTCOM_MSG_RX_BAT_STATE, &(_extcom_mod.rx_bat_info),
                              sizeof(_extcom_mod.rx_bat_info));
            }
        } break;
        case HL_HUP_CMD_PAIR_START: {
            if (hup_frame.data_addr[0] == 1) {
                _extcom_mod.temp_flag.tx1_pair_start_flag = true;
            } else if (hup_frame.data_addr[0] == 2) {
                _extcom_mod.temp_flag.tx2_pair_start_flag = true;
            }
        } break;
        case HL_HUP_CMD_PAIR_STOP: {
            if (hup_frame.data_addr[0] == 1) {
                _extcom_mod.temp_flag.tx1_pair_stop_flag = true;
            } else if (hup_frame.data_addr[0] == 2) {
                _extcom_mod.temp_flag.tx2_pair_stop_flag = true;
            }
        } break;
        case HL_HUP_CMD_TX_IN_BOX_STATE: {
            if (hup_frame.data_addr[0] == 1) {
                if (hup_frame.data_addr[1] == 1) {
                    _extcom_mod.temp_flag.rx_set_tx1_online_state_flag = true;
                } else {
                    _extcom_mod.temp_flag.rx_set_tx1_lose_state_flag = true;
                }
            } else if (hup_frame.data_addr[0] == 2) {
                if (hup_frame.data_addr[1] == 1) {
                    _extcom_mod.temp_flag.rx_set_tx2_online_state_flag = true;
                } else {
                    _extcom_mod.temp_flag.rx_set_tx2_lose_state_flag = true;
                }
            }
        }
        case HL_HUP_CMD_SET_BAT_INFO: {
            // do nothing
        } break;
        case HL_HUP_CMD_GET_CHARGE_STATE: {
            if (_extcom_mod.rx_charge_state != hup_frame.data_addr[0]) {
                _extcom_mod.rx_charge_state                    = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.rx_charge_state_get_flag = true;
                _mod_msg_send(HL_MOD_EXTCOM_MSG_RX_CHARGE_STATE, &(_extcom_mod.rx_charge_state),
                              sizeof(_extcom_mod.rx_charge_state));
            }
        } break;
        default:
            break;
    }
}

static void tx1_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

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
            if (_extcom_mod.tx1_bat_info != hup_frame.data_addr[0]) {
                _extcom_mod.tx1_bat_info                       = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.tx1_bat_info_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_tx1_bat_info_flag = false;

                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX1_BAT_STATE, &(_extcom_mod.tx1_bat_info),
                              sizeof(_extcom_mod.tx1_bat_info));
            }
        } break;
        case HL_HUP_CMD_GET_CHARGE_STATE: {
            if (_extcom_mod.tx1_charge_state != hup_frame.data_addr[0]) {
                _extcom_mod.tx1_charge_state                       = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.tx1_charge_state_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_tx1_charge_state_flag = false;

                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX1_CHARGE_STATE, &(_extcom_mod.tx1_charge_state),
                              sizeof(_extcom_mod.tx1_charge_state));
            }
        } break;
        default:
            break;
    }
}

static void tx2_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

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
            if (_extcom_mod.tx2_bat_info != hup_frame.data_addr[0]) {
                _extcom_mod.tx2_bat_info                       = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.tx2_bat_info_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_tx2_bat_info_flag = false;

                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX2_BAT_STATE, &(_extcom_mod.tx2_bat_info),
                              sizeof(_extcom_mod.tx2_bat_info));
            }
        } break;
        case HL_HUP_CMD_GET_CHARGE_STATE: {
            if (_extcom_mod.tx2_charge_state != hup_frame.data_addr[0]) {
                _extcom_mod.tx2_charge_state                       = hup_frame.data_addr[0];
                _extcom_mod.temp_flag.tx2_charge_state_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_tx2_charge_state_flag = false;

                _mod_msg_send(HL_MOD_EXTCOM_MSG_TX2_CHARGE_STATE, &(_extcom_mod.tx2_charge_state),
                              sizeof(_extcom_mod.tx2_charge_state));
            }
        } break;
        default:
            break;
    }
}

static void box_hup_success_handle_func(hup_protocol_type_t hup_frame)
{
    uint16_t len = ((uint16_t)(hup_frame.data_len_h) << 8) | hup_frame.data_len_l;

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
        LOG_E("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.frame_data_len = sizeof(hup_buf_tx1);
    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.role           = EM_HUP_ROLE_MASTER;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX1].hup), hup_buf_tx1, RT_NULL, tx1_hup_success_handle_func);
    if (ret == -1) {
        LOG_E("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.frame_data_len = sizeof(hup_buf_tx2);
    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.role           = EM_HUP_ROLE_MASTER;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_TX2].hup), hup_buf_tx2, RT_NULL, tx2_hup_success_handle_func);
    if (ret == -1) {
        LOG_E("hup init err!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.frame_data_len = sizeof(hup_buf_box);
    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.role           = EM_HUP_ROLE_SLAVE;
    objects[HL_MOD_EXTCOM_OBJECT_BOX].hup.hup_handle.timer_state    = EM_HUP_TIMER_DISABLE;

    ret = hl_util_hup_init(&(objects[HL_MOD_EXTCOM_OBJECT_BOX].hup), hup_buf_box, RT_NULL, box_hup_success_handle_func);
    if (ret == -1) {
        LOG_E("hup init err!");
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

    objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num  = HL_HAL_UART_NUMB_3;
    objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num = HL_HAL_UART_NUMB_1;
    objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num = HL_HAL_UART_NUMB_2;

    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num, 115200);
    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num, 115200);
    hl_hal_uart_init(objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num, 115200);

    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_RX].uart_num, _rx_uart_callback);
    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_TX1].uart_num, _tx1_uart_callback);
    hl_hal_uart_receive_reg(objects[HL_MOD_EXTCOM_OBJECT_TX2].uart_num, _tx2_uart_callback);

    return HL_HAL_USB_CDC_FUNC_OK;
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_PROBE, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_PROBE, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PROBE, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_GET_MAC_ADDR, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_PAIR_INFO, RT_NULL, 0);
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
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_PAIR_INFO, RT_NULL, 0);
            tx2_count = 100;
        } else {
            tx2_count--;
        }
    } else if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        flag->tx2_pair_mac_addr_get_flag = false;
        tx2_count                        = 0;
    }
}

static void _dev_pair_check_poll(void)
{
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (flag->rx_mac_addr_get_flag == true && flag->tx1_mac_addr_get_flag == true
        && flag->tx1_pair_mac_addr_get_flag == true) {
        if (flag->tx1_pair_check_flag == false) {
            flag->tx1_pair_check_flag = true;
            if (rt_memcmp(_rx_mac_addr, _tx1_pair_mac_addr, sizeof(_rx_mac_addr)) == 0) {
                flag->tx1_need_pair_flag = false;
            } else {
                flag->tx1_need_pair_flag = true;
            }
        }
    } else {
        flag->tx1_pair_check_flag = false;
    }

    if (flag->rx_mac_addr_get_flag == true && flag->tx2_mac_addr_get_flag == true
        && flag->tx2_pair_mac_addr_get_flag == true) {
        if (flag->tx2_pair_check_flag == false) {
            flag->tx2_pair_check_flag = true;
            if (rt_memcmp(_rx_mac_addr, _tx2_pair_mac_addr, sizeof(_rx_mac_addr)) == 0) {
                flag->tx2_need_pair_flag = false;
            } else {
                flag->tx2_need_pair_flag = true;
            }
        }
    } else {
        flag->tx2_pair_check_flag = false;
    }
}

static void _dev_pair_start_poll()
{
    uint8_t                     tx_num;
    static uint8_t              rx_count_1 = 0;
    static uint8_t              rx_count_2 = 0;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (flag->tx1_pair_check_flag == true && flag->tx1_need_pair_flag == true) {
        if (flag->tx1_pair_start_flag == false) {
            if (rx_count_1 == 0) {
                tx_num = 1;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PAIR_START, &tx_num, sizeof(tx_num));
                rx_count_1 = 100;
            } else {
                rx_count_1--;
            }
        }
    } else {
        flag->tx1_pair_start_flag = false;
        rx_count_1                = 0;
    }

    if (flag->tx2_pair_check_flag == true && flag->tx2_need_pair_flag == true) {
        if (flag->tx2_pair_start_flag == false) {
            if (rx_count_2 == 0) {
                tx_num = 2;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PAIR_START, &tx_num, sizeof(tx_num));
                rx_count_2 = 100;
            } else {
                rx_count_2--;
            }
        }
    } else {
        flag->tx2_pair_start_flag = false;
        rx_count_2                = 0;
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

    if (flag->tx1_pair_start_flag == true) {
        if (flag->tx1_pair_ok_flag == false) {
            if (flag->tx1_pair_mac_addr_set_flag == false) {
                if (tx1_count == 0) {
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_SET_PAIR_INFO, _rx_mac_addr,
                                      sizeof(_rx_mac_addr));
                    tx1_count = 100;
                } else if (tx1_count == 1) {
                    flag->tx1_pair_mac_addr_set_timeout_flag = true;
                } else {
                    tx1_count--;
                }
            }

            if (flag->rx_pair_mac_addr_tx1_set_flag == false) {
                if (rx_count_1 == 0) {
                    rx_pair_mac_set[0] = 1;
                    rt_memcpy(rx_pair_mac_set + 1, _tx1_mac_addr, sizeof(_tx1_mac_addr));
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_PAIR_INFO, rx_pair_mac_set,
                                      sizeof(rx_pair_mac_set));
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
        flag->tx1_pair_ok_flag                   = false;
        flag->tx1_pair_mac_addr_set_flag         = false;
        flag->rx_pair_mac_addr_tx1_set_flag      = false;
        flag->tx1_pair_mac_addr_set_timeout_flag = false;
        tx1_count                                = 0;
        rx_count_1                               = 0;
    }

    if (flag->tx2_pair_start_flag == true) {
        if (flag->tx2_pair_ok_flag == false) {
            if (flag->tx2_pair_mac_addr_set_flag == false) {
                if (tx2_count == 0) {
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_SET_PAIR_INFO, _rx_mac_addr,
                                      sizeof(_rx_mac_addr));
                    tx2_count = 100;
                } else if (tx2_count == 1) {
                    flag->tx2_pair_mac_addr_set_timeout_flag = true;
                } else {
                    tx2_count--;
                }
            }

            if (flag->rx_pair_mac_addr_tx2_set_flag == false) {
                if (rx_count_2 == 0) {
                    rx_pair_mac_set[0] = 2;
                    rt_memcpy(rx_pair_mac_set + 1, _tx2_mac_addr, sizeof(_tx2_mac_addr));
                    _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_PAIR_INFO, rx_pair_mac_set,
                                      sizeof(rx_pair_mac_set));
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
        flag->tx2_pair_mac_addr_set_flag         = false;
        flag->rx_pair_mac_addr_tx2_set_flag      = false;
        flag->tx2_pair_ok_flag                   = false;
        flag->tx2_pair_mac_addr_set_timeout_flag = false;
        tx2_count                                = 0;
        rx_count_2                               = 0;
    }
}

static void _dev_pair_stop_poll(void)
{
    uint8_t                     buf_send[2];
    static uint8_t              rx_count_1 = 0;
    static uint8_t              rx_count_2 = 0;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (flag->tx1_pair_ok_flag == true || flag->tx1_pair_mac_addr_set_timeout_flag == true) {
        if (flag->tx1_pair_stop_flag == false) {
            if (rx_count_1 == 0) {
                buf_send[0] = 1;
                if (flag->tx1_pair_mac_addr_set_timeout_flag == true) {
                    buf_send[1] = 0;
                } else {
                    buf_send[1] = 1;
                }
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PAIR_STOP, buf_send, sizeof(buf_send));
                rx_count_1 = 100;
            } else {
                rx_count_1--;
            }
        }
    } else {
        flag->tx1_pair_stop_flag = false;
        rx_count_1               = 0;
    }

    if (flag->tx2_pair_ok_flag == true || flag->tx2_pair_mac_addr_set_timeout_flag == true) {
        if (flag->tx2_pair_stop_flag == false) {
            if (rx_count_2 == 0) {
                buf_send[0] = 2;
                if (flag->tx2_pair_mac_addr_set_timeout_flag == true) {
                    buf_send[1] = 0;
                } else {
                    buf_send[1] = 1;
                }
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_PAIR_STOP, buf_send, sizeof(buf_send));
                rx_count_2 = 100;
            } else {
                rx_count_2--;
            }
        }
    } else {
        flag->tx2_pair_stop_flag = false;
        rx_count_2               = 0;
    }
}

static void _dev_set_tx_online_state_poll(void)
{
    uint8_t                     buf_send[2];
    static uint8_t              rx_count_1 = 0;
    static uint8_t              rx_count_2 = 0;
    static uint8_t              rx_count_3 = 0;
    static uint8_t              rx_count_4 = 0;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true
        && objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == true) {
        if (flag->rx_set_tx1_online_state_flag == false) {
            if (rx_count_1 == 0) {
                buf_send[0] = 1;
                buf_send[1] = 1;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_TX_IN_BOX_STATE, buf_send, sizeof(buf_send));
                rx_count_1 = 100;
            } else {
                rx_count_1--;
            }
        }
    } else {
        flag->rx_set_tx1_online_state_flag = false;
        rx_count_1                         = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true
        && objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        if (flag->rx_set_tx1_lose_state_flag == false) {
            if (rx_count_3 == 0) {
                buf_send[0] = 1;
                buf_send[1] = 0;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_TX_IN_BOX_STATE, buf_send, sizeof(buf_send));
                rx_count_3 = 100;
            } else {
                rx_count_3--;
            }
        }
    } else {
        flag->rx_set_tx1_lose_state_flag = false;
        rx_count_3                       = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true
        && objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == true) {
        if (flag->rx_set_tx2_online_state_flag == false) {
            if (rx_count_2 == 0) {
                buf_send[0] = 2;
                buf_send[1] = 1;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_TX_IN_BOX_STATE, buf_send, sizeof(buf_send));
                rx_count_2 = 100;
            } else {
                rx_count_2--;
            }
        }
    } else {
        flag->rx_set_tx2_online_state_flag = false;
        rx_count_2                         = 0;
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true
        && objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        if (flag->rx_set_tx2_lose_state_flag == false) {
            if (rx_count_4 == 0) {
                buf_send[0] = 2;
                buf_send[1] = 0;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_TX_IN_BOX_STATE, buf_send, sizeof(buf_send));
                rx_count_4 = 100;
            } else {
                rx_count_4--;
            }
        }
    } else {
        flag->rx_set_tx2_lose_state_flag = false;
        rx_count_4                       = 0;
    }
}

static void _dev_get_bat_info_poll(void)
{
    static uint16_t tx1_count = 0;
    static uint16_t tx2_count = 0;
    static uint16_t rx_count  = 0;

    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_TX1].connect_flag == false) {
        flag->tx1_bat_info_get_flag     = false;
        flag->tx1_charge_state_get_flag = false;
        _extcom_mod.tx1_bat_info        = 0xFF;
        _extcom_mod.tx1_charge_state    = 0;
        tx1_count                       = 0;
    } else {
        if (tx1_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_BAT_INFO, RT_NULL, 0);
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX1, HL_HUP_CMD_GET_CHARGE_STATE, RT_NULL, 0);
            tx1_count = 100;
        } else {
            tx1_count--;
        }
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_TX2].connect_flag == false) {
        flag->tx2_bat_info_get_flag     = false;
        flag->tx2_charge_state_get_flag = false;
        _extcom_mod.tx2_bat_info        = 0xFF;
        _extcom_mod.tx2_charge_state    = 0;
        tx2_count                       = 0;
    } else {
        if (tx2_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_BAT_INFO, RT_NULL, 0);
            _object_send_data(HL_MOD_EXTCOM_OBJECT_TX2, HL_HUP_CMD_GET_CHARGE_STATE, RT_NULL, 0);
            tx2_count = 100;
        } else {
            tx2_count--;
        }
    }

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == false) {
        flag->rx_bat_info_get_flag     = false;
        flag->rx_charge_state_get_flag = false;
        _extcom_mod.rx_bat_info        = 0xFF;
        _extcom_mod.rx_charge_state    = 0;
        rx_count                       = 0;
    } else {
        if (rx_count == 0) {
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_GET_BAT_INFO, RT_NULL, 0);
            _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_GET_CHARGE_STATE, RT_NULL, 0);
            rx_count = 100;
        } else {
            rx_count--;
        }
    }
}

static void _dev_set_bat_soc_poll(void)
{
    char                        buf_send[2];
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true) {
        if (flag->tx1_bat_info_get_flag == true) {
            if (flag->rx_set_tx1_bat_info_flag == false) {
                flag->rx_set_tx1_bat_info_flag = true;

                buf_send[0] = 1;
                buf_send[1] = _extcom_mod.tx1_bat_info;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_BAT_INFO, buf_send, sizeof(buf_send));
            }
        }

        if (flag->tx2_bat_info_get_flag == true) {
            if (flag->rx_set_tx2_bat_info_flag == false) {
                flag->rx_set_tx2_bat_info_flag = true;

                buf_send[0] = 2;
                buf_send[1] = _extcom_mod.tx2_bat_info;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_BAT_INFO, buf_send, sizeof(buf_send));
            }
        }

        if (flag->box_bat_info_get_flag == true) {
            if (flag->rx_set_box_bat_info_flag == false) {
                flag->rx_set_box_bat_info_flag = true;

                buf_send[0] = 3;
                buf_send[1] = _extcom_mod.box_bat_info;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_BAT_INFO, buf_send, sizeof(buf_send));
            }
        }
    } else {
        flag->rx_set_tx1_bat_info_flag = false;
        flag->rx_set_tx2_bat_info_flag = false;
        flag->rx_set_box_bat_info_flag = false;
    }
}

static void _dev_set_charge_state_poll(void)
{
    char                        buf_send[2];
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == true) {
        if (flag->tx1_charge_state_get_flag == true) {
            if (flag->rx_set_tx1_charge_state_flag == false) {
                flag->rx_set_tx1_charge_state_flag = true;

                buf_send[0] = 1;
                buf_send[1] = _extcom_mod.tx1_charge_state;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_CHARGE_STATE, buf_send, sizeof(buf_send));
            }
        }

        if (flag->tx2_charge_state_get_flag == true) {
            if (flag->rx_set_tx2_charge_state_flag == false) {
                flag->rx_set_tx2_charge_state_flag = true;

                buf_send[0] = 2;
                buf_send[1] = _extcom_mod.tx2_charge_state;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_CHARGE_STATE, buf_send, sizeof(buf_send));
            }
        }

        if (flag->box_charge_state_get_flag == true) {
            if (flag->rx_set_box_charge_state_flag == false) {
                flag->rx_set_box_charge_state_flag = true;

                buf_send[0] = 3;
                buf_send[1] = _extcom_mod.box_charge_state;
                _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_CHARGE_STATE, buf_send, sizeof(buf_send));
            }
        }
    } else {
        flag->rx_set_tx1_charge_state_flag = false;
        flag->rx_set_tx2_charge_state_flag = false;
        flag->rx_set_box_charge_state_flag = false;
    }
}

static void _dev_set_box_lid_state_poll(void)
{
    uint8_t                     lid_state;
    hl_mod_extcom_temp_flag_st* flag;

    flag = &_extcom_mod.temp_flag;

    if (objects[HL_MOD_EXTCOM_OBJECT_RX].connect_flag == false) {
        flag->rx_set_box_lid_state_flag = false;
        return;
    }

    if (flag->rx_set_box_lid_state_flag == false) {
        flag->rx_set_box_lid_state_flag = true;

        if (_extcom_mod.box_lid_state == 2) {
            lid_state = 1;
        } else {
            lid_state = 0;
        }

        _object_send_data(HL_MOD_EXTCOM_OBJECT_RX, HL_HUP_CMD_SET_BOX_LID_STATE, &lid_state, sizeof(lid_state));
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
        _dev_pair_check_poll();
        _dev_pair_start_poll();
        _dev_set_pair_mac_poll();
        _dev_pair_stop_poll();
        _dev_set_tx_online_state_poll();
        _dev_get_bat_info_poll();
        _dev_set_bat_soc_poll();
        _dev_set_charge_state_poll();
        _dev_set_box_lid_state_poll();

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
        LOG_E("extcom mod already inited!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    _objects_init();

    _extcom_mod.use_usb_cdc_flag = false;
    hl_hal_console_detach(false);

    LOG_I("extcom mod init success");

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
        LOG_E("extcom mod not init!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    hl_mod_extcom_stop();

    LOG_I("extcom mod deinit success");

    _extcom_mod.init_flag = false;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_start(void)
{
    rt_err_t rt_err;

    if (_extcom_mod.init_flag == false) {
        LOG_E("extcom mod not init!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (_extcom_mod.start_flag == true) {
        LOG_W("extcom mod already start!");
        return HL_MOD_EXTCOM_FUNC_OK;
    }

    _extcom_mod.tx1_probe_flag = false;
    _extcom_mod.tx2_probe_flag = false;
    _extcom_mod.rx_probe_flag  = false;

    _extcom_mod.box_lid_state                       = 0;
    _extcom_mod.box_bat_info                        = 0xFF;
    _extcom_mod.box_charge_state                    = 0;
    _extcom_mod.temp_flag.box_bat_info_get_flag     = false;
    _extcom_mod.temp_flag.box_charge_state_get_flag = false;

    _extcom_mod.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_extcom_mod.extcom_thread), "extcom_thread", _extcom_thread_entry, RT_NULL,
                            extcom_thread_stack, sizeof(extcom_thread_stack), 6, 32);
    if (rt_err == RT_ERROR) {
        LOG_E("extcom thread init failed");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    rt_thread_startup(&(_extcom_mod.extcom_thread));

    LOG_I("extcom mod start success!");

    _extcom_mod.start_flag = true;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_stop(void)
{
    if (_extcom_mod.init_flag == false) {
        LOG_E("extcom mod not init!");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    if (_extcom_mod.start_flag == false) {
        LOG_W("extcom mod not start!");
        return HL_MOD_EXTCOM_FUNC_OK;
    }

    _extcom_mod.thread_exit_flag = 1;

    LOG_I("wait extcom thread exit");

    while (_extcom_mod.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    LOG_I("extcom mod stop success!");

    _extcom_mod.start_flag = false;

    return HL_MOD_EXTCOM_FUNC_OK;
}

int hl_mod_extcom_ctrl(hl_mod_extcom_op_e op, void* arg, int arg_size)
{
    int     ret;
    uint8_t state;

    if (_extcom_mod.init_flag == false) {
        LOG_E("extcom mod not init!\n");
        return HL_MOD_EXTCOM_FUNC_ERR;
    }

    switch (op) {
        case HL_MOD_EXTCOM_ENTER_UPGRADE: {
            if (arg_size != sizeof(bool)) {
                LOG_E("size err, ctrl arg need <bool> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            ret = _handle_op_enter_upgrade((bool*)arg);
            if (ret == HL_MOD_EXTCOM_FUNC_ERR) {
                return HL_MOD_EXTCOM_FUNC_ERR;
            }
        } break;
        case HL_MOD_EXTCOM_START_TX1_PROBE: {
            if (arg_size != sizeof(bool)) {
                LOG_E("size err, ctrl arg need <bool> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.tx1_probe_flag = *(bool*)arg;
        } break;
        case HL_MOD_EXTCOM_START_TX2_PROBE: {
            if (arg_size != sizeof(bool)) {
                LOG_E("size err, ctrl arg need <bool> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.tx2_probe_flag = *(bool*)arg;
        } break;
        case HL_MOD_EXTCOM_START_RX_PROBE: {
            if (arg_size != sizeof(bool)) {
                LOG_E("size err, ctrl arg need <bool> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            _extcom_mod.rx_probe_flag = *(bool*)arg;
        } break;
        case HL_MOD_EXTCOM_SET_BOX_BAT_INFO: {
            if (arg_size != sizeof(uint8_t)) {
                LOG_E("size err, ctrl arg need <uint8_t> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            if (_extcom_mod.box_bat_info != *(uint8_t*)arg) {
                _extcom_mod.box_bat_info                       = *(uint8_t*)arg;
                _extcom_mod.temp_flag.box_bat_info_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_box_bat_info_flag = false;
            }
        } break;
        case HL_MOD_EXTCOM_SET_BOX_CHARGE_STATE: {
            if (arg_size != sizeof(uint8_t)) {
                LOG_E("size err, ctrl arg need <uint8_t> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            if (_extcom_mod.box_charge_state != *(uint8_t*)arg) {
                _extcom_mod.box_charge_state                       = *(uint8_t*)arg;
                _extcom_mod.temp_flag.box_charge_state_get_flag    = true;
                _extcom_mod.temp_flag.rx_set_box_charge_state_flag = false;
            }
        } break;
        case HL_MOD_EXTCOM_SET_BOX_LID_STATE: {
            if (arg_size != sizeof(uint8_t)) {
                LOG_E("size err, ctrl arg need <uint8_t> type pointer!");
                return HL_MOD_EXTCOM_FUNC_ERR;
            }

            state = *(uint8_t*)arg;

            if (_extcom_mod.box_lid_state != state) {
                _extcom_mod.box_lid_state                       = state;
                _extcom_mod.temp_flag.rx_set_box_lid_state_flag = false;
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