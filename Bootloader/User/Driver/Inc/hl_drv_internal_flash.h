/**
 * @file hl_drv_internal_flash.h
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

#ifndef __HL_DRV_INTERNAL_FLASH_H__
#define __HL_DRV_INTERNAL_FLASH_H__

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

/* Includes ------------------------------------------------------------------*/
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define FLASH_PAGE_SIZE        ((uint16_t)0x800)        //FLASH的页大小为2048个字节
#define FLASH_WRITE_START_ADDR ((uint32_t)0x08004000)   //FLASH写的初始地址(APP程序的起始地址)
#define FLASH_WRITE_END_ADDR   ((uint32_t)0x0803FFFF)   //FLASH写的结束地址

#define INTERNAL_FLASH_ERR      1
#define INTERNAL_FLASH_OK       0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

// 将flash中的数据读取出来 
int hl_drv_internal_flash_read(uint32_t addr, uint32_t *p_buff, uint32_t size);

// 将数据写入flash中
int hl_drv_internal_flash_write(uint32_t addr, uint32_t *p_buff, uint32_t size);

// 擦除指定页数据
int hl_drv_internal_flash_erase(uint32_t addr);

#endif
/*
 * EOF
 */