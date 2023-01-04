/**
 * @file hl_hal_boot_jump.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-21
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
 * <tr><td>2022-10-21     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_hal_boot_jump.h"
#include "stddef.h"
#include "n32l40x.h"

/* typedef -------------------------------------------------------------------*/

typedef void (*pFunction)(void);

/* define --------------------------------------------------------------------*/

#define N32_RAM_SIZE           ((uint32_t)0x6000) /* SRAM Size 32KB */
#define RAM_VECTOR_WORD_SIZE   ((uint32_t)54)
#define BOOT_MARK1_ADDR        ((uint32_t)0x1FFFF248) /* NVR Version */
#define BOOT_MARK2_ADDR        ((uint32_t)0x1FFFF298) /* BOOT Code */
#define BOOT_MARK3_ADDR        ((uint32_t)0x1FFFF2D0) /* BOOT NVIC */

/* variables -----------------------------------------------------------------*/

static pFunction JumpBoot = NULL;

/* Private function(only *.c)  -----------------------------------------------*/

static void _hl_sys_setsysclock_HSI_PLL(void)
{
    /* It is necessary to initialize the RCC peripheral to the reset */
    RCC_DeInit();
    
    /* Enable HSI */
    RCC_ConfigHsi(RCC_HSI_ENABLE);
    /* Wait till HSI is ready */
    if (RCC_WaitHsiStable() != SUCCESS)
    {
        /* If HSI fails to start-up, the application will have wrong clock
           configuration. User can add here some code to deal with this
           error */
        /* Go to infinite loop */
        while (1);
    }

    /* Enable ICACHE and Prefetch Buffer */
    FLASH_SetLatency(FLASH_LATENCY_2);
    FLASH_PrefetchBufSet(FLASH_PrefetchBuf_EN);
    FLASH_iCacheCmd(FLASH_iCache_EN);
    /* AHB prescaler factor set to HCLK = SYSCLK = 96M */
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);
    /* APB2 prescaler factor set to 1,PCLK2 = HCLK/2 = 48M */
    RCC_ConfigPclk2(RCC_HCLK_DIV2);
    /* APB1 prescaler factor set to 1,PCLK1 = HCLK/4 = 24M */
    RCC_ConfigPclk1(RCC_HCLK_DIV4);
    
    /* Config PLL */
    RCC_ConfigPll(RCC_PLL_HSI_PRE_DIV2, RCC_PLL_MUL_12, RCC_PLLDIVCLK_DISABLE);
    /* Enable PLL */
    RCC_EnablePll(ENABLE);
    while (RCC_GetFlagStatus(RCC_CTRL_FLAG_PLLRDF) == RESET);

    /* Switch PLL clock to SYSCLK. */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL);
}

/* Exported functions --------------------------------------------------------*/
/**
 * 
 * @brief 进入boot模式函数，必须在系统的最开始调用，之前不能有任何的初始化时钟操作，除了SystemInit函数。 
 * @date 2022-10-21
 * @author lilin (lin.li@hollyland-tech.com)
 * 
 * @details 
 * @note 
 * @par 修改日志:
 * <table>
 * <tr><th>Date             <th>Author         <th>Description
 * <tr><td>2022-10-21      <td>lilin     <td>新建
 * </table>
 */
void hl_hal_jump_to_boot()
{
//   rt_hw_interrupt_enable(irq_level);
//   hl_sys_rt_thread_exit();
  
  uint32_t i,*pVec,*pMark;
  uint32_t BootAddr, SPAddr, VacAddr;
  /* Disable all interrupt */
  __disable_irq();
                    
  /* Config IWDG */
  IWDG_ReloadKey();
  IWDG_WriteConfig(IWDG_WRITE_ENABLE);
  IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);

  /* Config system clock as 96M with HSI and PLL */
  _hl_sys_setsysclock_HSI_PLL();

  /* Reset peripheral used by boot */
  USART_DeInit(USART1);
  RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_USB, ENABLE);
  RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_USB, DISABLE);

  VacAddr = 0x20000000 + N32_RAM_SIZE - 0x100;
  pVec = (uint32_t *)VacAddr;
  for(i = 0; i < RAM_VECTOR_WORD_SIZE; i++)
  {
      pVec[i] = 0;
  }

  pMark = (uint32_t *)BOOT_MARK1_ADDR;
  /*BOOT V1.0 and NVR V1.2 */
  if(*pMark == 0xFFED0012)
  {
      SPAddr = 0x20001090;
      
      BootAddr                = 0x1FFF00D9;
      pVec[SysTick_IRQn + 16] = 0x1FFF0907;
      pVec[USB_LP_IRQn + 16]  = 0x1FFF093F;
      pVec[USART1_IRQn + 16]  = 0x1FFF091D;
  }
  else
  {
      pMark = (uint32_t *)BOOT_MARK2_ADDR;
      /*BOOT V1.1 and NVR V1.4 */
      if(*pMark == 0xFFFFFFFF)
      {
          SPAddr = 0x20001090;
          
          BootAddr                = 0x1FFF00D9;
          pVec[SysTick_IRQn + 16] = 0x1FFF0923;
          pVec[USB_LP_IRQn + 16]  = 0x1FFF097D;
          pVec[USART1_IRQn + 16]  = 0x1FFF095B;
      }
      else
      {
          pMark = (uint32_t *)BOOT_MARK3_ADDR;
          /*BOOT V1.2 and NVR V1.5 */
          if(*pMark == 0xFFFFFFFF)
          {
              SPAddr = (*(uint32_t *)BOOT_MARK2_ADDR);
              
              BootAddr                = 0x1FFF00D9;
              pVec[SysTick_IRQn + 16] = 0x1FFF210D;
              pVec[USB_LP_IRQn + 16]  = 0x1FFF23F1;
              pVec[USART1_IRQn + 16]  = 0x1FFF2189;
          }
          else
          {
              /*BOOT V1.2 and NVR V1.6 */
              SPAddr = (*(uint32_t *)BOOT_MARK2_ADDR);
              
              BootAddr                = pMark[0];
              pVec[SysTick_IRQn + 16] = pMark[1];
              pVec[USART1_IRQn + 16]  = pMark[2];
              pVec[USB_LP_IRQn + 16]  = pMark[3];
          }
      }
  }

  /* Enable interrupt */
  __enable_irq();
  /* Set Jumpboot Addr */
  JumpBoot = (pFunction)BootAddr;
  /* Initialize user application's Stack Pointer */
  __set_MSP(SPAddr);
  /* Initialize NVIC Table */
  SCB->VTOR = VacAddr;
  /* Start Jump to BOOT */
  JumpBoot();
}

/*
 * EOF
 */