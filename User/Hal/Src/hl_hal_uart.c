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

/**************************************************************************
 *                                宏                                       *
 **************************************************************************/

//

/**************************************************************************
 *                             数据类型                                       *
 **************************************************************************/

//

/**************************************************************************
 *                               常量                                       *
 **************************************************************************/

//

/**************************************************************************
 *                            局部函数原型                                      *
 **************************************************************************/

//


/**************************************************************************
 *                             全局变量                                       *
 **************************************************************************/
/* 调试串口句柄       */
static UART_HandleTypeDef s_huart1 = {0};
static UART_HandleTypeDef s_huart2 = {0};
static UART_HandleTypeDef s_huart3 = {0};

/* 串口回调函数指针 */
static uart_rcv_cb_t uart1_rcv_cb = NULL;
static uart_rcv_cb_t uart2_rcv_cb = NULL;
static uart_rcv_cb_t uart3_rcv_cb = NULL;

/* 串口工作标志 */
static bool s_uart1_is_active = false;
static bool s_uart2_is_active = false;
static bool s_uart3_is_active = false;

/**************************************************************************
 *                              中断函数实现                              *
 **************************************************************************/ 
void USART1_IRQHandler(void)
{
    uint8_t receive_data = 0;    // 缓存接收数据    
    
    /* 接收中断 */
    if((__HAL_UART_GET_FLAG(&s_huart1, UART_FLAG_RXNE) != RESET)) {
         
        /* 串口数据接收 */
        receive_data = (uint8_t) READ_REG(s_huart1.Instance->RDR);

        /* 串口处理回调函数，主要作用是将接收数据缓存到fifo中 */
        if (uart1_rcv_cb != NULL) {
            uart1_rcv_cb(receive_data); 
        }
    }
    
    /* 清除溢出中断 */
    if((__HAL_UART_GET_FLAG(&s_huart1, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart1.Instance->RDR);
    }
    
    /* 清除中断标志 */
    HAL_UART_IRQHandler(&s_huart1);
}

void USART2_IRQHandler(void)
{
    uint8_t receive_data = 0;    // 缓存接收数据    
    
    /* 接收中断 */
    if((__HAL_UART_GET_FLAG(&s_huart2, UART_FLAG_RXNE) != RESET)) {
         
        /* 串口数据接收 */
        receive_data = (uint8_t) READ_REG(s_huart2.Instance->RDR);

        /* 串口处理回调函数，主要作用是将接收数据缓存到fifo中 */
        if (uart2_rcv_cb != NULL) {
            uart2_rcv_cb(receive_data); 
        }
    }
    
    /* 清除溢出中断 */
    if((__HAL_UART_GET_FLAG(&s_huart2, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart2.Instance->RDR);
    }
    
    /* 清除中断标志 */
    HAL_UART_IRQHandler(&s_huart2);
}

void USART3_IRQHandler(void)
{
    uint8_t receive_data = 0;    // 缓存接收数据    
    
    /* 接收中断 */
    if((__HAL_UART_GET_FLAG(&s_huart3, UART_FLAG_RXNE) != RESET)) {
         
        /* 串口数据接收 */
        receive_data = (uint8_t) READ_REG(s_huart3.Instance->RDR);

        /* 串口处理回调函数，主要作用是将接收数据缓存到fifo中 */
        if (uart3_rcv_cb != NULL) {
            uart3_rcv_cb(receive_data); 
        }
    }
    
    /* 清除溢出中断 */
    if((__HAL_UART_GET_FLAG(&s_huart3, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart3.Instance->RDR);
    }
    
    /* 清除中断标志 */
    HAL_UART_IRQHandler(&s_huart3);
}

/**************************************************************************
 *                              局部函数实现                                    *
 **************************************************************************/ 
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
    __HAL_RCC_USART1_CLK_ENABLE();
    
    s_huart1.Instance          = USART1;
    s_huart1.Init.BaudRate     = baudrate;
    s_huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    s_huart1.Init.StopBits     = UART_STOPBITS_1;
    s_huart1.Init.Parity       = UART_PARITY_NONE;
    s_huart1.Init.Mode         = UART_MODE_TX_RX;
    s_huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    s_huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    if (HAL_UART_Init(&s_huart1) != HAL_OK) {
        // Error_Handler();
        s_uart1_is_active = false;
        return false;
    }  
    
    /* 串口中断配置 */
    __HAL_UART_ENABLE_IT(&s_huart1, UART_IT_RXNE);              // 开启接收中断
    s_uart1_is_active = true;
	return true;
}

/**************************************************************************
* 函数名称: 	hl_hal_uart2_config
* 功能描述: 	串口2配置
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
    __HAL_RCC_USART2_CLK_ENABLE();
  
    s_huart2.Instance          = USART2;
    s_huart2.Init.BaudRate     = baudrate;
    s_huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    s_huart2.Init.StopBits     = UART_STOPBITS_1;
    s_huart2.Init.Parity       = UART_PARITY_NONE;
    s_huart2.Init.Mode         = UART_MODE_TX_RX;
    s_huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    s_huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    
    if (HAL_UART_Init(&s_huart2) != HAL_OK) {
        // Error_Handler();
        s_uart2_is_active = false;
        return false;
    }  
    
    /* 串口中断配置 */
    __HAL_UART_ENABLE_IT(&s_huart2, UART_IT_RXNE);              // 开启接收中断
    s_uart2_is_active = true;
	return true;
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
    __HAL_RCC_USART3_CLK_ENABLE();
    
    s_huart3.Instance          = USART3;
    s_huart3.Init.BaudRate     = baudrate;
    s_huart3.Init.WordLength   = UART_WORDLENGTH_8B;
    s_huart3.Init.StopBits     = UART_STOPBITS_1;
    s_huart3.Init.Parity       = UART_PARITY_NONE;
    s_huart3.Init.Mode         = UART_MODE_TX_RX;
    s_huart3.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    s_huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    
    if (HAL_UART_Init(&s_huart3) != HAL_OK) {
        // Error_Handler();
        s_uart3_is_active = false;
        return false;
    }  
    
    /* 串口中断配置 */
    __HAL_UART_ENABLE_IT(&s_huart3, UART_IT_RXNE);              // 开启接收中断

	s_uart3_is_active = true;
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
bool hl_hal_uart_receive_reg(UART_PORT_E USARTx, uart_rcv_cb_t rcv_cb_handle)
{
	switch (USARTx) {
		case UART_PORT1:
			uart1_rcv_cb = rcv_cb_handle;
			break;
			
		case UART_PORT2:
			uart2_rcv_cb = rcv_cb_handle;
			break;
			
		case UART_PORT3:
			uart3_rcv_cb = rcv_cb_handle;
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
bool hl_hal_uart_init(UART_PORT_E USARTx, uint32_t baudrate)
{
	switch (USARTx) {
		case UART_PORT1:
			return hl_hal_uart1_config(baudrate);
			break;

		case UART_PORT2:
			return hl_hal_uart2_config(baudrate);
			break;

		case UART_PORT3:
			return hl_hal_uart3_config(baudrate);
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
bool hl_hal_uart_deinit(UART_PORT_E USARTx)
{
	switch (USARTx) {
		case UART_PORT1:
			s_uart1_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart1, UART_IT_RXNE);              // 开启接收中断
			HAL_UART_DeInit(&s_huart1);
			HAL_NVIC_DisableIRQ(USART1_IRQn);
			__HAL_RCC_USART1_CLK_DISABLE();
			break;

		case UART_PORT2:
			s_uart2_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart2, UART_IT_RXNE);              // 开启接收中断
			HAL_UART_DeInit(&s_huart2);
			HAL_NVIC_DisableIRQ(USART2_IRQn);
			__HAL_RCC_USART2_CLK_DISABLE();
			break;

		case UART_PORT3:
			s_uart3_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart3, UART_IT_RXNE);              // 开启接收中断
			HAL_UART_DeInit(&s_huart3);
			HAL_NVIC_DisableIRQ(USART3_IRQn);
			__HAL_RCC_USART3_CLK_DISABLE();
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
bool hl_hal_uart_send(UART_PORT_E USARTx, uint8_t *p_data, uint16_t size)
{
	if (p_data == NULL) {
		return false;
	}

  	switch (USARTx) {
		case UART_PORT1:
			if (s_uart1_is_active == true) {
				while (size--) {
	            	USART1->TDR = *p_data; 
					p_data++;
	            	while ((USART1->ISR & UART_FLAG_TXE)==0);
				}
			}
			break;
	
		case UART_PORT2:
			if (s_uart2_is_active == true) {
				while (size--) {
	            	USART2->TDR = *p_data;
					p_data++;
	            	while ((USART2->ISR & UART_FLAG_TXE)==0);
				}
			}
			break;
			
		case UART_PORT3:
			if (s_uart3_is_active == true) {
				while (size--) {
	            	USART3->TDR = *p_data;
					p_data++;
	            	while ((USART3->ISR & UART_FLAG_TXE)==0);
				}
			}
			break;

		default:
			return false;
			break;
	}
    
    return true;
}

