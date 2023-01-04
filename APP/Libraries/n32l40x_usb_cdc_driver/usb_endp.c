/*****************************************************************************
 * Copyright (c) 2022, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file usb_endp.c
 * @author Nations
 * @version v1.2.0
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 */
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_bot.h"
#include "usb_istr.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "hl_util_fifo.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern uint8_t  USB_Tx_State;

extern hl_util_fifo_t hl_cdc_out_fifo;
/**
 * @brief EP1 IN Callback Routine.
 */
void EP1_IN_Callback(void)
{
    Mass_Storage_In();
}

/**
 * @brief EP2 OUT Callback Routine.
 */
void EP2_OUT_Callback(void)
{
    Mass_Storage_Out();
}


/**
 * @brief  EP1 IN Callback Routine.
 */
void EP5_IN_Callback (void)
{
    //uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;
    uint8_t buffer[64];

    if (USB_Tx_State == 1)
    {
        if (hl_util_fifo_data_size(&hl_cdc_out_fifo) == 0) 
        {
            USB_Tx_State = 0;
        }
        else 
        {
            if (hl_util_fifo_data_size(&hl_cdc_out_fifo) > VIRTUAL_COM_PORT_DATA_SIZE)
            {
                //read 64bytes
                USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;
                hl_util_fifo_read(&hl_cdc_out_fifo, buffer, USB_Tx_length);
 
            }
            else 
            {
                // read all bytes left
            USB_Tx_length = hl_util_fifo_data_size(&hl_cdc_out_fifo);
            hl_util_fifo_read(&hl_cdc_out_fifo, buffer, USB_Tx_length);
            }
            USB_CopyUserToPMABuf(buffer, ENDP5_TXADDR, USB_Tx_length);
            USB_SetEpTxCnt(ENDP5, USB_Tx_length);
            USB_SetEpTxValid(ENDP5); 
        }
    }
}

/**
 * @brief  EP3 OUT Callback Routine.
 */
void EP3_OUT_Callback(void)
{
    uint16_t USB_Rx_Cnt;
    uint8_t data_read[128];

    /* Get the received data buffer and update the counter */
    USB_Rx_Cnt = USB_SilRead(EP3_OUT, data_read);

    /* USB data will be immediately processed, this allow next USB traffic being 
    NAKed till the end of the USART Xfer */

    USB_CDC_Recv_Data_Save(data_read, USB_Rx_Cnt);

    /* Enable the receive of data on EP3 */
    USB_SetEpRxValid(ENDP3);
}

/**
 * @brief  SOF Callback Routine.
 */
void SOF_Callback(void)
{
    static uint32_t FrameCount = 0;

    if(bDeviceState == CONFIGURED)
    {
        if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
        {
            /* Reset the frame counter */
            FrameCount = 0;

            /* Check the data to be sent through IN pipe */
            Handle_USBAsynchXfer();
        }
    }
}

/**
 * ERR_Callback
 * @brief 添加USB错误则去初始化USB
 * @author yangxy (rd52@hollyland-tech.com)
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2021-12-09      <td>yangxy     <td>新建
 * </table>
 */
void ERR_Callback(void)
{
  USB_Interrupts_Config(DISABLE);
}