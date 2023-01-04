/**
 * @file hl_hal_hard_spi.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-29
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
 * <tr><td>2022-11-29     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_hard_spi.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void hl_hal_hard_spi_init(void) 
{

    SPI_InitType spi_type;
    GPIO_InitType gpio_init_struct;

    GPIO_InitStruct(&gpio_init_struct);

    RCC_EnableAPB2PeriphClk(
        FLASH_CS_GPIO_CLK | FLASH_SPI_MOSI_GPIO_CLK | FLASH_SPI_MISO_GPIO_CLK | FLASH_SPI_SCK_GPIO_CLK, ENABLE);

    RCC_EnableAPB2PeriphClk(FLASH_SPI_CLK|RCC_APB2_PERIPH_AFIO, ENABLE);

    gpio_init_struct.Pin        = FLASH_SPI_SCK_PIN;
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_init_struct.GPIO_Alternate = GPIO_AF0_SPI2;
    GPIO_InitPeripheral(FLASH_SPI_SCK_GPIO_PORT, &gpio_init_struct);

    /*!< Configure FLASH_CS_PIN pin: FLASH Card CS pin */
    gpio_init_struct.Pin       = FLASH_CS_PIN;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(FLASH_CS_GPIO_PORT, &gpio_init_struct);

    /*!< Configure FLASH_SPI pins: MOSI */
    gpio_init_struct.Pin        = FLASH_SPI_MOSI_PIN;
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_init_struct.GPIO_Alternate = GPIO_AF0_SPI2;
    GPIO_InitPeripheral(FLASH_SPI_MOSI_GPIO_PORT, &gpio_init_struct);

    /*!< Configure FLASH_SPI pins: MISO */
    gpio_init_struct.Pin       = FLASH_SPI_MISO_PIN;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
    GPIO_InitPeripheral(FLASH_SPI_MISO_GPIO_PORT, &gpio_init_struct);

    HARD_SPI_CS_HIGH();

    spi_type.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    spi_type.SpiMode = SPI_MODE_MASTER;
    spi_type.DataLen = SPI_DATA_SIZE_8BITS;
    spi_type.CLKPOL = SPI_CLKPOL_HIGH;
    spi_type.CLKPHA = SPI_CLKPHA_SECOND_EDGE;
    spi_type.NSS = SPI_NSS_SOFT;
    spi_type.BaudRatePres = SPI_BR_PRESCALER_4;
    spi_type.FirstBit = SPI_FB_MSB;
    spi_type.CRCPoly = 7;
    SPI_Init(FLASH_SPI, &spi_type);

    SPI_Enable(FLASH_SPI, ENABLE);

}

uint8_t hl_hal_hard_spi_send_recv(uint8_t byte)
{
    int num = 0;
    /*!< Loop while DAT register in not emplty */
    while (SPI_I2S_GetStatus(FLASH_SPI, SPI_I2S_TE_FLAG) == RESET) {
        num++;
        if (num > 1000 * 1000) {
            return HARD_SPI_FUNC_RET_ERR;
        }
    }

    /*!< Send byte through the FLASH_SPI peripheral */
    SPI_I2S_TransmitData(FLASH_SPI, byte);

    num = 0;
    /*!< Wait to receive a byte */
    while (SPI_I2S_GetStatus(FLASH_SPI, SPI_I2S_RNE_FLAG) == RESET) {
        num++;
        if (num > 1000 * 1000) {
            return HARD_SPI_FUNC_RET_ERR;
        }
    }

    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(FLASH_SPI);
}

void hl_hal_hard_spi_deinit(void)
{
    GPIO_InitType GPIO_InitStructure;

    /*!< Disable the FLASH_SPI  */
    SPI_Enable(FLASH_SPI, DISABLE);

    /*!< DeInitializes the FLASH_SPI */
    SPI_I2S_DeInit(FLASH_SPI);

    /*!< FLASH_SPI Periph clock disable */
    RCC_EnableAPB2PeriphClk(FLASH_SPI_CLK, DISABLE);

    /*!< Configure FLASH_SPI pins: SCK */
    GPIO_InitStructure.Pin       = FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Analog;         //模拟模式
    GPIO_InitPeripheral(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure FLASH_SPI pins: MISO */
    GPIO_InitStructure.Pin = FLASH_SPI_MISO_PIN;
    GPIO_InitPeripheral(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure FLASH_SPI pins: MOSI */
    GPIO_InitStructure.Pin = FLASH_SPI_MOSI_PIN;
    GPIO_InitPeripheral(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure FLASH_CS_PIN pin: FLASH Card CS pin */
    GPIO_InitStructure.Pin = FLASH_CS_PIN;
    GPIO_InitPeripheral(FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * EOF
 */