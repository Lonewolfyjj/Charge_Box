/**
 * @file hl_hal_soft_i2c.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-17
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
 * <tr><td>2022-10-17     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "hl_hal_soft_i2c.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/

#define DBG_SECTION_NAME "hal_soft_i2c"
#define DBG_LEVEL DBG_WARNING
#include <rtdbg.h>

/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/

/**
 * @brief 配置SDA引脚为推挽输出模式
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void SDA_OUT(soft_i2c_info *info)
{
    GPIO_InitType gpio_init_struct;

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin     = info->gpio_pin_sdax;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
    gpio_init_struct.GPIO_Mode    = GPIO_Mode_Out_OD;
    gpio_init_struct.GPIO_Pull = GPIO_No_Pull;    
    GPIO_InitPeripheral(info->gpiox, &gpio_init_struct);    
}


/**
 * @brief 配置SDA引脚为上拉输入模式
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void SDA_IN(soft_i2c_info *info)
{
    GPIO_InitType gpio_init_struct;

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = info->gpio_pin_sdax;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Input;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
    gpio_init_struct.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitPeripheral(info->gpiox, &gpio_init_struct);
}


/**
 * @brief 控制SDA和SCL的输出电平，产生一个起始信号,在SCL的高电平期间，拉低SDA
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void i2c_start(soft_i2c_info *info)
{
    SDA_OUT(info);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
    IIC_SDA(info->sda_pin_num, info->gpiox_base) = 1;
    info->delay_us(IIC_TIME * 2);
    IIC_SDA(info->sda_pin_num, info->gpiox_base) = 0;//在SCK高电平期间  拉低SDA电平  产生下降沿表示启动信号
    info->delay_us(IIC_TIME);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
}


/**
 * @brief 控制SDA和SCL的输出电平，产生一个停止信号，在SCL的高电平期间，拉高SDA
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void i2c_stop(soft_i2c_info *info)
{
    SDA_OUT(info);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
    IIC_SDA(info->sda_pin_num, info->gpiox_base) = 0;
    info->delay_us(IIC_TIME);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
    info->delay_us(IIC_TIME);
    IIC_SDA(info->sda_pin_num, info->gpiox_base) = 1;//在SCL的高电平期间 拉高SDA电平 产生上升沿表示停止信号
    info->delay_us(IIC_TIME);
}


/**
 * @brief   控制SDA和SCL的输出电平，发送一个字节的数据（8个位）
            在SCL的低电平期间，变换SDA的电平，发送一位数据
            在SCL的高电平期间，等待接收者接收一位数据
 * @param [in] w_data 一个字节数据
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void i2c_write_byte(uint8_t w_data, soft_i2c_info *info)
{
    SDA_OUT(info);
    for (uint8_t i = 0; i < 8; i++) {
        IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
        info->delay_us(IIC_TIME);
        if (w_data & (0x80 >> i)) {
            IIC_SDA(info->sda_pin_num, info->gpiox_base) = 1;
        } else {
            IIC_SDA(info->sda_pin_num, info->gpiox_base) = 0;
        }
        info->delay_us(IIC_TIME);
        IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
        info->delay_us(IIC_TIME);    //等待从机接收
    }
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
}

/**
 * @brief   控制SCL的输出电平，读取一个字节数据（8个二进制位）
            在SCL的低电平期间，延时等待发送方发送一位数据
            在SCL的高电平期间，接收一位数据
 * @param [in] info i2c引脚信息结构体地址
 * @return uint8_t 读取到的一字节数据
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static uint8_t i2c_read_byte(soft_i2c_info *info)
{
    uint8_t rdata = 0;

    SDA_IN(info);
    for (int i = 7; i >= 0; i--) {
        IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
        info->delay_us(IIC_TIME);
        IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
        info->delay_us(IIC_TIME);
        rdata |= (READ_SDA(info->sda_pin_num, info->gpiox_base) << i);
        info->delay_us(IIC_TIME);
    }
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
    return rdata;
}



/**
 * @brief   控制SCL的输出电平，产生一个应答信号（ACK）
            在SCL的低电平期间，发送一个电平信号（高电平 或 低电平）
            在SCL的高电平期间，等待对面读取应答信号
 * @param [in] ack 要发送的应答参数
                   ACK(0)为有应答，NOACK(1)为无应答
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static void i2c_write_ack(uint8_t ack, soft_i2c_info *info)
{
    SDA_OUT(info);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
    info->delay_us(IIC_TIME);
    IIC_SDA(info->sda_pin_num, info->gpiox_base) = ack;//发送应答电平信号
    info->delay_us(IIC_TIME);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
    info->delay_us(IIC_TIME);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
}


/**
 * @brief   控制SCL的输出电平，读取应答信号（ACK）
            在SCL的高电平期间，读取应答信号（高电平 或 低电平）
 * @param [in] info i2c引脚信息结构体地址
 * @return uint8_t 有应答返回 ACK(0)
                无应答返回 NOACK(1)
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
static uint8_t i2c_wait_ack(soft_i2c_info *info)
{
    uint16_t err_time = 0;

    SDA_IN(info);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;
    info->delay_us(IIC_TIME);
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 1;
    info->delay_us(IIC_TIME);
    while (READ_SDA(info->sda_pin_num, info->gpiox_base)) {//读取应答信号
        err_time++;
        if(err_time > 250) {
            i2c_stop(info); //无应答
            //LOG_E("[error] no ack");
            return NOACK;
        }    
    } 
    IIC_SCL(info->scl_pin_num, info->gpiox_base) = 0;

    return ACK;   //有应答
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief 根据pro_addr中的地址，进行读取len个数据，存放在rdata中
 * @param [in] info i2c引脚信息结构体地址
 * @param [in] pro_addr 包含设备读写地址，操作地址
 * @param [in] rdata 存放读取到的数据
 * @param [in] len 要读取数据大小
 * @return int 成功返回0，失败返回-1
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
int hl_hal_soft_i2c_read(soft_i2c_info *info, soft_i2c_dev_addr_info *pro_addr, uint8_t *rdata, uint16_t len)
{
    uint16_t count = 0;

    if (info == NULL || pro_addr == NULL || rdata == NULL || len <= 0) {
        LOG_E("[error] hl_hal_soft_i2c_read_data");
        return -1;
    }
    i2c_start(info); 
    i2c_write_byte(pro_addr->dev_writeaddr, info);
    i2c_wait_ack(info); 
    i2c_write_byte(pro_addr->pro_destination_addr, info);
    i2c_wait_ack(info);
    i2c_start(info);
    i2c_write_byte(pro_addr->dev_readaddr, info);   
    i2c_wait_ack(info);
    for (count = 0; count < len - 1; count++) {
        rdata[count] = i2c_read_byte(info);
        i2c_write_ack(ACK, info);
    }
    rdata[count] = i2c_read_byte(info);
    i2c_write_ack(NOACK, info);
    i2c_stop(info);

    return 0; 
}


/**
 * @brief 根据pro_addr中的地址，写入数据
 * @param [in] info i2c引脚信息结构体地址
 * @param [in] pro_addr 地址信息结构体地址，包含设备读写地址，操作地址
 * @param [in] wdata 写入的数据指针
 * @param [in] len 要写入的数据长度
 * @return int 成功返回0，失败返回-1
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
int hl_hal_soft_i2c_write(soft_i2c_info *info, soft_i2c_dev_addr_info *pro_addr, uint8_t *wdata, uint16_t len)
{
    if (info == NULL || pro_addr == NULL || wdata == NULL || len <= 0) {
        LOG_E("[error] hl_hal_soft_i2c_write_data");
        return -1;
    }
    i2c_start(info); 
    i2c_write_byte(pro_addr->dev_writeaddr, info);
    i2c_wait_ack(info); 
    i2c_write_byte(pro_addr->pro_destination_addr, info);
    i2c_wait_ack(info);
    while (len--) {
        i2c_write_byte(*wdata++, info);
        i2c_wait_ack(info);
    }
    i2c_stop(info);
    return 0;
}


/**
 * @brief i2c引脚初始化配置：时钟使能，推挽输出
 * @param [in] info i2c引脚信息结构体地址
 * @return int 成功返回0，失败返回-1
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
int hl_hal_soft_i2c_init(soft_i2c_info *info)
{
    GPIO_InitType gpio_init_struct;
    if (info == NULL) {
        LOG_E("[error] hl_hal_soft_i2c_init");
        return -1;
    }
    RCC_EnableAPB2PeriphClk(info->rcc_periph_gpiox, ENABLE);
    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = info->gpio_pin_sclx | info->gpio_pin_sdax;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
    GPIO_InitPeripheral(info->gpiox, &gpio_init_struct);
    GPIO_SetBits(info->gpiox, info->gpio_pin_sclx | info->gpio_pin_sdax);
    return 0;
}

 
/**
 * @brief i2c去初始化配置
 * @param [in] info i2c引脚信息结构体地址
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
void hl_hal_soft_i2c_deinit(soft_i2c_info *info)
{
    if (info == NULL) {
        LOG_E("[error] hl_hal_soft_i2c_deinit");
        return;
    }
    GPIO_DeInit(info->gpiox); 
}

/**
 * @brief 检测出I2C引脚对应的I2C从机设备地址
 * @param [in] info i2c引脚信息结构体地址
 * @return int 成功返回0，失败返回-1
 * @date 2022-10-17
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-17      <td>yijiujun     <td>新建
 * </table>
 */
int hl_hal_soft_i2c_test_dev_addr(soft_i2c_info *info)
{
    if (info == NULL) {
        LOG_E("[error] hl_hal_soft_i2c_test_dev_addr");
        return 1;
    }
    uint8_t valid_addr[10];
    uint16_t i, count = 0;
    for (uint16_t i = 0x00; i < 255; i += 2) {
        i2c_start(info); 
        i2c_write_byte(i, info);
        if (i2c_wait_ack(info) == ACK) {
            valid_addr[count++] = i;
        } else {
            i2c_stop(info);
        } 
    }
    if (count == 0) {
        LOG_E("[error] this i2c pin no ack");
        return 0;
    }
    for (i = 0; i < count; i++) {
        LOG_I("valid device address %d : 0x%x", i + 1, valid_addr[i]);
    }
    return 0;
}
/*
 * EOF
 */