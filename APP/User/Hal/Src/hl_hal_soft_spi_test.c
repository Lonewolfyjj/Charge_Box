/**
 * @file hl_hal_soft_spi_test.c
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

uint16_t hl_hal_soft_spi_read_maker_dev_id(hl_hal_soft_spi_info *spi_info)
{
    uint16_t reg_val = 0;

	HL_HAL_SPI_CS_LOW(spi_info->spi_cs_pin_num, spi_info->gpiox_base);
	hl_hal_soft_spi_send_recv(spi_info, 0x90);    //发送读取ID命令 0x90
	hl_hal_soft_spi_send_recv(spi_info, 0x00);
	hl_hal_soft_spi_send_recv(spi_info, 0x00);
	hl_hal_soft_spi_send_recv(spi_info, 0x00);

	reg_val |= hl_hal_soft_spi_send_recv(spi_info, 0xff) << 8;
	reg_val |= hl_hal_soft_spi_send_recv(spi_info, 0xff);

	HL_HAL_SPI_CS_LOW(spi_info->spi_cs_pin_num, spi_info->gpiox_base);
	return reg_val;
}

void hl_hal_soft_spi_test(void)
{
    uint16_t data = 0;
    hl_hal_soft_spi_info spi_info = {
        15,
        14,
        13,
        12,
        GPIO_PIN_15,
        GPIO_PIN_14,
        GPIO_PIN_13,
        GPIO_PIN_12,
        GPIOB_BASE,
        RCC_APB2_PERIPH_GPIOB,
        GPIOB
    };
    /* 软件SPI引脚初始化 */
    hl_hal_soft_spi_init(&spi_info);

    /* 读取W25Q32的制造商和设备ID */
    data = hl_hal_soft_spi_read_maker_dev_id(&spi_info);
    rt_kprintf("w25q32 manual and dev ID : %x\n", data);

}

MSH_CMD_EXPORT(hl_hal_soft_spi_test, soft SPI test);
/*
 * EOF
 */