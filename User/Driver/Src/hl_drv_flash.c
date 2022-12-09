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
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_WritePage 0x02
#define W25X_SectorErase 0x20
#define W25X_BlockErase 0xD8
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
#define W25X_Enable_Reset 0x66
#define W25X_Reset 0x99

/* variables -----------------------------------------------------------------*/
#if (HL_DRV_FLASH_TYPE)
    
#else
static hl_hal_soft_spi_info spi_info = { .spi_mosi_pin_num = 15,
                                         .spi_miso_pin_num = 14,
                                         .spi_sck_pin_num  = 13,
                                         .spi_cs_pin_num   = 12,
                                         .spi_mosi_pin     = GPIO_PIN_15,
                                         .spi_miso_pin     = GPIO_PIN_14,
                                         .spi_sck_pin      = GPIO_PIN_13,
                                         .spi_cs_pin       = GPIO_PIN_12,
                                         .gpiox_base       = GPIOB_BASE,
                                         .rcc_periph_gpiox = RCC_APB2_PERIPH_GPIOB,
                                         .gpiox            = GPIOB };
#endif

static bool _flash_init_flag = false;

static bool _flash_mutex_enbale_flag = false;

static struct rt_mutex _flash_mutex = { 0 };


/* Private function(only *.c)  -----------------------------------------------*/

static int _flash_mutex_enable()
{
    _flash_mutex_enbale_flag = true;
    return 0;
}

INIT_APP_EXPORT(_flash_mutex_enable);

static void hl_drv_flash_write_enable(void)
{
#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();

    hl_hal_hard_spi_send_recv(W25X_WriteEnable);  //写使能指令

    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    hl_hal_soft_spi_send_recv(&spi_info, W25X_WriteEnable);  //写使能指令

    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif
}

static void hl_drv_flash_write_disable(void)
{
#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();

    hl_hal_hard_spi_send_recv(W25X_WriteDisable);  //写禁止指令

    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    hl_hal_soft_spi_send_recv(&spi_info, W25X_WriteDisable);  //写禁止指令

    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif
}

static uint8_t hl_drv_flash_read_status_reg01(void)
{
    uint8_t sr1_data = 0;

#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();

    hl_hal_hard_spi_send_recv(W25X_ReadStatusReg);  //读SR1寄存器指令
    sr1_data = hl_hal_hard_spi_send_recv(0xFF);     //传一个无意义的数据

    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    hl_hal_soft_spi_send_recv(&spi_info, W25X_ReadStatusReg);  //读SR1寄存器指令
    sr1_data = hl_hal_soft_spi_send_recv(&spi_info, 0xFF);     //传一个无意义的数据

    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    return sr1_data;
}

static int hl_drv_flash_wait_write_enable_ok(void)
{
    uint8_t reg_val = 0;
    int num = 0;
    while (1) {
        reg_val = hl_drv_flash_read_status_reg01() & 0x02;
        num++;
        if (reg_val == 0x02) {
            break;
        }
         if (num > 1000 * 1000 * 1) {
             return FLASH_RET_ERR;
         }
    }
    return FLASH_RET_OK;
}

static int hl_drv_flash_wait_write_end(void)
{
    uint8_t reg_val = 0;
    int num = 0;
    while (1) {
        reg_val = hl_drv_flash_read_status_reg01() & 0x01;
        num++;
        if (reg_val != 0x01) {
            break;
        }
         if (num > 1000 * 1000 * 1) {
             return FLASH_RET_ERR;
         }
    }
    return FLASH_RET_OK;
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
static int hl_drv_flash_write_page(uint32_t addr, uint8_t* w_data, int32_t len)
{
    uint8_t ret = 0;
    if (w_data == RT_NULL || _flash_init_flag == false) {
        debug_printf("[error] hl_drv_flash_write_page 2\n");
        return FLASH_RET_ERR;
    }
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_take(&_flash_mutex, RT_WAITING_FOREVER); 
    }
    hl_drv_flash_write_enable();
    ret = hl_drv_flash_wait_write_enable_ok();  //等待完全写使能
    if (ret == FLASH_RET_ERR) {
        debug_printf("[error] hl_drv_flash_write_page 1\n");
        return FLASH_RET_ERR;
    }

#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_WritePage);         //发送写页指令   0x02
    hl_hal_hard_spi_send_recv(((addr) >> 16) & 0xff);  //发送24bit地址
    hl_hal_hard_spi_send_recv(((addr) >> 8) & 0xff);
    hl_hal_hard_spi_send_recv(addr & 0xff);
    while (len--) {
        hl_hal_hard_spi_send_recv(*w_data);
        w_data++;
    }
    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    hl_hal_soft_spi_send_recv(&spi_info, W25X_WritePage);         //发送写页指令   0x02
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);  //发送24bit地址
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);
    hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);
    while (len--) {
        hl_hal_soft_spi_send_recv(&spi_info, *w_data);
        w_data++;
    }
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    hl_drv_flash_write_disable();
    ret = hl_drv_flash_wait_write_end();  // 等待写入结束
    if (ret == FLASH_RET_ERR) {
        debug_printf("[error] hl_drv_flash_write_page 2\n");
        return FLASH_RET_ERR;
    }
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_release(&_flash_mutex); 
    }
    return FLASH_RET_OK;
}

