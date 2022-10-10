/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-06     luzhanghao       the first version
 */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "n32l40x.h"
#include "hl_util_fifo.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONSOLE_UART        "uart1"

#define FIFO_BUFSZ 128  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct rt_semaphore shell_rx_sem; /* defined console rx semaphore*/

static hl_util_fifo_t hl_console_fifo;

static char fifo_buf[FIFO_BUFSZ] = { 0 };

/* Private functions ---------------------------------------------------------*/
//#ifdef RT_USING_CONSOLE

static void hl_hal_console_callback(uint8_t data)
{
    if (1 == hl_util_fifo_write(&hl_console_fifo, &data, 1)) {
        rt_sem_release(&shell_rx_sem);
    }
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_init(void)
{
    USART_InitType USART_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    hl_util_fifo_init(&hl_console_fifo, fifo_buf, sizeof(fifo_buf));
    
    /* ��ʼ�����ڽ������ݵ��ź��� */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
    
    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    /* Enable USARTx Clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    
    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    
    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    
    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
    
    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USARTx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USARTx Receive interrupts */
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
    /* Enable the USARTx */
    USART_Enable(USART1, ENABLE);
}

/**
 * @brief This function deinit rtx1290 driver
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
void hl_hal_console_deinit(void)
{

}

/**
 * @brief This function is the console output interface
 */
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            USART_SendData(USART1, (uint8_t)a);
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
        }
        USART_SendData(USART1, *(str + i));
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
    }
}

/**
 * @brief This function is the console input interface
 */
char rt_hw_console_getchar(void)
{
    char ch = -1;

    while (1 != hl_util_fifo_read(&hl_console_fifo, (uint8_t *)&ch, 1)) {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    }
    
    return ch;
}

//#endif /* end of RT_USING_CONSOLE */

void USART1_IRQHandler(void)
{
    uint8_t receive_data;    // ��������
    
    /* enter interrupt */
    rt_interrupt_enter();          //���ж���һ��Ҫ������Ժ����������ж�
    
    if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = USART_ReceiveData(USART1);
        hl_hal_console_callback(receive_data);
    } else {
        receive_data = USART_ReceiveData(USART1);
    }
    
    /* leave interrupt */
    rt_interrupt_leave();    //���ж���һ��Ҫ������Ժ������뿪�ж�
}
