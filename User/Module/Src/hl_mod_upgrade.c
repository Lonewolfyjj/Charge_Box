/**
 * @file hl_mod_upgrade.c
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
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_upgrade.h"
#include "string.h"
#include "hl_mod_typedef.h"

/* typedef -------------------------------------------------------------------*/

#pragma pack(push)  //保存之前的对齐方式
#pragma pack(1)     //以下内容设置为1字节对齐

#if 0
typedef struct Fat12Header
{
    uint8_t  BS_OEMName[8];      // OEM字符串,必须为8个字符,不足以空格填空
    uint16_t BPB_BytsPerSec;     // 每扇区字节数
    uint8_t  BPB_SecPerClus;     // 每簇占用的扇区数
    uint16_t BPB_RsvdSecCnt;     // Boot占用的扇区数
    uint8_t  BPB_NumFATs;        // FAT表的记录数
    uint16_t BPB_RootEntCnt;     // 最大根目录文件数
    uint16_t BPB_TotSec16;       // 逻辑扇区总数
    uint8_t  BPB_Media;          // 媒体描述符
    uint16_t BPB_FATSz16;        // 每个FAT占用扇区数
    uint16_t BPB_SecPerTrk;      // 每个磁道扇区数
    uint16_t BPB_NumHeads;       // 磁头数
    uint32_t BPB_HiddSec;        // 隐藏扇区数
    uint32_t BPB_TotSec32;       // 如果BPB_TotSec16是0,则在这里记录
    uint8_t  BS_DrvNum;          // 中断13的驱动器号
    uint8_t  BS_Reserved1;       // 未使用
    uint8_t  BS_BootSig;         // 扩展引导标志
    uint32_t BS_VolID;           // 卷序列号
    uint8_t  BS_VolLab[11];      // 卷标,必须是11个字符,不足以空格填充
    uint8_t  BS_FileSysType[8];  // 文件系统类型,必须是8个字符,不足填充空格
} hl_mod_upgrade_fat12header_st;
#endif
typedef struct _hl_mod_upgrade_file_info_st
{
    char     DIR_Name[11];      //文件名
    uint8_t  DIR_Attr;          //文件属性
    uint8_t  reserve[10];       //保留值
    uint16_t DIR_WrtTime;       //最后一次写入时间
    uint16_t DIR_WrtDate;       //最后一次写入日期
    uint16_t DIR_FstClus;       //文件数据起始簇
    uint32_t DIR_FileSize;      //文件大小
} hl_mod_upgrade_file_info_st;

#pragma pack(pop)  //恢复到push指令之前的对齐方式

typedef struct _hl_mod_upgrade_st
{
    void*            msg_hd;
    bool             init_flag;
    bool             start_flag;
    struct rt_thread ui_thread_fd;
    int              thread_exit_flag;
} hl_mod_upgrade_st;

typedef enum _hl_mod_upgrade_file_dec_e
{
    UPGRADE_NO_FILE,
    UPGRADE_FILE_OK
} hl_mod_upgrade_file_dec_e;

/* define --------------------------------------------------------------------*/

#define DBG_SECTION_NAME "mod_upgrade"
#define DBG_LEVEL DBG_INFO
#include <rtdbg.h>

#define UPGRADE_THREAD_STACK_SIZE    512

#define UPGRADE_FILE_TOTAL           512    //升级文件总容量

/* variables -----------------------------------------------------------------*/

static hl_mod_upgrade_st _upgrade_info = {
    .msg_hd = NULL, 
    .init_flag = false, 
    .start_flag = false, 
    .ui_thread_fd = { 0 }, 
    .thread_exit_flag = -1
};

//static hl_mod_upgrade_fat12header_st _fat12_head;   //文件系统的头信息

static hl_mod_msg_handle_st _upgrade_msg_hd;

static uint8_t _upgrade_thread_stack[UPGRADE_THREAD_STACK_SIZE] = { 0 };

extern int flash_write_use_state;

/* Private function(only *.c)  -----------------------------------------------*/

