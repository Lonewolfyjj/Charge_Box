/**
 * @file hl_mod_upgrade.c
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

#include "hl_mod_upgrade.h"
#include "hl_drv_internal_flash.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define UPGRADE_BOX_APP_ADDRESS 0x08004000  //BOX的APP运行地址
#define BUFFER_MAX_SIZE         2048        //缓冲区最大值
#define debug_printf printf

//#define UPGRADE_FILE_NAME "12345678.txt"
#define UPGRADE_FILE_NAME "A6902Box.bin"

/* variables -----------------------------------------------------------------*/

static uint8_t f_data_buffer[BUFFER_MAX_SIZE];  //读写缓冲区
static FIL     fnew;  //文件操作对象

static bool _upgrade_init_flag = false;

/* Private function(only *.c)  -----------------------------------------------*/

static void _upgrade_app_file_write(uint32_t file_size)
{
    uint32_t fnum;  //文件成功读写数量
    uint32_t count_num = 0;
    while (1) {
        fnum = hl_drv_fatfs_read(&fnew, f_data_buffer, sizeof(f_data_buffer));

        if (fnum < BUFFER_MAX_SIZE) {   //检测是否为4的倍数，不是的话补齐为4的倍数
            if (fnum % 4 == 1) {
                f_data_buffer[fnum] = 0xff;
                f_data_buffer[fnum + 1] = 0xff;
                f_data_buffer[fnum + 2] = 0xff;
                fnum += 3;
                file_size += 3;
            } else if (fnum % 4 == 2) {
                f_data_buffer[fnum] = 0xff;
                f_data_buffer[fnum + 1] = 0xff;
                fnum += 2;
                file_size += 2;
            } else if (fnum % 4 == 3) {
                f_data_buffer[fnum] = 0xff;
                fnum += 1;
                file_size += 1;
            } 
        }
        // 擦除指定页数据, 一页2048字节
        hl_drv_internal_flash_erase(UPGRADE_BOX_APP_ADDRESS + count_num);

        hl_drv_internal_flash_write(UPGRADE_BOX_APP_ADDRESS + count_num, (uint32_t *)f_data_buffer, fnum);
        
        count_num += fnum;  //累计已经写入的数据量
        //debug_printf("--count_num: %d\r\n", count_num);
        if (count_num == file_size) {
            break;
        }
    }
}

/* Exported functions --------------------------------------------------------*/

int hl_mod_upgrade_init()
{
    int ret;
    if (_upgrade_init_flag == true) {
        debug_printf("upgrade mod already inited!\r\n");
        return UPGRADE_FUN_RET_OK;
    }
    ret = hl_drv_fatfs_init();
    if (ret == FATFS_FUN_RET_ERR) {
        return UPGRADE_FUN_RET_ERR;
    }
    _upgrade_init_flag = true;
    return UPGRADE_FUN_RET_OK;
}

int hl_mod_upgrade_deinit()
{
    int ret;
    if (_upgrade_init_flag == false) {
        debug_printf("upgrade mod no init!\r\n");
        return UPGRADE_FUN_RET_OK;
    }

    ret = hl_drv_fatfs_deinit();
    if (ret == FATFS_FUN_RET_ERR) {
        return UPGRADE_FUN_RET_ERR;
    }
    return UPGRADE_FUN_RET_OK;
}

int hl_mod_upgrade_start()
{
    int file_size = 0;

    if (_upgrade_init_flag == false) {
        debug_printf("upgrade mod no init!\r\n");
        return UPGRADE_FUN_RET_ERR;
    }

    file_size = hl_drv_fatfs_get_file_size(UPGRADE_FILE_NAME);
    
    debug_printf("--文件大小为：%d\r\n", file_size);
    if (file_size <= 0) {
        return UPGRADE_FUN_RET_ERR; //file no exist or file name invaild
    }

    hl_drv_fatfs_open(&fnew, UPGRADE_FILE_NAME, FA_READ);
    
    _upgrade_app_file_write(file_size);

    hl_drv_fatfs_close(&fnew);

    hl_drv_fatfs_del_file(UPGRADE_FILE_NAME);

    return UPGRADE_FUN_RET_OK;
}

/*
 * EOF
 */