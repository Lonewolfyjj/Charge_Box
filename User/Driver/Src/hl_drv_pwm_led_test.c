/**
 * @file hl_drv_pwm_led_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-11-15
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
 * <tr><td>2022-11-15     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_drv_pwm_led.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


static void hl_drv_pwm_led_test(int argc, char *argv[])
{
    uint8_t val;
    uint16_t breath_max_val = 200;

    hl_drv_pwm_led_init();

    /* 设置呼吸灯显示的最大亮度值 */
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MAX_VAL, &breath_max_val, sizeof(breath_max_val));

    /* 所有的灯进行呼吸显示 */
    val = HL_DRV_PWM_LED_TX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_TX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_RX;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_BOX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_BOX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_BOX3;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_BOX4;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));

    val = HL_DRV_PWM_LED_BOX5;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));

    rt_thread_mdelay(10000);

    /* 10s钟后进入睡眠模式 */
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_LED_SLEEP_MODE, RT_NULL, 1);
    
    rt_thread_mdelay(3000);

    /* 3s钟后进入活跃模式 */
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_LED_ACTIVE_MODE, RT_NULL, 1);

    val = HL_DRV_PWM_LED_BOX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_BOX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    val = HL_DRV_PWM_LED_BOX3;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    val = HL_DRV_PWM_LED_BOX4;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    val = HL_DRV_PWM_LED_BOX5;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    val = HL_DRV_PWM_LED_TX1;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_TX2;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));
    
    val = HL_DRV_PWM_LED_RX;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    rt_thread_mdelay(5000); 

    //val = HL_DRV_PWM_LED_BOX3;
    //hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MODE, &val, sizeof(val));

    breath_max_val = 0;
    /* 设置呼吸灯显示的最大亮度值 */
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BREATH_MAX_VAL, &breath_max_val, sizeof(breath_max_val));

    val = HL_DRV_PWM_LED_BOX3;
    hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_BRIGHT_MODE, &val, sizeof(val));

    // /* 5s钟后，关闭BOX4的灯显示 */
    // val = HL_DRV_PWM_LED_BOX4;
    // hl_drv_pwm_led_ctrl(HL_DRV_PWM_SET_CLOSE_MODE, &val, sizeof(val));
}

MSH_CMD_EXPORT(hl_drv_pwm_led_test, pwm led test);
/*
 * EOF
 */