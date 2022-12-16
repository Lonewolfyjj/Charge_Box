/************************************************************************
* 版权所有 (C)2001, 深圳市昊一源科技有限公司。
* 
* 文件名称：
* 文件标识：
* 内容摘要：
* 其它说明：
* 当前版本：			V1.0.0.0
* 作     者：		lzh 
* 完成日期：			2021/06/16
* 
* 修改记录1： 
*    修改日期： 2019/07/06
*    版 本 号： V1.0.0.0
*    修 改 人： XXXX
*    修改内容： 
* 修改记录2：…
************************************************************************/
#include "hl_hal_uart.h"
#include "n32l40x.h"

/**************************************************************************
 *                                宏                                       *
 **************************************************************************/

/**************************************************************************
 *                             数据类型                                       *
 **************************************************************************/

/**************************************************************************
 *                               常量                                       *
 **************************************************************************/

/**************************************************************************
 *                            局部函数原型                                      *
 **************************************************************************/

//

/**************************************************************************
 *                             全局变量                                       *
 **************************************************************************/
/* 串口回调函数指针 */
static uart_rcv_cb_t uart1_rcv_cb  = RT_NULL;
static uart_rcv_cb_t uart2_rcv_cb  = RT_NULL;
static uart_rcv_cb_t uart3_rcv_cb  = RT_NULL;
static uart_rcv_cb_t uart5_rcv_cb = RT_NULL;
static uart_rcv_cb_t lpuart_rcv_cb = RT_NULL;

/* 串口工作标志 */
static bool s_uart1_is_active  = false;
static bool s_uart2_is_active  = false;
static bool s_uart3_is_active  = false;
static bool s_uart5_is_active  = false;
static bool s_lpuart_is_active = false;

/**************************************************************************
 *                              中断函数实现                              *
 **************************************************************************/
void USART1_IRQHandler(void)
{
    uint8_t receive_data;  // 接收数据

    /* enter interrupt */
    rt_interrupt_enter();  //在中断中一定要调用这对函数，进入中断

    if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = USART_ReceiveData(USART1);
        if (uart1_rcv_cb != RT_NULL) {
            uart1_rcv_cb(receive_data);
        }
    } else {
        receive_data = USART_ReceiveData(USART1);
    }

    /* leave interrupt */
    rt_interrupt_leave();  //在中断中一定要调用这对函数，离开中断
}

