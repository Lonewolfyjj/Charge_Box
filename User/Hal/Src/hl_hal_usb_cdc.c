/**
 * @file hl_hal_usb_cdc.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-20
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
 * <tr><td>2022-10-20     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_usb_cdc.h"
#include "hl_util_fifo.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define IN_FIFO_BUFSZ 512
#define OUT_FIFO_BUFSZ 512

/* variables -----------------------------------------------------------------*/

hl_util_fifo_t hl_cdc_in_fifo;
hl_util_fifo_t hl_cdc_out_fifo;

static char in_fifo_buf[IN_FIFO_BUFSZ] = { 0 };
static char out_fifo_buf[OUT_FIFO_BUFSZ] = { 0 };

static bool init_flag = false;

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_hal_usb_cdc_init()
{
    if (init_flag == true) {
        return HL_HAL_USB_CDC_FUNC_OK;
    }

    hl_util_fifo_init(&hl_cdc_in_fifo, in_fifo_buf, sizeof(in_fifo_buf));
    hl_util_fifo_init(&hl_cdc_out_fifo, out_fifo_buf, sizeof(out_fifo_buf));

    if(USB_Config(SYSCLK_VALUE_48MHz) == SUCCESS)
    {
        USB_Init();

    //    if (bDeviceState != CONFIGURED)
    //    {
    //        USB_Interrupts_Config(DISABLE);
    //    }
    } else {
        return HL_HAL_USB_CDC_FUNC_ERR;
    }

    init_flag = true;

    return HL_HAL_USB_CDC_FUNC_OK;
}

int hl_hal_usb_cdc_deinit()
{
    if (init_flag == false) {
        return HL_HAL_USB_CDC_FUNC_OK;
    }

    init_flag = false;
    
    return HL_HAL_USB_CDC_FUNC_OK;
}

int hl_hal_usb_cdc_read(uint8_t *buf, uint32_t len, uint32_t *p_size)
{
    if (init_flag == false) {
        return HL_HAL_USB_CDC_FUNC_ERR;
    }

    int ret;

    ret = hl_util_fifo_read(&hl_cdc_in_fifo, buf, len);

    *p_size = ret;

    return HL_HAL_USB_CDC_FUNC_OK;
}

int hl_hal_usb_cdc_write(uint8_t *buf, uint32_t len, uint32_t *p_size)
{
    if (init_flag == false) {
        return HL_HAL_USB_CDC_FUNC_ERR;
    }
    
    int ret;

    ret = hl_util_fifo_write(&hl_cdc_out_fifo, buf, len);

    *p_size = ret;

    return HL_HAL_USB_CDC_FUNC_OK;
}

/*
 * EOF
 */