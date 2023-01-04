/**
 * @file hl_RTC_PCF85063ATL.c
 * @author libo (rd46@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-09-06
 * 
 * @copyright Copyright (c) 2022 hollyland
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date           <th>Version  <th>Author         <th>Description
 * <tr><td>2022-09-06     <td>v1.0     <td>libo     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/*
 * EOF
 */

#include "hl_drv_pcf85063atl.h"

#define RTC_PCF85063_ADDR 0x51                         /* 从机地址 */
static hl_hal_soft_i2c_numb_e i2c = HL_HAL_SOFT_I2C_NUMB_1;
/**
 * 
 * @brief 写寄存器 
 * @date 2022-09-07
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-09-07      <td>libo     <td>新建
 * </table>
 */
static rt_err_t write_regs(rt_uint8_t reg, rt_uint8_t* data, rt_uint8_t len)
{
    /* 调用I2C设备接口传输数据 */
    if (!hl_hal_soft_i2c_api_write(i2c, RTC_PCF85063_ADDR, reg, data, (uint16_t)len)) {
        //rt_kprintf("RTC_PCF85063ATL  write_regs  ok\n");
        return RT_EOK;
    } else {
        rt_kprintf(" hl_drv_pcf85063atl.c write_regs  error!\n");
        return -RT_ERROR;
    }
}

/**
 * 
 * @brief 读寄存器数据 
 * @date 2022-09-07
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-09-07       <td>libo           <td>新建
 * </table>
 */
static rt_err_t read_regs(rt_uint8_t reg, rt_uint8_t* data, rt_uint8_t len)
{
    /* 调用I2C设备接口传输数据 */
    if (!hl_hal_soft_i2c_api_read(i2c, RTC_PCF85063_ADDR, reg, data, (uint16_t)len)) {
        return RT_EOK;
    } else {
        rt_kprintf("hl_drv_pcf85063atl.c  read_regs  error\n");
        return -RT_ERROR;
    }
}

uint8_t hl_drv_rtc_pcf85063_init()
{
    uint8_t reg_val;
    uint8_t val;

    if (hl_hal_soft_i2c_api_init(HL_HAL_SOFT_I2C_NUMB_1)) {
        rt_kprintf("get hl_drv_rtc_pcf85063_init  fail\n");
        return -1;
    }

    val = 0xA9;
    reg_val = val;

    write_regs(RTC_PCF85063_RAM_byte, &reg_val, 1);

    reg_val = 0;

    read_regs(RTC_PCF85063_RAM_byte, &reg_val, 1);
    if (reg_val != val) {
        rt_kprintf("get hl_drv_rtc_pcf85063_init  fail\n");
        return -1;
    }

    return 0;
}


uint8_t hl_drv_rtc_pcf85063_deinit()
{
    if (hl_hal_soft_i2c_api_deinit(HL_HAL_SOFT_I2C_NUMB_1)) {
        rt_kprintf("RTC_PCF85063ATL  get hl_drv_rtc_pcf85063_init  fail\n");
        return -1;
    }
    return 0;
}

/**
 * 
 * @brief 10进制转16进制 （只改变进制 不变数值）
 * @param [in] data 
 * @return u_int8_t 
 * @date 2022-10-14
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-14      <td>libo     <td>新建
 * </table>
 */
static uint8_t DecimalToBCD(uint8_t data)
{
    uint8_t high = 0;

    while(data >= 10){
        high++;
        data -= 10;
    }
    return((high<<4)|data);
}

/**
 * 
 * @brief 16进制转10进制 （只改变进制 不变数值）
 * @param [in] data 
 * @return u_int8_t 
 * @date 2022-10-14
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-14      <td>libo     <td>新建
 * </table>
 */
static uint8_t BCDToDecimal(uint8_t data)
{
    uint8_t tenweight,oneweight,result;
    oneweight =  data&0x0f;
    tenweight = (data&0xf0)>>4;
    result = tenweight*10 + oneweight*1;
    return result;
}


/**
 * 
 * @brief 获取时间
 * @param [in] ptr 
 * @return uint8_t 
 * @date 2022-09-07
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-09-07      <td>libo     <td>新建
 * </table>
 */
