/**
 * @file hl_hal_lowpower.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-24
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
 * <tr><td>2022-10-24     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_lowpower.h"
#include "n32l40x.h"
#include "hl_hal_gpio.h"

/* typedef -------------------------------------------------------------------*/

enum
{
    SYSCLK_PLLSRC_HSI,
    SYSCLK_PLLSRC_HSIDIV2,
    SYSCLK_PLLSRC_HSI_PLLDIV2,
    SYSCLK_PLLSRC_HSIDIV2_PLLDIV2,
    SYSCLK_PLLSRC_HSE,
    SYSCLK_PLLSRC_HSEDIV2,
    SYSCLK_PLLSRC_HSE_PLLDIV2,
    SYSCLK_PLLSRC_HSEDIV2_PLLDIV2,
};

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

extern ErrorStatus HSEStartUpStatus;
ErrorStatus        HSIStartUpStatus;

/* Private function(only *.c)  -----------------------------------------------*/
static void _soc_gpio_irq_handle(void* args)
{
}

static void _gpio_wakeup_config(void)
{
//    GPIO_InitType GPIO_InitStructure;
//    EXTI_InitType EXTI_InitStructure;
//    NVIC_InitType NVIC_InitStructure;
//
//    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
//    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
//
//    GPIO_InitStruct(&GPIO_InitStructure);
//    GPIO_InitStructure.Pin       = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
//    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Up;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
//    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
//
//    GPIO_InitStruct(&GPIO_InitStructure);
//    GPIO_InitStructure.Pin       = GPIO_PIN_7;
//    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Up;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
//    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
//
//    GPIO_ConfigEXTILine(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE3);
//    GPIO_ConfigEXTILine(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE4);
//    GPIO_ConfigEXTILine(GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE5);
//    GPIO_ConfigEXTILine(GPIOA_PORT_SOURCE, GPIO_PIN_SOURCE7);
//
//    /*Configure key EXTI line*/
//    EXTI_InitStructure.EXTI_Line    = EXTI_LINE3 | EXTI_LINE4 | EXTI_LINE5 | EXTI_LINE7;
//    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_InitPeripheral(&EXTI_InitStructure);
//
//    /*Set key input interrupt priority*/
//    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI3_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
//    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
//    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    hl_hal_gpio_init(GPIO_HALL_BOX);
    hl_hal_gpio_attach_irq(GPIO_HALL_BOX, PIN_IRQ_MODE_RISING_FALLING, _soc_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_BOX, PIN_IRQ_ENABLE);

    hl_hal_gpio_init(GPIO_HALL_TX1);
    hl_hal_gpio_attach_irq(GPIO_HALL_TX1, PIN_IRQ_MODE_RISING_FALLING, _soc_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_TX1, PIN_IRQ_ENABLE);

    hl_hal_gpio_init(GPIO_HALL_TX2);
    hl_hal_gpio_attach_irq(GPIO_HALL_TX2, PIN_IRQ_MODE_RISING_FALLING, _soc_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_TX2, PIN_IRQ_ENABLE);

    hl_hal_gpio_init(GPIO_HALL_RX);
    hl_hal_gpio_attach_irq(GPIO_HALL_RX, PIN_IRQ_MODE_RISING_FALLING, _soc_gpio_irq_handle, RT_NULL);
    hl_hal_gpio_irq_enable(GPIO_HALL_RX, PIN_IRQ_ENABLE);

}

