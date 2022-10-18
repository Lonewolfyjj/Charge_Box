/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <rtthread.h>

#include "hl_hal_gpio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
int main(void)
{
    /* Output a message on Hyperterminal using printf function */
    rt_kprintf("hollyland A6902_box start\n");
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
