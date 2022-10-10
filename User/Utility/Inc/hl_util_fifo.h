/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-30     luzhanghao       the first version
 */
#ifndef APPLICATIONS_UTILITY_INC_HL_UTIL_FIFO_H_
#define APPLICATIONS_UTILITY_INC_HL_UTIL_FIFO_H_
/* Includes ------------------------------------------------------------------*/
#include "rtdef.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef struct _hl_fifo_t
{
    uint32_t fifo_size;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint8_t  *buffer;
}hl_fifo_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// @brief This function init fifo with the fifo pointer.
rt_err_t hl_util_fifo_init(hl_fifo_t *p_fifo, uint32_t buf_size);

// @brief This function deinit fifo with the fifo pointer.
void hl_util_fifo_deinit(hl_fifo_t *p_fifo);

// @brief This function get data size with the fifo pointer.
uint32_t hl_util_fifo_data_size(hl_fifo_t *p_fifo);

// @brief This function read n data with the fifo pointer & buff pointer & len.
uint32_t hl_util_fifo_read(hl_fifo_t *p_fifo, uint8_t *p_buf, uint32_t len);

// @brief This function write n data with the fifo pointer & buff pointer & len.
uint32_t hl_util_fifo_write(hl_fifo_t *p_fifo, uint8_t *p_buf, uint32_t len);

// @brief This function clear fifo with the fifo pointer.
rt_err_t hl_util_fifo_clear(hl_fifo_t *p_fifo);

#endif /* APPLICATIONS_UTILITY_INC_HL_UTIL_FIFO_H_ */
