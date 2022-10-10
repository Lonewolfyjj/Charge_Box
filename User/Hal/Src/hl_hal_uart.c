/************************************************************************
* ��Ȩ���� (C)2001, �������һԴ�Ƽ����޹�˾��
* 
* �ļ����ƣ�
* �ļ���ʶ��
* ����ժҪ��
* ����˵����
* ��ǰ�汾��			V1.0.0.0
* ��     �ߣ�		lzh 
* ������ڣ�			2021/06/16
* 
* �޸ļ�¼1�� 
*    �޸����ڣ� 2019/07/06
*    �� �� �ţ� V1.0.0.0
*    �� �� �ˣ� XXXX
*    �޸����ݣ� 
* �޸ļ�¼2����
************************************************************************/
#include "hl_hal_uart.h"

/**************************************************************************
 *                                ��                                       *
 **************************************************************************/

//

/**************************************************************************
 *                             ��������                                       *
 **************************************************************************/

//

/**************************************************************************
 *                               ����                                       *
 **************************************************************************/

//

/**************************************************************************
 *                            �ֲ�����ԭ��                                      *
 **************************************************************************/

//


/**************************************************************************
 *                             ȫ�ֱ���                                       *
 **************************************************************************/
/* ���Դ��ھ��       */
static UART_HandleTypeDef s_huart1 = {0};
static UART_HandleTypeDef s_huart2 = {0};
static UART_HandleTypeDef s_huart3 = {0};

/* ���ڻص�����ָ�� */
static uart_rcv_cb_t uart1_rcv_cb = NULL;
static uart_rcv_cb_t uart2_rcv_cb = NULL;
static uart_rcv_cb_t uart3_rcv_cb = NULL;

/* ���ڹ�����־ */
static bool s_uart1_is_active = false;
static bool s_uart2_is_active = false;
static bool s_uart3_is_active = false;

/**************************************************************************
 *                              �жϺ���ʵ��                              *
 **************************************************************************/ 
void USART1_IRQHandler(void)
{
    uint8_t receive_data = 0;    // �����������    
    
    /* �����ж� */
    if((__HAL_UART_GET_FLAG(&s_huart1, UART_FLAG_RXNE) != RESET)) {
         
        /* �������ݽ��� */
        receive_data = (uint8_t) READ_REG(s_huart1.Instance->RDR);

        /* ���ڴ���ص���������Ҫ�����ǽ��������ݻ��浽fifo�� */
        if (uart1_rcv_cb != NULL) {
            uart1_rcv_cb(receive_data); 
        }
    }
    
    /* �������ж� */
    if((__HAL_UART_GET_FLAG(&s_huart1, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart1.Instance->RDR);
    }
    
    /* ����жϱ�־ */
    HAL_UART_IRQHandler(&s_huart1);
}

void USART2_IRQHandler(void)
{
    uint8_t receive_data = 0;    // �����������    
    
    /* �����ж� */
    if((__HAL_UART_GET_FLAG(&s_huart2, UART_FLAG_RXNE) != RESET)) {
         
        /* �������ݽ��� */
        receive_data = (uint8_t) READ_REG(s_huart2.Instance->RDR);

        /* ���ڴ���ص���������Ҫ�����ǽ��������ݻ��浽fifo�� */
        if (uart2_rcv_cb != NULL) {
            uart2_rcv_cb(receive_data); 
        }
    }
    
    /* �������ж� */
    if((__HAL_UART_GET_FLAG(&s_huart2, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart2.Instance->RDR);
    }
    
    /* ����жϱ�־ */
    HAL_UART_IRQHandler(&s_huart2);
}

void USART3_IRQHandler(void)
{
    uint8_t receive_data = 0;    // �����������    
    
    /* �����ж� */
    if((__HAL_UART_GET_FLAG(&s_huart3, UART_FLAG_RXNE) != RESET)) {
         
        /* �������ݽ��� */
        receive_data = (uint8_t) READ_REG(s_huart3.Instance->RDR);

        /* ���ڴ���ص���������Ҫ�����ǽ��������ݻ��浽fifo�� */
        if (uart3_rcv_cb != NULL) {
            uart3_rcv_cb(receive_data); 
        }
    }
    
    /* �������ж� */
    if((__HAL_UART_GET_FLAG(&s_huart3, UART_FLAG_ORE) != RESET)) {
        receive_data = (uint8_t) READ_REG(s_huart3.Instance->RDR);
    }
    
    /* ����жϱ�־ */
    HAL_UART_IRQHandler(&s_huart3);
}

/**************************************************************************
 *                              �ֲ�����ʵ��                                    *
 **************************************************************************/ 
/**************************************************************************
* ��������: 	hl_hal_uart1_config
* ��������: 	����1����
* �������: BaudRate:���ڲ�����
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����          �汾��          �޸���	      �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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
    
    /* �����ж����� */
    __HAL_UART_ENABLE_IT(&s_huart1, UART_IT_RXNE);              // ���������ж�
    s_uart1_is_active = true;
	return true;
}

/**************************************************************************
* ��������: 	hl_hal_uart2_config
* ��������: 	����2����
* �������: BaudRate:���ڲ�����
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����          �汾��          �޸���	      �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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
    
    /* �����ж����� */
    __HAL_UART_ENABLE_IT(&s_huart2, UART_IT_RXNE);              // ���������ж�
    s_uart2_is_active = true;
	return true;
}

