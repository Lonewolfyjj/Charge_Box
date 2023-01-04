/**
 * @file hl_mod_upgrade.h
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2023-01-03
 * 
 * ██╗  ██╗ ██████╗ ██╗     ██╗  ██╗   ██╗██╗      █████╗ ███╗   ██╗██████╗ 
 * ██║  ██║██╔═══██╗██║     ██║  ╚██╗ ██╔╝██║     ██╔══██╗████╗  ██║██╔══██╗
 * ███████║██║   ██║██║     ██║   ╚████╔╝ ██║     ███████║██╔██╗ ██║██║  ██║
 * ██╔══██║██║   ██║██║     ██║    ╚██╔╝  ██║     ██╔══██║██║╚██╗██║██║  ██║
 * ██║  ██║╚██████╔╝███████╗███████╗██║   ███████╗██║  ██║██║ ╚████║██████╔╝
 * ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚══════╝╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝
 * @copyright Copyright (c) 2023 hollyland
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date           <th>Version  <th>Author         <th>Description
 * <tr><td>2023-01-03     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _HL_MOD_UPGRADE_H_
#define _HL_MOD_UPGRADE_H_

/* Includes ------------------------------------------------------------------*/

#include "hl_drv_flash.h"

/* typedef -------------------------------------------------------------------*/

typedef enum _hl_mod_upgrade_op_cmd_e {

    //进行一次升级文件检测, type of parameter is <RT_NULL> pointer.
    HL_MOD_UPGRADE_FILE_CHECK
}hl_mod_upgrade_op_cmd_e;


typedef enum _hl_mod_upgrade_msg_e {

    HL_MOD_UPGRADE_FILE_EXIST_MSG,      //升级文件存在

} hl_mod_upgrade_msg_e;

/* define --------------------------------------------------------------------*/

#define UPGRADE_FILE_SYSTEM_ADDR        0x1003      //文件系统信息起始地址（FLASH偏移地址 + 3）
#define UPGRADE_FILE_DIR_ENTRY_ADDR     0x4000      //文件目录项起始地址（FLASH偏移地址 + 3KB）

#define HL_MOD_UPGRADE_FUNC_OK          0
#define HL_MOD_UPGRADE_FUNC_ERR         1

#define UPGRADE_FILE_NAME               "A6902BOX"

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_mod_upgrade_init(void *msg_hd);

int hl_mod_upgrade_deinit(void);

int hl_mod_upgrade_start(void);

int hl_mod_upgrade_stop(void);

int hl_mod_upgrade_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size);

#endif
/*
 * EOF
 */