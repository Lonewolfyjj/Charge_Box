/**
 * @file hl_util_config.h
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-28
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
 * <tr><td>2022-10-28     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HL_UTIL_CONFIG_H__
#define __HL_UTIL_CONFIG_H__

/* Includes ------------------------------------------------------------------*/

#include "rtthread.h"
#include "stdbool.h"
#include "stdint.h"

/* typedef -------------------------------------------------------------------*/

/**
 * 
 * @brief 写入内容到存储器件的函数，成功返回HL_UTIL_CONFIG_FUNC_OK，失败返回HL_UTIL_CONFIG_FUNC_ERR
 * @date 2022-10-28
 * @author lilin (lin.li@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-28      <td>lilin     <td>新建
 * </table>
 */
typedef int (*hl_util_config_write_in)(char* data, int len);

/**
 * 
 * @brief 从存储器件读出内容的函数，成功返回HL_UTIL_CONFIG_FUNC_OK，失败返回HL_UTIL_CONFIG_FUNC_ERR
 * @date 2022-10-28
 * @author lilin (lin.li@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-28      <td>lilin     <td>新建
 * </table>
 */
typedef int (*hl_util_config_read_out)(char* data, int len);

typedef struct _hl_util_config_st
{
    bool lowpower_flag;
    bool boot_jump_flag;
} hl_util_config_st, *hl_util_config_st_p;

/* define --------------------------------------------------------------------*/

#define HL_UTIL_CONFIG_FUNC_ERR 1
#define HL_UTIL_CONFIG_FUNC_OK 0

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_util_config_init(hl_util_config_write_in write, hl_util_config_read_out read);

int hl_util_config_deinit(void);

int hl_util_config_get(hl_util_config_st_p* config);

int hl_util_config_save(void);

#endif
/*
 * EOF
 */