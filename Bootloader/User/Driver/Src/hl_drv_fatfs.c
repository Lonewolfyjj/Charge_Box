/**
 * @file hl_drv_fatfs.c
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
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_fatfs.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define debug_printf printf
#define FATFS_PATH "0:"  //文件系统的路径

/* variables -----------------------------------------------------------------*/

static FATFS fat_fs;  //文件系统

#if (FATFS_DRV_MKFS_FUN)
BYTE work[FF_MAX_SS];  //一定是一个全局变量
#endif

static uint8_t _drv_init_flag = 0;

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_drv_fatfs_init(FATFS* fs)
{
    FRESULT res_flash;  //文件操作结果

    if (_drv_init_flag == 1) {
        debug_printf("[erro] fatfs already init\r\n");
        return FATFS_FUN_RET_ERR;
    }
    res_flash = f_mount(&fat_fs, FATFS_PATH, 1);

    if (res_flash == FR_NO_FILESYSTEM) {
#if (FATFS_DRV_MKFS_FUN)
        /* 格式化 */
        res_flash = f_mkfs(FATFS_PATH, 0, work, sizeof(work));
        if (res_flash == FR_OK) {
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL, FATFS_PATH, 1);

            /* 重新挂载	*/
            res_flash = f_mount(&fat_fs, FATFS_PATH, 1);
            
        } else {
            debug_printf("mkfs failed\r\n");
        }
#endif
    }
    if (res_flash != FR_OK) {
        debug_printf("[error] res_mount = %d\r\n", res_flash);
        return FATFS_FUN_RET_ERR;
    }

    debug_printf("\r\n****** mount successful ******\r\n");
    _drv_init_flag = 1;
    return FATFS_FUN_RET_OK;
}

int hl_drv_fatfs_deinit()
{
    FRESULT res_flash;  //文件操作结果

    if (_drv_init_flag == 0) {
        return FATFS_FUN_RET_ERR;
    }
    res_flash = f_mount(NULL, FATFS_PATH, 1);  //卸载文件系统
    if (res_flash != FR_OK) {
        return FATFS_FUN_RET_ERR;
    }

    _drv_init_flag = 0;
    return FATFS_FUN_RET_OK;
}

int hl_drv_fatfs_open(FIL* fnew, char* filename, uint8_t op_flag)
{
    FRESULT res_flash;  //文件操作结果
    char    path_string[20] = { FATFS_PATH };

    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }

    strcat(path_string, filename);  //将磁盘路径和文件名称组合成一个文件路径

    res_flash = f_open(fnew, path_string, op_flag);  //只读
    if (res_flash != FR_OK) {
        debug_printf("res_open = %d\r\n", res_flash);
        return FATFS_FUN_RET_ERR;
    }

    return FATFS_FUN_RET_OK;
}

int hl_drv_fatfs_close(FIL* fnew)
{
    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }
    /* 不再读写，关闭文件 */
    f_close(fnew);

    return FATFS_FUN_RET_OK;
}

int hl_drv_fatfs_read(FIL* fnew, uint8_t* read_buffer, uint32_t data_size)
{
    FRESULT res_flash;  //文件操作结果
    UINT    fnum;       //文件成功读写数量

    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }

    res_flash = f_read(fnew, read_buffer, data_size, &fnum);
    if (res_flash != FR_OK) {
        return FATFS_FUN_RET_ERR;
    }

    return fnum;
}

#if (FATFS_DRV_WRITE_FUN)
int hl_drv_fatfs_write(FIL* fnew, uint8_t* read_buffer, uint32_t data_size)
{
    UINT    fnum;       //文件成功读写数量
    FRESULT res_flash;  //文件操作结果

    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }

    /* 将指定存储区内容写入到文件内 */
    res_flash = f_write(&fnew, write_buffer, data_size, &fnum);

    if (res_flash == FR_OK) {
        debug_printf("文件写入成功，写入字节数据：%d\r\n", fnum);
    } else {
        debug_printf("[erro] 文件写入失败, res_flash = %d\r\n", res_flash);
        return FATFS_FUN_RET_ERR;
    }

    return fnum;
}
#endif

int hl_drv_fatfs_del_file(char* filename)
{
    FRESULT res_flash;  //文件操作结果
    char    path_string[20] = { FATFS_PATH };

    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }

    strcat(path_string, filename);  //将磁盘路径和文件名称组合成一个文件路径

    res_flash = f_unlink(path_string);
    debug_printf("unlink ret = %d\r\n", res_flash);
    if (res_flash != FR_OK) {
        return FATFS_FUN_RET_ERR;
    }

    return FATFS_FUN_RET_OK;
}

int hl_drv_fatfs_get_file_size(char* filename)
{
    FRESULT res_flash;  //文件操作结果
    FILINFO file_info;  //文件信息结构体
    char    path_string[20] = { FATFS_PATH };

    if (_drv_init_flag == 0) {
        debug_printf("[erro] fatfs no init\r\n");
        return FATFS_FUN_RET_ERR;
    }
    strcat(path_string, filename);  //将磁盘路径和文件名称组合成一个文件路径

    res_flash = f_stat(filename, &file_info);
    debug_printf("f_stat ret = %d\r\n", res_flash);
    if (res_flash != FR_OK) {
        return FATFS_FUN_RET_ERR;
    }

    return file_info.fsize;
}

/*
 * EOF
 */