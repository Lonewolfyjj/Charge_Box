/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-30     luzhanghao       the first version
 */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtthread.h"
#include "hl_util_fifo.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define HL_MAX(a, b)    ((a) > (b))?(a):(b)
#define HL_MIN(a, b)    ((a) < (b))?(a):(b)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @brief This function init fifo with the fifo pointer.
 *
 * @param p_fifo is the fifo pointer.
 *
 * @param buf_size is the fifo size want to set.
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
rt_err_t hl_util_fifo_init(hl_fifo_t *p_fifo, uint32_t buf_size)
{
    if ((p_fifo == RT_NULL) || (buf_size == 0)) {
        return RT_EINVAL;
    }

    p_fifo->fifo_size     = buf_size;
    p_fifo->read_pointer  = 0;
    p_fifo->write_pointer = 0;
    p_fifo->buffer        = rt_malloc(buf_size);

    if (p_fifo->buffer == RT_NULL) {
        return RT_ERROR;
    }

    return RT_EOK;
}

/**
 * @brief This function deinit fifo with the fifo pointer.
 *
 * @param p_fifo is the fifo pointer.
 */
void hl_util_fifo_deinit(hl_fifo_t *p_fifo)
{
    if ((p_fifo == RT_NULL) || (p_fifo->buffer == RT_NULL)) {
        return;
    }

    rt_free(p_fifo->buffer);

    p_fifo->fifo_size     = 0;
    p_fifo->read_pointer  = 0;
    p_fifo->write_pointer = 0;
    p_fifo->buffer        = RT_NULL;
}

/**
 * @brief This function get data size with the fifo pointer.
 *
 * @param p_fifo is the fifo pointer.
 *
 * @return the fifo data size
 */
uint32_t hl_util_fifo_data_size(hl_fifo_t *p_fifo)
{
    if ((p_fifo == RT_NULL) || (p_fifo->fifo_size == 0)) {
        return 0;
    }

    if (p_fifo->write_pointer >= p_fifo->read_pointer)  {
        return (p_fifo->write_pointer - p_fifo->read_pointer);
    } else {
        return (p_fifo->fifo_size - p_fifo->read_pointer + p_fifo->write_pointer);
    }
}

/**
 * @brief This function read n data with the fifo pointer & buff pointer & len.
 *
 * @param p_fifo is the fifo pointer.
 *
 * @param p_fifo is the fifo pointer, point to the buffer you want read to.
 *
 * @param len is the data size want to read.
 *
 * @return the actual number of data read
 */
uint32_t hl_util_fifo_read(hl_fifo_t *p_fifo, uint8_t *p_buf, uint32_t len)
{
   uint32_t size       = 0;
   uint32_t targe_size = 0;

   if ((p_fifo == RT_NULL) || (p_fifo->fifo_size == 0)) {
       return 0;
   }

   if (p_fifo->read_pointer == p_fifo->write_pointer) {
        return 0;
   } else {
       targe_size = HL_MIN(hl_util_fifo_data_size(p_fifo), len);
       size = p_fifo->fifo_size - p_fifo->read_pointer;
       if ((p_fifo->write_pointer < p_fifo->read_pointer) && (targe_size > size))  {
           memcpy(p_buf, (uint8_t *)&p_fifo->buffer[p_fifo->read_pointer], size);
           memcpy((uint8_t *)&p_buf[p_fifo->fifo_size - p_fifo->read_pointer], (uint8_t *)&p_fifo->buffer[0], targe_size - size);
       } else {
           memcpy(p_buf, (uint8_t *)&p_fifo->buffer[p_fifo->read_pointer], targe_size);
       }

       p_fifo->read_pointer = (p_fifo->read_pointer + targe_size) % p_fifo->fifo_size;

       return targe_size;
   }
}

/**
 * @brief This function write n data with the fifo pointer & buff pointer & len.
 *
 * @param p_fifo is the fifo pointer.
 *
 * @param p_fifo is the fifo pointer, point to the buffer want write to fifo.
 *
 * @param len is the data size want to write.
 *
 * @return the actual number of data write
 */
uint32_t hl_util_fifo_write(hl_fifo_t *p_fifo, uint8_t *p_buf, uint32_t len)
{
    uint32_t targe_size = 0;
    uint32_t size = 0;

    if ((p_fifo == RT_NULL) || (p_fifo->fifo_size == 0)) {
        return 0;
    }

    targe_size = HL_MIN((p_fifo->fifo_size - hl_util_fifo_data_size(p_fifo) - 1), len);
    size       = p_fifo->fifo_size - p_fifo->write_pointer;
    if ((p_fifo->write_pointer >= p_fifo->read_pointer) && (targe_size > size)) {
        memcpy(&p_fifo->buffer[p_fifo->write_pointer], p_buf, size);
        memcpy(&p_fifo->buffer[0], (p_buf + size), (targe_size - size));
    } else {
        memcpy(&p_fifo->buffer[p_fifo->write_pointer], p_buf, targe_size);
    }

    p_fifo->write_pointer += targe_size;
    p_fifo->write_pointer %= p_fifo->fifo_size;

    return targe_size;
}

/**
 * @brief This function clear fifo with the fifo pointer.
 *
 * @param p_fifo is the fifo pointer.
 *
 * @return the error code, RT_EOK on initialization successfully.
 */
rt_err_t hl_util_fifo_clear(hl_fifo_t *p_fifo)
{
    if (p_fifo == RT_NULL) {
        return RT_EINVAL;
    }

    p_fifo->read_pointer  = 0;
    p_fifo->write_pointer = 0;

    return RT_EOK;
}