#if 0
static void _upgrade_fat12_header_printf(hl_mod_upgrade_fat12header_st rf)
{
    rt_kprintf("BS_OEMName:%s \t// OEM字符串, 必须为8个字符, 不足以空格填空\n", rf.BS_OEMName);
    rt_kprintf("BPB_BytsPerSec:%d \t// 每扇区字节数\n", rf.BPB_BytsPerSec);
    rt_kprintf("BPB_SecPerClus:%d \t// 每簇占用的扇区数\n", rf.BPB_SecPerClus);
    rt_kprintf("BPB_RsvdSecCnt:%d \t// Boot占用的扇区数\r\n", rf.BPB_RsvdSecCnt);
    rt_kprintf("BPB_NumFATs:%d \t\t// FAT表的记录数\n", rf.BPB_NumFATs);
    rt_kprintf("BPB_RootEntCnt:%d \t// 最大根目录文件数\n", rf.BPB_RootEntCnt);
    rt_kprintf("BPB_TotSec16:%d \t// 逻辑扇区总数\n", rf.BPB_TotSec16);
    rt_kprintf("BPB_Media:%d \t\t// 媒体描述符\n", rf.BPB_Media);
    rt_kprintf("BPB_FATSz16:%d \t\t// 每个FAT占用扇区数\n", rf.BPB_FATSz16);
    rt_kprintf("BPB_SecPerTrk:%d \t// 每个磁道扇区数\n", rf.BPB_SecPerTrk);
    rt_kprintf("BPB_NumHeads:%d \t\t// 磁头数\n", rf.BPB_NumHeads);
    rt_kprintf("BPB_HiddSec:%d \t\t// 隐藏扇区数\n", rf.BPB_HiddSec);
    rt_kprintf("BPB_TotSec32:%d \t\t// 如果BPB_TotSec16是0,则在这里记录\n", rf.BPB_TotSec32);
    rt_kprintf("BS_DrvNum:%d \t\t/ 中断13的驱动器号\n", rf.BS_DrvNum);
    rt_kprintf("BS_Reserved1:%d \t\t// 未使用\n", rf.BS_Reserved1);
    rt_kprintf("BS_BootSig:%d \t\t// 扩展引导标志\n", rf.BS_BootSig);
    rt_kprintf("BS_VolID:%d \t// 卷序列号\n", rf.BS_VolID);
    rt_kprintf("BS_VolLab:%s \t // 卷标, 必须是11个字符,不足以空格填充\n", rf.BS_VolLab);
    rt_kprintf("BS_FileSysType:%s \t // 文件系统类型,必须是8个字符,不足填充空格\n", rf.BS_FileSysType);
}

static void _upgrade_file_info_printf(hl_mod_upgrade_file_info_st file_info)
{
    rt_kprintf("DIR_Name:%s \n", file_info.DIR_Name);
    rt_kprintf("DIR_Attr:%d \n", file_info.DIR_Attr);
    rt_kprintf("DIR_WrtTime:%d \r\n", file_info.DIR_WrtTime);
    rt_kprintf("DIR_WrtDate:%d \n", file_info.DIR_WrtDate);
    rt_kprintf("DIR_FstClus:%d \n", file_info.DIR_FstClus);
    rt_kprintf("DIR_FileSize:%d \n", file_info.DIR_FileSize);
}
#endif

static int _mod_msg_send(uint8_t cmd, void* param, uint16_t len)
{
    if (_upgrade_info.msg_hd != RT_NULL && _upgrade_msg_hd.msg_send != RT_NULL) {
        _upgrade_msg_hd.msg_send(_upgrade_msg_hd.msg_id, cmd, param, len);
        return HL_MOD_UPGRADE_FUNC_OK;
    }

    return HL_MOD_UPGRADE_FUNC_ERR;
}

static uint8_t _upgrade_file_find(char* file_name)
{
    hl_mod_upgrade_file_info_st file_info;
    int                         i;
    int                         f_nlen = strlen(file_name);
    uint8_t                     ret = UPGRADE_NO_FILE;

    LOG_I("要找的文件名为:%s \n", file_name);
    LOG_I("start find...\n");

    for (i = 0; i < UPGRADE_FILE_TOTAL; i++) {
        hl_drv_flash_read(UPGRADE_FILE_DIR_ENTRY_ADDR + i * sizeof(file_info), (uint8_t*)&file_info, sizeof(file_info));

        if (file_info.DIR_Name[0] == 0) {   //查找到最后，没有文件了，说明没有找到
            ret = UPGRADE_NO_FILE;
            break;
        }

        if (strncmp(file_name, file_info.DIR_Name, f_nlen) == 0) {
            LOG_I("文件存在，已找到, %d\n", i);
            //_upgrade_file_info_printf(file_info);
            ret = UPGRADE_FILE_OK;
            break;
        }
    }
    return ret;
}

static void _upgrade_thread_entry(void* arg)
{
    uint8_t f_ret = 0, count = 0, find_sign = 0;
    while (_upgrade_info.thread_exit_flag == 0) {
        
        if (flash_write_use_state == 1) {       //大容量的FLASH写还在使用，重新计超时时间
            flash_write_use_state = 0;
            count = 0;
            find_sign = 1;
            LOG_I("flash write used\n");
        }
        
        if (count >= 5) {                       //5秒之后，大容量的FLASH写没有使用时，进行升级文件检测

            if (find_sign == 1) {
                find_sign = 0;
                f_ret = _upgrade_file_find(UPGRADE_FILE_NAME);
                if (UPGRADE_FILE_OK == f_ret) {

                    LOG_I("file exist, start reset...\n");
                    //__NVIC_SystemReset();     //重启
                    _mod_msg_send(HL_MOD_UPGRADE_FILE_EXIST_MSG, RT_NULL, 0);
                }
            }
            
        } else {
            count++;
        }

        rt_thread_mdelay(1000);
    }

    _upgrade_info.thread_exit_flag = -1;
}


