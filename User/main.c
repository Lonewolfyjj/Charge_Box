/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <rtthread.h>

#include "hl_app_main.h"

#include "n32l40x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  functions ---------------------------------------------------------*/

bool hl_iwdog_feed_flag = true;

/* Exported functions --------------------------------------------------------*/
int main(void)
{
    /* Output a message on Hyperterminal using printf function */
    rt_kprintf("hollyland A6902_box start\n");

    hl_app_main_init();

    hl_app_main_start();

    // 该独立看门狗是用来进行系统健壮性维护的，如果遭遇静电等打击使得程序失效，则会复位进入低功耗，软件正常进入低功耗也是靠这个位置停止喂狗进入的
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32);
    IWDG_CntReload(40000 / 32);
    IWDG_ReloadKey();
    IWDG_Enable();

    while (1) {
        if (hl_iwdog_feed_flag == true) {
            IWDG_ReloadKey();
        }

        rt_thread_delay(100);
    }

    return 0;
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param file pointer to the source file name
 * @param line assert_param error line source number
 */
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    rt_kprintf("Wrong parameters value: file %s on line %d\r\n", file, line);
}

#endif
