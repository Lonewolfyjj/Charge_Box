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
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


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
uint8_t hl_hal_soft_spi_send_recv(hl_hal_soft_spi_info *spi_info, uint8_t wdata)
{
    uint8_t count, reg_val = 0;
    for (count = 0; count < 8; count++) {
        SPI_SCK(spi_info->spi_sck_pin_num, spi_info->gpiox_base) = 0;
        if (wdata & 0x80) {     //在下降沿处，主机发送一位数据给从机
            SPI_MOSI(spi_info->spi_mosi_pin_num, spi_info->gpiox_base) = 1;
        } else {
            SPI_MOSI(spi_info->spi_mosi_pin_num, spi_info->gpiox_base) = 0;
        }
        wdata <<= 1;
        reg_val <<= 1; 
        SPI_SCK(spi_info->spi_sck_pin_num, spi_info->gpiox_base) = 1;
		if(SPI_MISO(spi_info->spi_miso_pin_num, spi_info->gpiox_base) == 1) {          //在上升沿处，主机接收一位数据
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
    SPI_SCK(spi_sck_pin_num, gpiox_base) = 0;

    for (count = 0; count < 8; count++) { 
        SPI_MOSI(spi_mosi_pin_num, gpiox_base) = (wdata >> (7 - count)) & 0x01;     //发送
        delay_us(1);
        SPI_SCK(spi_sck_pin_num, gpiox_base) = 1;
        reg_val |= SPI_MOSI(spi_miso_pin_num, gpiox_base) << (7 - count);     //接收
        delay_us(1);
        SPI_SCK(spi_sck_pin_num, gpiox_base) = 0;        
    }
    return reg_val;
}
#endif

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
void hl_hal_soft_spi_init(hl_hal_soft_spi_info *spi_info)
{
    GPIO_InitType gpio_init_struct;
    RCC_EnableAPB2PeriphClk(spi_info->rcc_periph_gpiox, ENABLE);
    GPIO_InitStruct(&gpio_init_struct);

    gpio_init_struct.Pin = spi_info->spi_cs_pin | spi_info->spi_mosi_pin | spi_info->spi_sck_pin;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Pull = GPIO_Pull_Up;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
    GPIO_InitPeripheral(spi_info->gpiox, &gpio_init_struct);

    gpio_init_struct.Pin = spi_info->spi_miso_pin;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
    gpio_init_struct.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitPeripheral(spi_info->gpiox, &gpio_init_struct);

}

void hl_hal_soft_spi_deinit(hl_hal_soft_spi_info *spi_info)
{
    GPIO_InitType gpio_init_struct;
    RCC_EnableAPB2PeriphClk(spi_info->rcc_periph_gpiox, DISABLE);
    GPIO_InitStruct(&gpio_init_struct);

    gpio_init_struct.Pin = spi_info->spi_cs_pin | spi_info->spi_mosi_pin | spi_info->spi_sck_pin | spi_info->spi_miso_pin;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Analog;      //模拟模式
    gpio_init_struct.GPIO_Pull = GPIO_No_Pull;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_Low;
    GPIO_InitPeripheral(spi_info->gpiox, &gpio_init_struct);

}

/*
 * EOF
 */