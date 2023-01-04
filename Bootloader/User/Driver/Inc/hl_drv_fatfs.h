/**
 * @file hl_drv_fatfs.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-12-20
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
 * <tr><td>2022-12-20     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _HL_DRV_FATFS_H_
#define _HL_DRV_FATFS_H_

/* Includes ------------------------------------------------------------------*/

#include "ff.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

/* 文件写入函数 */
#define FATFS_DRV_WRITE_FUN     0
/* 格式化函数 */
#define FATFS_DRV_MKFS_FUN      0

#define FATFS_FUN_RET_ERR       -1
#define FATFS_FUN_RET_OK        0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief 挂载文件系统
 * 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化,
 * 初始化函数调用流程如下 
 * f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init() 
 * @return int 
 * @date 2022-12-24
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-24      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_init();

/**
 * @brief 卸载文件系统
 * @return int 
 * @date 2022-12-24
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-24      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_deinit();

/**
 * @brief 打开或创建文件
 * @param [in] fnew 
 * @param [in] filename 
 * @param [in] op_flag 
 * @return int 
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_open(FIL* fnew, char* filename, uint8_t op_flag);

/**
 * @brief 关闭文件
 * @param [in] fnew 
 * @return int 
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_close(FIL* fnew);

/**
 * @brief 读取文件的数据
 * @param [in] fnew 文件对象指针
 * @param [in] read_buffer 读缓冲区
 * @param [in] data_size 读写数量
 * @return int 成功返回读取到的字节数, 失败返回-1
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_read(FIL* fnew, uint8_t* read_buffer, uint32_t data_size);

#if (FATFS_DRV_WRITE_FUN)
/**
 * @brief 向文件的写入数据
 * @param [in] fnew 文件对象指针
 * @param [in] write_buffer 写数据缓冲区
 * @param [in] data_size 读写数量
 * @return int 成功返回写入成功的字节数, 失败返回-1
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_write(FIL* fnew, uint8_t* read_buffer, uint32_t data_size);
#endif

/**
 * @brief 删除一个文件
 * @param [in] filename 文件名
 * @return int 
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_del_file(char* filename);

/**
 * @brief 获取一个文件的大小
 * @param [in] filename 
 * @return int 成功返回文件大小， 失败返回-1
 * @date 2022-12-26
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-12-26      <td>yijiujun     <td>新建
 * </table>
 */
int hl_drv_fatfs_get_file_size(char* filename);
#endif
/*
 * EOF
 */