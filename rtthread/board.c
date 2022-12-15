/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */

#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "n32l40x.h"
#include "hl_hal_console.h"

#define _SCB_BASE (0xE000E010UL)
#define _SYSTICK_CTRL (*(rt_uint32_t*)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD (*(rt_uint32_t*)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL (*(rt_uint32_t*)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB (*(rt_uint32_t*)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI (*(rt_uint8_t*)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

// Holds the system core clock, which is the system clock
// frequency supplied to the SysTick timer and the processor
// core clock.
extern uint32_t SystemCoreClock;

static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF) {
        return 1;
    }

    _SYSTICK_LOAD = ticks - 1;
    _SYSTICK_PRI  = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;

    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 4096
static uint32_t rt_heap[RT_HEAP_SIZE];  // heap default size: 4K(1024 * 4)
RT_WEAK void*   rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void* rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

#include "hl_hal_boot_jump.h"
#include "hl_hal_lowpower.h"
#include "hl_drv_flash.h"
#include "hl_util_config.h"

#define HL_CONFIG_START_ADDR 0

static int _hl_util_config_write(char* data, int len)
{
    int ret;

    ret = hl_drv_flash_write(HL_CONFIG_START_ADDR, data, len);
    if (ret == FLASH_RET_ERR) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    return HL_UTIL_CONFIG_FUNC_OK;
}

static int _hl_util_config_read(char* data, int len)
{
    int ret;

    ret = hl_drv_flash_read(HL_CONFIG_START_ADDR, data, len);
    if (ret == FLASH_RET_ERR) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    return HL_UTIL_CONFIG_FUNC_OK;
}

bool _flash_init_flag = false;

static void _hl_start_state_check(void)
{
    int ret;

    hl_util_config_st_p config;

    ret = hl_drv_flash_init();
    if (ret == FLASH_RET_ERR) {
        return;
    } else {
        _flash_init_flag = true;
    }

    hl_util_config_init(_hl_util_config_write, _hl_util_config_read);

    hl_util_config_get(&config);

    if (config->boot_jump_flag == 1) {
        config->boot_jump_flag = 0;
        hl_util_config_save();
        hl_drv_flash_deinit();

        hl_hal_jump_to_boot();
    }

    if (config->lowpower_flag == 1) {
        config->lowpower_flag = 0;
        hl_util_config_save();
        hl_drv_flash_deinit();

        hl_hal_lowpower_enter();
    }
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    _hl_start_state_check();

    /* System Clock Update */
    SystemCoreClockUpdate();

    /* System Tick Configuration */
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif

    /* Set the shell console output device */
#ifdef RT_USING_CONSOLE
    hl_hal_console_init();
#endif
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}