static void _gpio_lowpower_config(void)
{
    // GPIO_InitType GPIO_InitStructure;

    // RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    // GPIO_InitStruct(&GPIO_InitStructure);
    // GPIO_InitStructure.Pin       = GPIO_PIN_14 | GPIO_PIN_13;
    // GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Down;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
    // GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief  Selects PLL clock as System clock source and configure HCLK, PCLK2
 *         and PCLK1 prescalers.
 */
void SetSysClockToPLL(uint32_t freq, uint32_t RCC_SYSCLKSource)
{
    uint32_t pllsrcclk;
    uint32_t pllsrc;
    uint32_t pllmul;
    uint32_t plldiv = RCC_PLLDIVCLK_DISABLE;
    uint32_t latency;
    uint32_t pclk1div, pclk2div;
    uint32_t msi_ready_flag = RESET;
    uint8_t  src            = SYSCLK_PLLSRC_HSE;
    if (HSE_VALUE != 16000000) {
        /* HSE_VALUE == 8000000 is needed in this project! */
        while (1)
            ;
    }

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration
     * -----------------------------*/

    if ((src == SYSCLK_PLLSRC_HSI) || (src == SYSCLK_PLLSRC_HSIDIV2) || (src == SYSCLK_PLLSRC_HSI_PLLDIV2)
        || (src == SYSCLK_PLLSRC_HSIDIV2_PLLDIV2)) {
        /* Enable HSI */
        RCC_ConfigHsi(RCC_HSI_ENABLE);

        /* Wait till HSI is ready */
        HSIStartUpStatus = RCC_WaitHsiStable();

        if (HSIStartUpStatus != SUCCESS) {
            /* If HSI fails to start-up, the application will have wrong clock
               configuration. User can add here some code to deal with this
               error */

            /* Go to infinite loop */
            while (1)
                ;
        }

        if ((src == SYSCLK_PLLSRC_HSIDIV2) || (src == SYSCLK_PLLSRC_HSIDIV2_PLLDIV2)) {
            pllsrc    = RCC_PLL_HSI_PRE_DIV2;
            pllsrcclk = HSI_VALUE / 2;

            if (src == SYSCLK_PLLSRC_HSIDIV2_PLLDIV2) {
                plldiv    = RCC_PLLDIVCLK_ENABLE;
                pllsrcclk = HSI_VALUE / 4;
            }
        } else if ((src == SYSCLK_PLLSRC_HSI) || (src == SYSCLK_PLLSRC_HSI_PLLDIV2)) {
            pllsrc    = RCC_PLL_HSI_PRE_DIV1;
            pllsrcclk = HSI_VALUE;

            if (src == SYSCLK_PLLSRC_HSI_PLLDIV2) {
                plldiv    = RCC_PLLDIVCLK_ENABLE;
                pllsrcclk = HSI_VALUE / 2;
            }
        }

    } else if ((src == SYSCLK_PLLSRC_HSE) || (src == SYSCLK_PLLSRC_HSEDIV2) || (src == SYSCLK_PLLSRC_HSE_PLLDIV2)
               || (src == SYSCLK_PLLSRC_HSEDIV2_PLLDIV2)) {
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        HSEStartUpStatus = RCC_WaitHseStable();

        if (HSEStartUpStatus != SUCCESS) {
            /* If HSE fails to start-up, the application will have wrong clock
               configuration. User can add here some code to deal with this
               error */

            /* Go to infinite loop */
            while (1)
                ;
        }

        if ((src == SYSCLK_PLLSRC_HSEDIV2) || (src == SYSCLK_PLLSRC_HSEDIV2_PLLDIV2)) {
            pllsrc    = RCC_PLL_SRC_HSE_DIV2;
            pllsrcclk = HSE_VALUE / 2;

            if (src == SYSCLK_PLLSRC_HSEDIV2_PLLDIV2) {
                plldiv    = RCC_PLLDIVCLK_ENABLE;
                pllsrcclk = HSE_VALUE / 4;
            }
        } else if ((src == SYSCLK_PLLSRC_HSE) || (src == SYSCLK_PLLSRC_HSE_PLLDIV2)) {
            pllsrc    = RCC_PLL_SRC_HSE_DIV1;
            pllsrcclk = HSE_VALUE;

            if (src == SYSCLK_PLLSRC_HSE_PLLDIV2) {
                plldiv    = RCC_PLLDIVCLK_ENABLE;
                pllsrcclk = HSE_VALUE / 2;
            }
        }
    }

    latency = (freq / 32000000);

    if (freq > 54000000) {
        pclk1div = RCC_HCLK_DIV4;
        pclk2div = RCC_HCLK_DIV2;
    } else {
        if (freq > 27000000) {
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV1;
        } else {
            pclk1div = RCC_HCLK_DIV1;
            pclk2div = RCC_HCLK_DIV1;
        }
    }

    if (((freq % pllsrcclk) == 0) && ((freq / pllsrcclk) >= 2) && ((freq / pllsrcclk) <= 32)) {
        pllmul = (freq / pllsrcclk);
        if (pllmul <= 16) {
            pllmul = ((pllmul - 2) << 18);
        } else {
            pllmul = (((pllmul - 17) << 18) | (1 << 27));
        }
    } else {

        while (1)
            ;
    }

    /* Cheak if MSI is Ready */
    if (RESET == RCC_GetFlagStatus(RCC_CTRLSTS_FLAG_MSIRD)) {
        /* Enable MSI and Config Clock */
        RCC_ConfigMsi(RCC_MSI_ENABLE, RCC_MSI_RANGE_4M);
        /* Waits for MSI start-up */
        while (SUCCESS != RCC_WaitMsiStable())
            ;

        msi_ready_flag = SET;
    }

    /* Select MSI as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_MSI);

    FLASH_SetLatency(latency);

    /* HCLK = SYSCLK */
    RCC_ConfigHclk(RCC_SYSCLKSource);  //RCC_SYSCLK_DIV1

    /* PCLK2 = HCLK */
    RCC_ConfigPclk2(pclk2div);

    /* PCLK1 = HCLK */
    RCC_ConfigPclk1(pclk1div);

    /* Disable PLL */
    RCC_EnablePll(DISABLE);

    RCC_ConfigPll(pllsrc, pllmul, plldiv);

    /* Enable PLL */
    RCC_EnablePll(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_CTRL_FLAG_PLLRDF) == RESET)
        ;

    /* Select PLL as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSysclkSrc() != 0x0C)
        ;

    if (msi_ready_flag == SET) {
        /* MSI oscillator OFF */
        RCC_ConfigMsi(RCC_MSI_DISABLE, RCC_MSI_RANGE_4M);
    }
}

/* Exported functions --------------------------------------------------------*/

void hl_hal_lowpower_enter(void)
{
    int i = 100000;

    _gpio_wakeup_config();
    _gpio_lowpower_config();

    while (i--) {
    }

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);

    SetSysClockToPLL(64000000, RCC_SYSCLK_DIV1);

    PWR_EnterSTOP2Mode(PWR_STOPENTRY_WFI, PWR_CTRL3_RAM1RET);  //低功耗一直停在这里
    __NVIC_SystemReset();                                      //重启
}

/*
 * EOF
 */