static uint16_t hl_drv_flash_read_maker_dev_id(void)
{
    uint16_t reg_val = 0;

#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_ManufactDeviceID);  //发送读取制造商ID命令 0x90
    hl_hal_hard_spi_send_recv(0x00);
    hl_hal_hard_spi_send_recv(0x00);
    hl_hal_hard_spi_send_recv(0x00);
    reg_val |= hl_hal_hard_spi_send_recv(0xff) << 8;
    reg_val |= hl_hal_hard_spi_send_recv(0xff);
    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    hl_hal_soft_spi_send_recv(&spi_info, W25X_ManufactDeviceID);  //发送读取制造商ID命令 0x90
    hl_hal_soft_spi_send_recv(&spi_info, 0x00);
    hl_hal_soft_spi_send_recv(&spi_info, 0x00);
    hl_hal_soft_spi_send_recv(&spi_info, 0x00);

    reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff) << 8;
    reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff);

    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    return reg_val;
}

static uint32_t hl_drv_flash_read_id(void)
{
    uint32_t reg_val = 0, ret = 0;

#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();

    ret = hl_hal_hard_spi_send_recv(W25X_JedecDeviceID);  //发送读取ID命令 0x9f
    if (ret == HARD_SPI_FUNC_RET_ERR) {
        debug_printf("[error] hl_drv_flash_read_id\n");
        return FLASH_RET_ERR;
    }

    reg_val |= hl_hal_hard_spi_send_recv(0xff) << 16;
    reg_val |= hl_hal_hard_spi_send_recv(0xff) << 8;
    reg_val |= hl_hal_hard_spi_send_recv(0xff);

    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    hl_hal_soft_spi_send_recv(&spi_info, W25X_JedecDeviceID);  //发送读取ID命令 0x9f

    reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff) << 16;
    reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff) << 8;
    reg_val |= hl_hal_soft_spi_send_recv(&spi_info, 0xff);

    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    //rt_kprintf("flash ID :0x%x\n", reg_val);
    return reg_val;
}

static void hl_drv_flash_reset_device()
{

#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_Enable_Reset);  //启用复位
    HARD_SPI_CS_HIGH();

    //rt_thread_mdelay(1);

    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_Reset);  //复位
    HARD_SPI_CS_HIGH();

#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    hl_hal_soft_spi_send_recv(&spi_info, W25X_Enable_Reset);  //启用复位
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);

    //rt_thread_mdelay(1);

    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    hl_hal_soft_spi_send_recv(&spi_info, W25X_Reset);  //复位
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif
    //rt_thread_mdelay(1);
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
static uint8_t hl_drv_flash_erase_sector(uint32_t addr)
{
    uint8_t ret = 0;
    if (_flash_init_flag == false) {
        debug_printf("[error] hl_drv_flash_erase_sector 0\n");
        return FLASH_RET_ERR;
    }
    //addr *= 4096;
    addr &= 0xFFF000;  //得到这个地址对应的扇区首地址
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_take(&_flash_mutex, RT_WAITING_FOREVER); 
    }
    hl_drv_flash_write_enable();
    ret = hl_drv_flash_wait_write_enable_ok();  //等待完全写使能
    if (ret == FLASH_RET_ERR) {
        debug_printf("[error] hl_drv_flash_erase_sector 1\n");
        return FLASH_RET_ERR;
    }
#if (HL_DRV_FLASH_TYPE)
    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_SectorErase);       //发送扇区擦除指令
    hl_hal_hard_spi_send_recv(((addr) >> 16) & 0xff);  //发送24bit地址
    hl_hal_hard_spi_send_recv(((addr) >> 8) & 0xff);
    hl_hal_hard_spi_send_recv(addr & 0xff);
    HARD_SPI_CS_HIGH();
#else
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    hl_hal_soft_spi_send_recv(&spi_info, W25X_SectorErase);       //发送扇区擦除指令
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);  //发送24bit地址
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);
    hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    hl_drv_flash_write_disable();
    ret = hl_drv_flash_wait_write_end();  //等待擦除完成
    if (ret == FLASH_RET_ERR) {
        debug_printf("[error] hl_drv_flash_erase_sector 2\n");
        return FLASH_RET_ERR;
    }
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_release(&_flash_mutex); 
    }
    return FLASH_RET_OK;
}

/**
 * @brief  擦除FLASH扇区，整片擦除
 * @param  无
 * @retval 无
 */
