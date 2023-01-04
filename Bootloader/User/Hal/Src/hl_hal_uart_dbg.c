/**
 * @file hl_hal_uart_dbg.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-23
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
 * <tr><td>2022-12-23     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_uart.h"
#include "n32l40x.h"
#include <stdio.h>

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

static void _uart1_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure usartx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    /* Configure usartx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

static int _uart1_config(uint32_t baudrate)
{
    USART_InitType USART_InitStructure;

    /* Enable usartx Clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);

    _uart1_gpio_init();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = baudrate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_TX;    //| USART_MODE_RX;
#if 0
    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    /* Configure usartx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable usartx Receive interrupts */
    //USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
    /* Enable the usartx */
    USART_Enable(USART1, ENABLE);

    return true;
}

/* Exported functions --------------------------------------------------------*/

void hl_hal_uart_dbg_init()
{
    _uart1_gpio_init();
    _uart1_config(115200);
}

void hl_hal_uart_dbg_deinit()
{
    USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
    USART_Enable(USART1, DISABLE);
    USART_DeInit(USART1);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, DISABLE);
}

/**
 * @brief 重定义fputc函数
 * @param [in] ch 输入字符
 * @param [in] f 文件指针
 * @return int 
 * @date 2022-12-23
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-23      <td>yijiujun     <td>新建
 * </table>
 */
int fputc(int ch, FILE* fp)
{
    uint8_t p_data = (uint8_t)ch;
    
    USART_SendData(USART1, p_data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);

    return 0;
}

/*
 * EOF
 */