/* Exported functions --------------------------------------------------------*/

int hl_mod_upgrade_start(void)
{
    rt_err_t rt_err;

    if (_upgrade_info.init_flag == false) {
        LOG_E("upgrade mod no init!");
        return HL_MOD_UPGRADE_FUNC_ERR;
    }

    if (_upgrade_info.start_flag == true) {
        LOG_E("upgrade mod already start!");
        return HL_MOD_UPGRADE_FUNC_OK;
    }
    _upgrade_info.thread_exit_flag = 0;

    rt_err = rt_thread_init(&(_upgrade_info.ui_thread_fd), "upgrade_thread", _upgrade_thread_entry, RT_NULL, _upgrade_thread_stack,
                            sizeof(_upgrade_thread_stack), 6, 10);
    if (rt_err == RT_ERROR) {
        LOG_E("upgrade thread create failed");
        return HL_MOD_UPGRADE_FUNC_ERR;
    }
    rt_thread_startup(&(_upgrade_info.ui_thread_fd));

    _upgrade_info.start_flag = true;

    return HL_MOD_UPGRADE_FUNC_OK;
}

int hl_mod_upgrade_stop(void)
{
    if (_upgrade_info.init_flag == false) {
        LOG_E("ui mod not init!");
        return HL_MOD_UPGRADE_FUNC_ERR;
    }

    if (_upgrade_info.start_flag == false) {
        LOG_E("ui mod not start!");
        return HL_MOD_UPGRADE_FUNC_OK;
    }

    _upgrade_info.thread_exit_flag = 1;

    LOG_I("wait ui thread exit");

    while (_upgrade_info.thread_exit_flag != -1) {
        rt_thread_mdelay(10);
    }

    LOG_I("ui mod stop success!");

    _upgrade_info.start_flag = false;

    return HL_MOD_UPGRADE_FUNC_OK;
}

int hl_mod_upgrade_init(void *msg_hd)
{
    int ret;
    if (_upgrade_info.init_flag == true) {
        LOG_E("upgrade mod already inited!");
        return HL_MOD_UPGRADE_FUNC_OK;
    }

    hl_drv_flash_init();
 
    /* 读取文件系统的头部信息 */
    // ret= hl_drv_flash_read(UPGRADE_FILE_SYSTEM_ADDR, (uint8_t*)&_fat12_head, sizeof(_fat12_head));
    // if (ret == FLASH_RET_ERR) {
    //     LOG_E("upgrade mod init failed!");
    //     return HL_MOD_UPGRADE_FUNC_ERR;
    // }

    if (msg_hd != RT_NULL) {
        _upgrade_msg_hd.msg_id   = ((hl_mod_msg_handle_st*)(msg_hd))->msg_id;
        _upgrade_msg_hd.msg_send = ((hl_mod_msg_handle_st*)(msg_hd))->msg_send;
        _upgrade_info.msg_hd  = &_upgrade_msg_hd;
    } else {
        _upgrade_info.msg_hd = RT_NULL;
    }

    LOG_I("upgrade mod init success");

    _upgrade_info.msg_hd = msg_hd;

    _upgrade_info.init_flag = true;
    return HL_MOD_UPGRADE_FUNC_OK;
}

int hl_mod_upgrade_deinit(void)
{
    if (_upgrade_info.init_flag == false) {
        LOG_E("upgrade mod no init!");
        return HL_MOD_UPGRADE_FUNC_OK;
    }

    hl_drv_flash_deinit();

    _upgrade_info.msg_hd = NULL;

    _upgrade_info.init_flag = false;
    return HL_MOD_UPGRADE_FUNC_OK;
}

int hl_mod_upgrade_ctrl(uint8_t op_cmd, void *arg, int32_t arg_size)
{
    if (_upgrade_info.init_flag == false) {
        LOG_E("upgrade mod no init!");
        return HL_MOD_UPGRADE_FUNC_ERR;
    }
    switch (op_cmd) {
        case HL_MOD_UPGRADE_FILE_CHECK:
            flash_write_use_state = 1;
            break;
        case 1:
            break;
        default:
            break;
    }
    return HL_MOD_UPGRADE_FUNC_OK;
}

/*
 * EOF
 */