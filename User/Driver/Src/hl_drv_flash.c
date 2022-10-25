/**
 * @file hl_drv_flash.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-25
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
 * <tr><td>2022-10-25     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_flash.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_WritePage		    0x02
#define W25X_SectorErase		0x20 
#define W25X_BlockErase			0xD8 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable_Reset       0x66
#define W25X_Reset              0x99

/* variables -----------------------------------------------------------------*/

static hl_hal_soft_spi_info spi_info = {
	.spi_mosi_pin_num 	= 15,
	.spi_miso_pin_num 	= 14,
	.spi_sck_pin_num  	= 13,
	.spi_cs_pin_num   	= 12,
	.spi_mosi_pin     	= GPIO_PIN_15,
	.spi_miso_pin     	= GPIO_PIN_14,
	.spi_sck_pin      	= GPIO_PIN_13,
	.spi_cs_pin			= GPIO_PIN_12,
	.gpiox_base			= GPIOB_BASE,
	.rcc_periph_gpiox 	= RCC_APB2_PERIPH_GPIOB,
	.gpiox				= GPIOB
};

static bool flash_init_flag = false;

/* Private function(only *.c)  -----------------------------------------------*/

static void hl_drv_w25q32_write_enable(void)
{
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

	hl_hal_soft_spi_send_recv(&spi_info, W25X_WriteEnable);				//写使能指令
	
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
}

static void hl_drv_w25q32_write_disable(void)
{
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

	hl_hal_soft_spi_send_recv(&spi_info, W25X_WriteDisable);    		//写禁止指令
	
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
}

static uint8_t hl_drv_w25q32_read_status_reg01(void)
{
	uint8_t sr1_data=0;
	HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

	hl_hal_soft_spi_send_recv(&spi_info, W25X_ReadStatusReg);           //读SR1寄存器指令
	sr1_data = hl_hal_soft_spi_send_recv(&spi_info, 0xFF); 				//传一个无意义的数据
	
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	return sr1_data;
}

/**
 * @brief 擦除flash中的对应扇区地址中的数据
 * @param [in] addr 
 * @date 2022-10-25
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-25      <td>yijiujun     <td>新建
 * </table>
 */
static void hl_drv_w25q32_erase_sector(uint32_t addr)
{
    addr *= 4096;
	//addr &= 0xFFF000;//得到这个地址对应的扇区首地址

	hl_drv_w25q32_write_enable(); 
	while ((hl_drv_w25q32_read_status_reg01() & 0x02) != 0x02);         //等待完全写使能
	
	HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	hl_hal_soft_spi_send_recv(&spi_info, W25X_SectorErase);         	//发送扇区擦除指令 
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);  	    //发送24bit地址    
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);   
    hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);  
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base); 
	
	hl_drv_w25q32_write_disable();
	while ((hl_drv_w25q32_read_status_reg01() & 0x01) == 0x01);          //等待擦除完成
}

/**
 * @brief 将数据写入对应的地址，最多写一页（256个字节）
 * @param [in] addr 
 * @param [in] w_data 
 * @param [in] len 
 * @return int 
 * @date 2022-10-25
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-25      <td>yijiujun     <td>新建
 * </table>
 */
static int hl_drv_w25q32_write_page(uint32_t addr, uint8_t *w_data, int32_t len)
{
    if (w_data == NULL) {
        return -1;
    }
	//hl_drv_w25q32_erase_sector(addr);
	hl_drv_w25q32_write_enable(); 
	while ((hl_drv_w25q32_read_status_reg01() & 0x02) != 0x02);   		//等待完全写使能

	HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	hl_hal_soft_spi_send_recv(&spi_info, W25X_WritePage);               //发送写页指令   0x02
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);  	    //发送24bit地址    
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);   
    hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);  
    while (len--) {
        hl_hal_soft_spi_send_recv(&spi_info, *w_data);
        w_data++;
	}
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	
	hl_drv_w25q32_write_disable();
	while ((hl_drv_w25q32_read_status_reg01() & 0x01) == 0x01); 		// 等待写入结束
    return 0;
}

/* Exported functions --------------------------------------------------------*/

uint16_t hl_drv_w25q32_read_maker_dev_id(void)
{
	uint16_t reg_val = 0;
	HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

	hl_hal_soft_spi_send_recv(&spi_info, W25X_ManufactDeviceID);    	//发送读取ID命令 0x90
	hl_hal_soft_spi_send_recv(&spi_info, 0x00);
	hl_hal_soft_spi_send_recv(&spi_info, 0x00);
	hl_hal_soft_spi_send_recv(&spi_info, 0x00);

	reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff) << 8;
	reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff);

	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	return reg_val;					
}

void hl_drv_w25q32_soft_reset_device()
{
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	hl_hal_soft_spi_send_recv(&spi_info, W25X_Enable_Reset);    		//启用复位
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

	rt_thread_mdelay(1);
	
	HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	hl_hal_soft_spi_send_recv(&spi_info, W25X_Reset);    				//复位
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
	
	rt_thread_mdelay(1);
}

/**
 * @brief 可以连续读数据
 * @param [in] addr 
 * @param [in] r_data 
 * @param [in] len 
 * @return int 
 * @date 2022-10-25
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-25      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_flash_read(uint32_t addr, uint8_t *r_data, uint32_t len)
{
    if (r_data == NULL || flash_init_flag == false) {
		debug_printf("[error] hl_drv_flash_read\n");
        return FLASH_RET_ERR;
    }
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base); 	//片选线拉低		
	hl_hal_soft_spi_send_recv(&spi_info, W25X_ReadData);    			//发读数据指令	
	/*发送24bit地址*/ 
	hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);   		//高八位
	hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);    		//中八位
	hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);             		//低八位	
	while (len--) {
        *r_data = hl_hal_soft_spi_send_recv(&spi_info, 0xff);
        r_data++;
	}
	HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    return FLASH_RET_OK;
}

/**
 * @brief 无校验写入，可以跨页写入数据
 * @param [in] addr 
 * @param [in] w_data 
 * @param [in] len 
 * @return int 
 * @date 2022-10-25
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-25      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_flash_write(uint32_t addr, uint8_t *w_data, uint32_t len)
{
	uint32_t pager;
	if (w_data == NULL || flash_init_flag == false) {
		debug_printf("[error] hl_drv_flash_write\n");
        return FLASH_RET_ERR;
    }
	pager = 256 - addr%256;    	//单页剩余的字节数
	if (len <= pager) {
		pager = len;			//不大于256字节
	}

	hl_drv_w25q32_erase_sector(addr);

	while (1) {
		hl_drv_w25q32_write_page(addr, w_data, pager);
		if(len == pager) {
			break;				//全部写入
		}
		w_data 	+= pager;
		addr 	+= pager;
		len 	-= pager;      	//减去已经写入的字节数
		if(len > 256) {
			pager = 256;        //一次写入256字节
		} else {
			pager = len;        //已经不足256字节
		}
	}
	return FLASH_RET_OK;
}

int hl_drv_flash_init(void)
{
	if (flash_init_flag == true) {
		debug_printf("[error] flash already init\n");
		return FLASH_RET_ERR;
	}
    /* 软件SPI引脚初始化 */
    hl_hal_soft_spi_init(&spi_info);

	flash_init_flag = true;
	return FLASH_RET_OK;
}

int hl_drv_flash_deinit(void)
{
	if (flash_init_flag == false) {
		return FLASH_RET_ERR;
	}

	flash_init_flag = false;
	return FLASH_RET_OK;
}
/*
 * EOF
 */