void USART2_IRQHandler(void)
{
    uint8_t receive_data;  // 接收数据

    /* enter interrupt */
    rt_interrupt_enter();  //在中断中一定要调用这对函数，进入中断

    if (USART_GetIntStatus(USART2, USART_INT_RXDNE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = USART_ReceiveData(USART2);
        if (uart2_rcv_cb != RT_NULL) {
            uart2_rcv_cb(receive_data);
        }
    } else {
        receive_data = USART_ReceiveData(USART2);
    }

    /* leave interrupt */
    rt_interrupt_leave();  //在中断中一定要调用这对函数，离开中断
}

void USART3_IRQHandler(void)
{
    uint8_t receive_data;  // 接收数据

    /* enter interrupt */
    rt_interrupt_enter();  //在中断中一定要调用这对函数，进入中断

    if (USART_GetIntStatus(USART3, USART_INT_RXDNE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = USART_ReceiveData(USART3);
        if (uart3_rcv_cb != RT_NULL) {
            uart3_rcv_cb(receive_data);
        }
    } else {
        receive_data = USART_ReceiveData(USART3);
    }

    /* leave interrupt */
    rt_interrupt_leave();  //在中断中一定要调用这对函数，离开中断
}

void UART5_IRQHandler(void)
{
    uint8_t receive_data;  // 接收数据

    /* enter interrupt */
    rt_interrupt_enter();  //在中断中一定要调用这对函数，进入中断

    if (USART_GetIntStatus(UART5, USART_INT_RXDNE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = USART_ReceiveData(UART5);
        if (uart5_rcv_cb != RT_NULL) {
            uart5_rcv_cb(receive_data);
        }
    } else {
        receive_data = USART_ReceiveData(UART5);
    }

    /* leave interrupt */
    rt_interrupt_leave();  //在中断中一定要调用这对函数，离开中断
}

void LPUART_IRQHandler(void)
{
    uint8_t receive_data;  // 接收数据

    /* enter interrupt */
    rt_interrupt_enter();  //在中断中一定要调用这对函数，进入中断

    if (LPUART_GetIntStatus(LPUART_INT_FIFO_NE) != RESET) {
        /* Read one byte from the receive data register */
        receive_data = LPUART_ReceiveData();
        if (lpuart_rcv_cb != RT_NULL) {
            lpuart_rcv_cb(receive_data);
        }
    } else {
        receive_data = LPUART_ReceiveData();
    }

    /* leave interrupt */
    rt_interrupt_leave();  //在中断中一定要调用这对函数，离开中断
}

/**************************************************************************
 *                              局部函数实现                                    *
 **************************************************************************/

static void _uart1_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

static void _uart1_gpio_deinit(void)
{
}

/**************************************************************************
* 函数名称: 	hl_hal_uart1_config
* 功能描述: 	串口1配置
* 输入参数: BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
static int hl_hal_uart1_config(uint32_t baudrate)
{
    USART_InitType USART_InitStructure;

    /* Enable USARTx Clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);

    _uart1_gpio_init();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = baudrate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USARTx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USARTx Receive interrupts */
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
    /* Enable the USARTx */
    USART_Enable(USART1, ENABLE);

    s_uart1_is_active = true;
    return true;
}

static void _uart2_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART2;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6_USART2;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

static void _uart2_gpio_deinit(void)
{
}

/**************************************************************************
* 函数名称: 	hl_hal_uart3_config
* 功能描述: 	串口3配置
* 输入参数: BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
static int hl_hal_uart2_config(uint32_t baudrate)
{
    USART_InitType USART_InitStructure;

    /* Enable USARTx Clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);

    _uart2_gpio_init();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = baudrate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USARTx */
    USART_Init(USART2, &USART_InitStructure);
    /* Enable USARTx Receive interrupts */
    USART_ConfigInt(USART2, USART_INT_RXDNE, ENABLE);
    /* Enable the USARTx */
    USART_Enable(USART2, ENABLE);

    s_uart2_is_active = true;
    return true;
}

static void _uart3_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF0_USART3;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_USART3;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

static void _uart3_gpio_deinit(void)
{
}

/**************************************************************************
* 函数名称: 	hl_hal_uart3_config
* 功能描述: 	串口3配置
* 输入参数: BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
static int hl_hal_uart3_config(uint32_t baudrate)
{
    USART_InitType USART_InitStructure;

    /* Enable USARTx Clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, ENABLE);

    _uart3_gpio_init();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = baudrate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USARTx */
    USART_Init(USART3, &USART_InitStructure);
    /* Enable USARTx Receive interrupts */
    USART_ConfigInt(USART3, USART_INT_RXDNE, ENABLE);
    /* Enable the USARTx */
    USART_Enable(USART3, ENABLE);

    s_uart3_is_active = true;
    return true;
}

static void _uart5_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6_UART5;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6_UART5;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

static void _uart5_gpio_deinit(void)
{
}

/**************************************************************************
* 函数名称: 	hl_hal_uart3_config
* 功能描述: 	串口3配置
* 输入参数: BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
static int hl_hal_uart5_config(uint32_t baudrate)
{
    USART_InitType USART_InitStructure;

    /* Enable USARTx Clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART5, ENABLE);

    _uart5_gpio_init();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = baudrate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USARTx */
    USART_Init(UART5, &USART_InitStructure);
    /* Enable USARTx Receive interrupts */
    USART_ConfigInt(UART5, USART_INT_RXDNE, ENABLE);
    /* Enable the USARTx */
    USART_Enable(UART5, ENABLE);

    s_uart5_is_active = true;
    return true;
}

