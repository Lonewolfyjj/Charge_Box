/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <rtthread.h>

#include "hl_app_main.h"
#include "cm_backtrace.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int main(void)
{
    cm_backtrace_init("A6902_Box", "V1.0.0", "V1.0.0");

    /* Output a message on Hyperterminal using printf function */
    rt_kprintf("hollyland A6902_box start\n");

    hl_app_main_init();

    hl_app_main_start();

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