static int hl_drv_flash_erase_chip(void)
{
    if (_flash_init_flag == false) {
        debug_printf("[error] hl_drv_flash_erase_chip\n");
        return FLASH_RET_ERR;
    }
    /* 发送FLASH写使能命令 */
    hl_drv_flash_write_enable();
    hl_drv_flash_wait_write_enable_ok();

#if (HL_DRV_FLASH_TYPE)
    /* 整块 Erase */
    HARD_SPI_CS_LOW();
    hl_hal_hard_spi_send_recv(W25X_ChipErase);  //启用复位
    HARD_SPI_CS_HIGH();
#else
    /* 整块 Erase */
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
    hl_hal_soft_spi_send_recv(&spi_info, W25X_ChipErase);  //发送芯片擦除指令
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif

    /* 等待擦除完毕*/
    hl_drv_flash_wait_write_end();

    return FLASH_RET_OK;
}
/* Exported functions --------------------------------------------------------*/

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
int hl_drv_flash_read(uint32_t addr, uint8_t* r_data, uint32_t len)
{
    uint32_t reg_val;
    if (r_data == RT_NULL || _flash_init_flag == false) {
        debug_printf("[error] hl_drv_flash_read 1 \n");
        return FLASH_RET_ERR;
    }
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_take(&_flash_mutex, RT_WAITING_FOREVER); 
    }
#if HL_DRV_FLASH_TYPE
    /* 硬件SPI读取FLASH */
    HARD_SPI_CS_LOW();
    reg_val = hl_hal_hard_spi_send_recv(W25X_ReadData);  //发送读取数据命令0x03
    if (reg_val == HARD_SPI_FUNC_RET_ERR) {
        debug_printf("[error] hl_drv_flash_read 2 \n");
        return FLASH_RET_ERR;
    }
    /*发送24bit地址*/
    hl_hal_hard_spi_send_recv(((addr) >> 16) & 0xff);  //高八位
    hl_hal_hard_spi_send_recv(((addr) >> 8) & 0xff);   //中八位
    hl_hal_hard_spi_send_recv(addr & 0xff);            //低八位
    while (len--) {
        *r_data = hl_hal_hard_spi_send_recv(0xff);
        r_data++;
    }
    HARD_SPI_CS_HIGH();
#else
    /* 软件SPI读取FLASH */
    HL_HAL_SPI_CS_LOW(spi_info.spi_cs_pin_num, spi_info.gpiox_base);  //片选线拉低
    hl_hal_soft_spi_send_recv(&spi_info, W25X_ReadData);              //发读数据指令
    /*发送24bit地址*/
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 16) & 0xff);  //高八位
    hl_hal_soft_spi_send_recv(&spi_info, ((addr) >> 8) & 0xff);   //中八位
    hl_hal_soft_spi_send_recv(&spi_info, addr & 0xff);            //低八位
    while (len--) {
        *r_data = hl_hal_soft_spi_send_recv(&spi_info, 0xff);
        r_data++;
    }
    HL_HAL_SPI_CS_HIGH(spi_info.spi_cs_pin_num, spi_info.gpiox_base);
#endif
    if (_flash_mutex_enbale_flag == true) {
        rt_mutex_release(&_flash_mutex);
    }
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
int hl_drv_flash_write(uint32_t addr, uint8_t* w_data, uint32_t len)
{
    uint8_t  ret;
    uint32_t pager;
    if (w_data == RT_NULL || _flash_init_flag == false) {
        debug_printf("[error] hl_drv_flash_write 1\n");
        return FLASH_RET_ERR;
    }
    
    pager = 256 - addr % 256;  //单页剩余的字节数
    if (len <= pager) {
        pager = len;  //不大于256字节
    }

    ret = hl_drv_flash_erase_sector(addr);
    if (ret == FLASH_RET_ERR) {
        return FLASH_RET_ERR;
    }

    while (1) {

        ret = hl_drv_flash_write_page(addr, w_data, pager);
        if (ret == FLASH_RET_ERR) {
            debug_printf("[error] hl_drv_flash_write failed 2\n");
            return FLASH_RET_ERR;
        }
        if (len == pager) {
            break;  //全部写入
        }
        w_data += pager;
        addr += pager;
        len -= pager;  //减去已经写入的字节数
        if (len > 256) {
            pager = 256;  //一次写入256字节
        } else {
            pager = len;  //已经不足256字节
        }
    }

    return FLASH_RET_OK;
}

int hl_drv_flash_init(void)
{
    if (_flash_init_flag == true) {
        debug_printf("[error] flash already init\n");
        return FLASH_RET_ERR;
    }

#if (HL_DRV_FLASH_TYPE)
    /* 硬件SPI引脚初始化 */
    hl_hal_hard_spi_init();
#else
    /* 软件SPI引脚初始化 */
    hl_hal_soft_spi_init(&spi_info);
#endif
    if (W25XXX_ID != hl_drv_flash_read_id()) {
        debug_printf("[error] hl_drv_flash_init\n");
        return FLASH_RET_ERR;
    }
    rt_mutex_init(&_flash_mutex, "flash__mutex", RT_IPC_FLAG_PRIO);

    _flash_init_flag = true;
    return FLASH_RET_OK;
}

int hl_drv_flash_deinit(void)
{
    if (_flash_init_flag == false) {
        return FLASH_RET_ERR;
    }

#if (HL_DRV_FLASH_TYPE)
    hl_hal_hard_spi_deinit();
#else
    hl_hal_soft_spi_deinit(&spi_info);
#endif

    rt_mutex_detach(&_flash_mutex);

    _flash_init_flag = false;
    return FLASH_RET_OK;
}
/*
 * EOF
 */