static void _lpuart_gpio_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6_LPUART;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    /* Configure USARTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6_LPUART;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

static void _lpuart_gpio_deinit(void)
{
}

/**************************************************************************
* 函数名称: 	hl_hal_uart3_config
* 功能描述: 	串口3配置
* 输入参数: BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
static int hl_hal_lpuart_config(uint32_t baudrate)
{
    LPUART_InitType LPUART_InitStructure;

    RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_APB1);

    /* Enable USARTx Clock */
    RCC_EnableRETPeriphClk(RCC_RET_PERIPH_LPUART, ENABLE);

    _lpuart_gpio_init();

    LPUART_DeInit();
    LPUART_StructInit(&LPUART_InitStructure);
    LPUART_InitStructure.BaudRate            = baudrate;
    LPUART_InitStructure.Parity              = LPUART_PE_NO;
    LPUART_InitStructure.RtsThreshold        = LPUART_RTSTH_FIFOFU;
    LPUART_InitStructure.HardwareFlowControl = LPUART_HFCTRL_NONE;
    LPUART_InitStructure.Mode                = LPUART_MODE_RX | LPUART_MODE_TX;
    /* Configure LPUART */
    LPUART_Init(&LPUART_InitStructure);

    NVIC_InitType NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = LPUART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USARTx Receive interrupts */
    LPUART_ConfigInt(LPUART_INT_FIFO_NE, ENABLE);

    s_lpuart_is_active = true;
    return true;
}

/**************************************************************************
 *                              全局函数实现                                    *
 **************************************************************************/