/**************************************************************************
* ��������: 	hl_hal_uart3_config
* ��������: 	����3����
* �������: BaudRate:���ڲ�����
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����          �汾��          �޸���	      �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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
    
    /* �����ж����� */
    __HAL_UART_ENABLE_IT(&s_huart3, UART_IT_RXNE);              // ���������ж�

	s_uart3_is_active = true;
	return true;
}

/**************************************************************************
 *                              ȫ�ֺ���ʵ��                                    *
 **************************************************************************/
/**************************************************************************
* ��������: 	hl_hal_uart_receive_reg
* ��������: 	���ڽ��ջص�����ע��
* �������: USARTx: ����ö�ٱ��				art_rcv_cb_t���ص�����
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����			�汾��		   �޸���		  �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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
* ��������: 	hl_hal_uart_config
* ��������: 	��������
* �������: USARTx: ����ö�ٱ��	  BaudRate:���ڲ�����
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����			�汾��		   �޸���		  �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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
* ��������: 	hl_hal_uart_deint
* ��������: 	����ȥʼ��
* �������: USARTx: ����ö�ٱ�� 
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����          �汾��          �޸���	      �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
**************************************************************************/
bool hl_hal_uart_deinit(UART_PORT_E USARTx)
{
	switch (USARTx) {
		case UART_PORT1:
			s_uart1_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart1, UART_IT_RXNE);              // ���������ж�
			HAL_UART_DeInit(&s_huart1);
			HAL_NVIC_DisableIRQ(USART1_IRQn);
			__HAL_RCC_USART1_CLK_DISABLE();
			break;

		case UART_PORT2:
			s_uart2_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart2, UART_IT_RXNE);              // ���������ж�
			HAL_UART_DeInit(&s_huart2);
			HAL_NVIC_DisableIRQ(USART2_IRQn);
			__HAL_RCC_USART2_CLK_DISABLE();
			break;

		case UART_PORT3:
			s_uart3_is_active = false;
			__HAL_UART_DISABLE_IT(&s_huart3, UART_IT_RXNE);              // ���������ж�
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
* ��������: 	hl_hal_uart_send
* ��������: 	���ڷ�������
* �������: USARTx: ����ö�ٱ��				p_data������ָ��  		size�����ݴ�С
* �������: ��
* �� �� ֵ: true���ɹ�				false��ʧ��
* ����˵��: 
* �޸�����          �汾��          �޸���	      �޸�����
* -----------------------------------------------
* 2021/06/16	V1.0.0.0	 ¬�ú�	      ����
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

