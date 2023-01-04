/**
 * @file hl_drv_internal_flash.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-24
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
 * <tr><td>2022-12-24     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_internal_flash.h"
#include "n32l40x_flash.h"
#include <stdio.h>

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_drv_internal_flash_read(uint32_t addr, uint32_t *p_buff, uint32_t size)
{
    uint16_t count;
    uint32_t *data_addr = (uint32_t *)(addr);   //得到数据起始地址, 注意是4字节的数据地址

    if (((addr & 0x000000003) != 0) || ((size % 4) != 0)) {		// 地址错误
        printf("[addr error] hl_drv_internal_flash_read.\r\n");
		return INTERNAL_FLASH_ERR;
	}

    /* Read */
    for (count = 0; count < size; count += 4) {     //读四个字节数据，存放四个字节数据

        *p_buff = *data_addr;
        data_addr++;            //偏移一位，就是偏移四个字节
        p_buff++;
    }

    return INTERNAL_FLASH_OK;
}

// 将数据写入flash中
int hl_drv_internal_flash_write(uint32_t addr, uint32_t *p_buff, uint32_t size)
{
    uint16_t count; 

    if (((addr & 0x000000003) != 0) || ((size%4) != 0)) {		// 地址错误
        printf("[addr error] hl_drv_internal_flash_write.\r\n");
		return INTERNAL_FLASH_ERR;
	}

    FLASH_Unlock();

    /* Write */
    for (count = 0; count < size; count += 4) {             //取四个字节数据，写四个字节数据

        if (FLASH_COMPL != FLASH_ProgramWord(addr, *p_buff)) {
			FLASH_Lock();
			return INTERNAL_FLASH_ERR;
		}
		
		addr += 4;      //偏移4位，就是偏移四个字节
		p_buff++;
    }
    

    FLASH_Lock();
    return INTERNAL_FLASH_OK;
}

// 擦除指定页数据
int hl_drv_internal_flash_erase(uint32_t addr)
{
    uint32_t erase_addr = (addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;   //得到页的首地址

    FLASH_Unlock();

    /* Erase */
    if (FLASH_COMPL != FLASH_EraseOnePage(erase_addr)) {

        while(1) {
            printf("Flash EraseOnePage Error. Please Deal With This Error Promptly\r\n");
        }
    }

    FLASH_Lock();
    return INTERNAL_FLASH_OK;
}


/*
 * EOF
 */