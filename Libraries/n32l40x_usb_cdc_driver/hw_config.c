/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
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
 * @file hw_config.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
/* Includes ------------------------------------------------------------------*/

#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "hl_util_fifo.h"
#include "rtthread.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
USART_InitType USART_InitStructure;
EXTI_InitType EXTI_InitStructure;

uint8_t  USB_Tx_State = 0; // keep

extern hl_util_fifo_t hl_cdc_out_fifo;
extern hl_util_fifo_t hl_cdc_in_fifo;

/* Extern variables ----------------------------------------------------------*/
extern LINE_CODING linecoding;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : Cfg_McoOut
 * Description    : Configures the main clock out.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void Cfg_McoOut(void)
 {
    EXTI_InitType EXTI_InitStructure;

    /* Configure the EXTI line 17 connected internally to the USB IP */
    EXTI_ClrITPendBit(EXTI_LINE17);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE17; 
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
}


/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
    RCC_DeInit();

    /*Enable HSE*/
    RCC->CTRL |= RCC_CTRL_HSEEN;
    while((RCC->CTRL & RCC_CTRL_HSERDF) != RCC_CTRL_HSERDF);

    /*Set PLL MUL*/
    RCC_ConfigPll(RCC_PLL_SRC_HSE_DIV1, RCC_PLL_MUL_3, RCC_PLLDIVCLK_DISABLE);
    /*Enable PLL*/
    RCC->CTRL |= RCC_CTRL_PLLEN;
    while((RCC->CTRL & RCC_CTRL_PLLRDF) != RCC_CTRL_PLLRDF); 

    /*Set AHB/APB1/APB2*/
    RCC->CFG |= RCC_CFG_AHBPRES_DIV1;
    RCC->CFG |= RCC_CFG_APB1PRES_DIV2;
    RCC->CFG |= RCC_CFG_APB2PRES_DIV1;

    FLASH->AC &= (uint32_t)((uint32_t)~FLASH_AC_LATENCY);
    FLASH->AC |= (uint32_t)(FLASH_AC_LATENCY_3);

    /* Select PLL as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t) ~(RCC_CFG_SCLKSW));
    RCC->CFG |= (uint32_t)RCC_CFG_SCLKSW_PLL;
    while ((RCC->CFG & RCC_CFG_SCLKSTS) != RCC_CFG_SCLKSTS_PLL) 
    {
    }
    /*For CLK Check*/
    Cfg_McoOut();
}

/*******************************************************************************
 * Function Name  : Set_USBClock
 * Description    : Configures USB Clock input (48MHz).
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
ErrorStatus Set_USBClock(uint32_t sysclk)
{
    ErrorStatus status = SUCCESS;

    switch(sysclk)
    {
    case SYSCLK_VALUE_48MHz: 
        RCC_ConfigUsbClk(RCC_USBCLK_SRC_PLLCLK_DIV1);
        break;

    case SYSCLK_VALUE_72MHz: 
        RCC_ConfigUsbClk(RCC_USBCLK_SRC_PLLCLK_DIV1_5);
        break;

    case SYSCLK_VALUE_96MHz: 
        RCC_ConfigUsbClk(RCC_USBCLK_SRC_PLLCLK_DIV2);
        break;

    default:
        status = ERROR;
        break;
    }

    return status;
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
    /* Set the device state to suspend */
    bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
    USB_DeviceMess *pInfo = &Device_Info;

    /* Set the device state to the correct state */
    if (pInfo->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
    else
    {
        bDeviceState = ATTACHED;
    }
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(uint8_t state)
{
    NVIC_InitType NVIC_InitStructure;

    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USB interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = state;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB Wake-up interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : USB_CDC_Recv_Data_Save.
* Description    : send the received data from USB to the UART 0.
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send.
* Return         : none.
*******************************************************************************/
void USB_CDC_Recv_Data_Save(uint8_t* data_buffer, uint8_t Nb_bytes)
{
    hl_util_fifo_write(&hl_cdc_in_fifo, data_buffer, Nb_bytes);
    // rt_kprintf("data len = %d\n", Nb_bytes);
    // int i = 0;
    // for (i = 0; i < Nb_bytes; i++) {
    //     rt_kprintf("%02x ", data_buffer[i]);
    // }
    // rt_kprintf("\n");
}

/*******************************************************************************
* Function Name  : Handle_USBAsynchXfer.
* Description    : send data to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void Handle_USBAsynchXfer (void)
{
  
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;
    uint8_t buffer[64];

    if(USB_Tx_State != 1)
    {
        //wlvl = 0
        if(hl_util_fifo_data_size(&hl_cdc_out_fifo) == 0) 
        {
            USB_Tx_State = 0; 
            return;
        }

        //wlvl
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
        USB_Tx_State = 1; 
        USB_CopyUserToPMABuf(buffer, ENDP1_TXADDR, USB_Tx_length);
        USB_SetEpTxCnt(ENDP1, USB_Tx_length);
        USB_SetEpTxValid(ENDP1); 
        
    }

}
/*******************************************************************************
* Function Name  : UART_To_USB_Send_Data.
* Description    : send the received data from UART 0 to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
// hl_note: 每次中断调用这个函数来存数据 USART_Rx_ptr_in：写缓存指针  USART_Rx_ptr_out：读缓存指针
// void USART_To_USB_Send_Data(void)
// {
//     if (linecoding.datatype == 7)
//     {
//         USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART1) & 0x7F;
//     }
//     else if (linecoding.datatype == 8)
//     {
//         USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART1);
//     }

//     USART_Rx_ptr_in++;

//     /* To avoid buffer overflow */
//     if(USART_Rx_ptr_in == USART_RX_DATA_SIZE)
//     {
//         USART_Rx_ptr_in = 0;
//     }
// }

/*******************************************************************************
 * Function Name  : USB_Config.
 * Description    : Configures the USB interrupts.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
ErrorStatus USB_Config(uint32_t sysclk)
{
    ErrorStatus status = SUCCESS;

    USB_Interrupts_Config(ENABLE);

    if(Set_USBClock(sysclk) == SUCCESS)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USB, ENABLE);
        status = SUCCESS;
    }
    else
    {
        status = ERROR;
    }

    return status;
}