static uint8_t hl_get_rtc_time(void* ptr)
{
    if (read_regs(RTC_PCF85063_Seconds, (rt_uint8_t*) ptr, 7)) {
        rt_kprintf("RTC_PCF85063ATL hl_get_rtc_time read_regs  RTC_PCF85063_Seconds fail\n");
        return -1;
    }
    return 0;
}

/**
 * 
 * @brief 锁住内部计时时钟
 * @return uint8_t 
 * @date 2022-10-11
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-11      <td>libo     <td>新建
 * </table>
 */
static uint8_t hl_lock_rtc_clock(void)
{
    rt_uint8_t reg0x0 = 0x00;
    if (read_regs(RTC_PCF85063_Control_1, &reg0x0, 1)) {
        rt_kprintf("RTC_PCF85063ATL hl_lock_rtc_clock read_regs  RTC_PCF85063_Control_1 fail\n");
        return -1;
    }    

    reg0x0 |=0x20;
    if (write_regs(RTC_PCF85063_Control_1, &reg0x0, 1)) {
        rt_kprintf("RTC_PCF85063ATL hl_lock_rtc_clock write_regs  RTC_PCF85063_Control_1 fail\n");
        return -1;
    }
    return 0;
}

/**
 * 
 * @brief 
 * @return uint8_t 
 * @date 2022-10-11
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-11      <td>libo     <td>新建
 * </table>
 */
static uint8_t hl_unlock_rtc_clock(void)
{
    rt_uint8_t reg0x0 = 0x00;
    if (read_regs(RTC_PCF85063_Control_1, &reg0x0, 1)) {
        rt_kprintf("RTC_PCF85063ATL hl_unlock_rtc_clock read_regs  RTC_PCF85063_Control_1 fail\n");
        return -1;
    }

    reg0x0 &= 0xDF;
    if (write_regs(RTC_PCF85063_Control_1, &reg0x0, 1)) {
        rt_kprintf("RTC_PCF85063ATL hl_unlock_rtc_clock write_regs  RTC_PCF85063_Control_1 fail\n");
        return -1;
    } 
    return 0;
}

/**
 * 
 * @brief 设置时间
 * @param [in] ptr 
 * @return uint8_t 
 * @date 2022-09-19
 * @author libo (rd46@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-09-19      <td>libo     <td>新建
 * </table>
 */
static uint8_t hl_set_rtc_time(void* ptr)
{
    if(ptr == NULL){
        rt_kprintf(" hl_set_rtc_time  param  error\n");
        return 1;
    }

    if(hl_lock_rtc_clock()){
        rt_kprintf("RTC_PCF85063ATL  hl_lock_rtc_clock  fail\n");
        return -1;
    }
    
    if (write_regs(RTC_PCF85063_Seconds, (rt_uint8_t*)ptr, 7)) {
        rt_kprintf("RTC_PCF85063ATL  write_regs RTC_PCF85063_Seconds  fail\n");
        return -1;
    }

    if(hl_unlock_rtc_clock()){
        rt_kprintf("RTC_PCF85063ATL  hl_lock_rtc_clock  fail\n");
        return -1;
    }
    return 0;
}


uint8_t hl_drv_rtc_pcf85063_io_ctrl(uint8_t cmd, void* ptr, uint16_t len)
{
    if(ptr == NULL){
        rt_kprintf(" hl_drv_rtc_pcf85063_io_ctrl  param  error\n");
        return 1;
    }

    uint8_t* ptr1 = (uint8_t*) ptr;
    
    /*将输入参数10进制转换成bcd码*/
    for (uint8_t i = 0; i < len; i++){
        ptr1[i]= DecimalToBCD(ptr1[i]);
    }
    
    switch (cmd) {
        case RTC_GET_TIME:
            if (hl_get_rtc_time(ptr)) {
                rt_kprintf("RTC_PCF85063ATL  read_regs   fail\n");
                return -1;
            }
            break;
        case RTC_SET_TIME:
            if (hl_set_rtc_time(ptr)) {
                rt_kprintf("RTC_PCF85063ATL  RTC_SET_TIME   fail\n");
                return -1;
            }
            break;
        default:
            break;
    }

    /*将输出参数bcd码转换成10进制*/
    for (uint8_t i = 0; i < len; i++){
         ptr1[i]= BCDToDecimal(ptr1[i]);
    }
    return 0;
}