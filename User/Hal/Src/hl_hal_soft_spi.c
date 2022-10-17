/**
 * @file hl_hal_soft_spi.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-17
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
 * <tr><td>2022-10-17     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "hl_hal_soft_spi.h"
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
#define SPI_MOSI_PIN            GPIO_PIN_15
#define SPI_MISO_PIN            GPIO_PIN_14
#define SPI_SCK_PIN             GPIO_PIN_13
#define SPI_CS_PIN              GPIO_PIN_12
#define SPI_GPIOX               GPIOB
#define SPI_RCC_PERIPH_GPIOX    RCC_APB2_PERIPH_GPIOB
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/




/**
 * @brief SPI对应的引脚初始化
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
void hl_hal_soft_spi_init(void)
{
    GPIO_InitType gpio_init_struct;
    RCC_EnableAPB2PeriphClk(SPI_RCC_PERIPH_GPIOX, ENABLE);
    GPIO_InitStruct(&gpio_init_struct);

    gpio_init_struct.Pin = SPI_MOSI_PIN | SPI_SCK_PIN | SPI_CS_PIN;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pull = GPIO_Pull_Up;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
    GPIO_InitPeripheral(SPI_GPIOX, &gpio_init_struct);

    gpio_init_struct.Pin = SPI_MISO_PIN;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
    gpio_init_struct.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitPeripheral(SPI_GPIOX, &gpio_init_struct);
}

#if 1

/**
 * @brief SPI_MODE3 模式软件模拟数据收发
 * @param [in] wdata 要写入的一字节数据
 * @return uint8_t 成功返回 读取到的一字节数据
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
uint8_t hl_hal_soft_spi_send_recv(uint8_t wdata)
{
    uint8_t count, reg_val = 0;
    for (count = 0; count < 8; count++) {
        SPI_SCK = 0;
        if (wdata & 0x80) {     //在下降沿处，主机发送一位数据给从机
            SPI_MOSI = 1;
        } else {
            SPI_MOSI = 0;
        }
        wdata <<= 1;
        reg_val <<= 1; 
        SPI_SCK = 1;
		if(SPI_MISO) {          //在上升沿处，主机接收一位数据
            reg_val |= 0x01;
        }              
    }
    return reg_val;
}


#else
/**
 * @brief SPI_MODE0 模式软件模拟数据收发
 * @param [in] wdata 要写入的一字节数据
 * @return uint8_t 成功返回 读取到的一字节数据
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
uint8_t hl_hal_soft_spi_send_recv_data(uint8_t wdata)
{
    uint8_t count, reg_val = 0;
    SPI_SCK = 0;

    for (count = 0; count < 8; count++) { 
        SPI_MOSI = (wdata >> (7 - count)) & 0x01;     //发送
        delay_us(1);
        SPI_SCK = 1;
        reg_val |= SPI_MISO << (7 - count);     //接收
        delay_us(1);
        SPI_SCK = 0;        
    }
    return reg_val;
}

#endif
/*
 * EOF
 */