/**************************************************************************
* 函数名称: 	hl_hal_uart_receive_reg
* 功能描述: 	串口接收回调函数注册
* 输入参数: USARTx: 串口枚举编号				art_rcv_cb_t：回调函数
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期			版本号		   修改人		  修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
bool hl_hal_uart_receive_reg(hl_hal_uart_numb_e USARTx, uart_rcv_cb_t rcv_cb_handle)
{
    switch (USARTx) {
        case HL_HAL_UART_NUMB_1:
            uart3_rcv_cb = rcv_cb_handle;
            break;
            
        case HL_HAL_UART_NUMB_2:
            uart5_rcv_cb = rcv_cb_handle;
            break;

        case HL_HAL_UART_NUMB_3:
            uart2_rcv_cb = rcv_cb_handle;
            break;

        case HL_HAL_UART_NUMB_4:
            lpuart_rcv_cb = rcv_cb_handle;
            break;
        case HL_HAL_UART_NUMB_5:
            uart1_rcv_cb = rcv_cb_handle;
            break;

        default:
            return false;
            break;
    }

    return true;
}

/**************************************************************************
* 函数名称: 	hl_hal_uart_config
* 功能描述: 	串口配置
* 输入参数: USARTx: 串口枚举编号	  BaudRate:串口波特率
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期			版本号		   修改人		  修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
bool hl_hal_uart_init(hl_hal_uart_numb_e USARTx, uint32_t baudrate)
{
    switch (USARTx) {

        case HL_HAL_UART_NUMB_1:
            return hl_hal_uart3_config(baudrate);
            break;

        case HL_HAL_UART_NUMB_2:
            return hl_hal_uart5_config(baudrate);
            break;

        case HL_HAL_UART_NUMB_3:
            return hl_hal_uart2_config(baudrate);
            break;

        case HL_HAL_UART_NUMB_4:
            return hl_hal_lpuart_config(baudrate);
            break;

        case HL_HAL_UART_NUMB_5:
            return hl_hal_uart1_config(baudrate);
            break;

        default:
            return false;
            break;
    }
}

/**************************************************************************
* 函数名称: 	hl_hal_uart_deint
* 功能描述: 	串口去始化
* 输入参数: USARTx: 串口枚举编号 
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
bool hl_hal_uart_deinit(hl_hal_uart_numb_e USARTx)
{
    switch (USARTx) {
        case HL_HAL_UART_NUMB_1:
            s_uart3_is_active = false;
            USART_ConfigInt(USART3, USART_INT_RXDNE, DISABLE);
            USART_Enable(USART3, DISABLE);
            USART_DeInit(USART3);
            _uart3_gpio_deinit();
            RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, DISABLE);
            break;

        case HL_HAL_UART_NUMB_2:
            s_uart5_is_active = false;
            USART_ConfigInt(UART5, USART_INT_RXDNE, DISABLE);
            USART_Enable(UART5, DISABLE);
            USART_DeInit(UART5);
            _uart5_gpio_deinit();
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART5, DISABLE);
            break;

        case HL_HAL_UART_NUMB_3:
            s_uart2_is_active = false;
            USART_ConfigInt(USART2, USART_INT_RXDNE, DISABLE);
            USART_Enable(USART2, DISABLE);
            USART_DeInit(USART2);
            _uart2_gpio_deinit();
            RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, DISABLE);
            break;

        case HL_HAL_UART_NUMB_4:
            s_lpuart_is_active = false;
            LPUART_ConfigInt(LPUART_INT_FIFO_NE, DISABLE);
            LPUART_DeInit();
            _lpuart_gpio_deinit();
            RCC_EnableRETPeriphClk(RCC_RET_PERIPH_LPUART, DISABLE);
            break;

        case HL_HAL_UART_NUMB_5:
            s_uart1_is_active = false;
            USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
            USART_Enable(USART1, DISABLE);
            USART_DeInit(USART1);
            _uart1_gpio_deinit();
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, DISABLE);
            break;

        default:
            return false;
            break;
    }

    return true;
}

/**************************************************************************
* 函数名称: 	hl_hal_uart_send
* 功能描述: 	串口发送数据
* 输入参数: USARTx: 串口枚举编号				p_data：数据指针  		size：数据大小
* 输出参数: 无
* 返 回 值: true：成功				false：失败
* 其它说明: 
* 修改日期          版本号          修改人	      修改内容
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 卢彰豪	      创建
**************************************************************************/
bool hl_hal_uart_send(hl_hal_uart_numb_e USARTx, uint8_t* p_data, uint16_t size)
{
    if (p_data == RT_NULL) {
        return false;
    }

    switch (USARTx) {
        case HL_HAL_UART_NUMB_1:
            if (s_uart3_is_active == true) {
                while (size--) {
                    USART_SendData(USART3, *p_data);
                    p_data++;
                    while (USART_GetFlagStatus(USART3, USART_FLAG_TXDE) == RESET)
                        ;
                }
            }
            break;

        case HL_HAL_UART_NUMB_2:
            if (s_uart5_is_active == true) {
                while (size--) {
                    USART_SendData(UART5, *p_data);
                    p_data++;
                    while (USART_GetFlagStatus(UART5, USART_FLAG_TXDE) == RESET)
                        ;
                }
            }
            break;

        case HL_HAL_UART_NUMB_3:
            if (s_uart2_is_active == true) {
                while (size--) {
                    USART_SendData(USART2, *p_data);
                    p_data++;
                    while (USART_GetFlagStatus(USART2, USART_FLAG_TXDE) == RESET)
                        ;
                }
            }
            break;

        case HL_HAL_UART_NUMB_4:
            if (s_lpuart_is_active == true) {
                while (size--) {
                    LPUART_SendData(*p_data);
                    p_data++;
                    while (LPUART_GetFlagStatus(LPUART_FLAG_TXC) == RESET)
                        ;
                    LPUART_ClrFlag(LPUART_FLAG_TXC);
                }
            }
            break;

        case HL_HAL_UART_NUMB_5:
            if (s_uart1_is_active == true) {
                while (size--) {
                    USART_SendData(USART1, *p_data);
                    p_data++;
                    while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET)
                        ;
                }
            }
            break;

        default:
            return false;
            break;
    }

    return true;
}
