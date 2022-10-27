/**
 * @file hl_drv_flash_test.c
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

#include "rtthread.h"
#include "hl_drv_flash.h"
#include "stdlib.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


int hl_drv_flash_test(int argc, char *argv[])
{
    int count, len, ret;
    uint8_t data[50] = {0};     //这里数组过大，会导致线程空间不足，程序出现停止运行的可能

    if (argc <= 1) {
        rt_kprintf("format : [cmd datalen] max_len:50\n");
        return -1;
    }
    len = atoi(argv[1]);
    if (len > 50) {
        rt_kprintf("[error] max_len:50\n");
        return -1;
    }

    /* flash初始化 */
    hl_drv_flash_init();
    
    for (count = 0; count < len; count++) {
        data[count] = count;
    }

    /* 往flash中写入len个数据，可以跨页写 */
    ret = hl_drv_flash_write(0x000000, data, len);
    if (ret == FLASH_RET_OK) {
        rt_kprintf("write [%d] data ok\n", len);
    }

    rt_memset(data, 0, sizeof(data));

    /* 从flash中读取len个数据，并打印 */
    hl_drv_flash_read(0x000000, data, len);

    for (count = 0; count < len; count++) {
        rt_kprintf("data[%d]:%d\t", count, data[count]);
        if (count % 5 == 0) {
            rt_kprintf("\n");
        }
    }
    rt_kprintf("\nread [%d] data end\n", len);

    return 0;
}

MSH_CMD_EXPORT(hl_drv_flash_test, flash driver test);
/*
 * EOF
 */