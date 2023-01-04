/**
 * @file hl_util_config.c
 * @author lilin (lin.li@hollyland-tech.com)
 * @brief 
 * @version 1.0
 * @date 2022-10-28
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
 * <tr><td>2022-10-28     <td>v1.0     <td>lilin     <td>内容
 * </table>
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_util_config.h"

/* typedef -------------------------------------------------------------------*/

typedef struct _hl_util_config_handle_st
{
    bool                    init_flag;
    hl_util_config_write_in write;
    hl_util_config_read_out read;
    hl_util_config_st       config;
} hl_util_config_handle_st;

/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/

static hl_util_config_handle_st _config_hd = {
    .init_flag = false,
    .write     = RT_NULL,
    .read      = RT_NULL,
    .config    = { 0 },
};

/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int hl_util_config_init(hl_util_config_write_in write, hl_util_config_read_out read)
{
    int ret;

    if (write == RT_NULL || read == RT_NULL) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    if (_config_hd.init_flag == true) {
        return HL_UTIL_CONFIG_FUNC_OK;
    }

    _config_hd.read  = read;
    _config_hd.write = write;

    ret = _config_hd.read((char *)(&(_config_hd.config)), sizeof(_config_hd.config));
    if (ret == HL_UTIL_CONFIG_FUNC_ERR) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    _config_hd.init_flag = true;

    return HL_UTIL_CONFIG_FUNC_OK;
}

int hl_util_config_deinit(void)
{
    if (_config_hd.init_flag == false) {
        return HL_UTIL_CONFIG_FUNC_OK;
    }

    _config_hd.read  = RT_NULL;
    _config_hd.write = RT_NULL;

    _config_hd.init_flag = false;

    return HL_UTIL_CONFIG_FUNC_OK;
}

int hl_util_config_get(hl_util_config_st_p* config)
{
    if (_config_hd.init_flag == false) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    *config = &(_config_hd.config);

    return HL_UTIL_CONFIG_FUNC_OK;
}

int hl_util_config_save(void)
{
    int ret;

    if (_config_hd.init_flag == false) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    ret = _config_hd.write((char *)(&(_config_hd.config)), sizeof(_config_hd.config));
    if (ret == HL_UTIL_CONFIG_FUNC_ERR) {
        return HL_UTIL_CONFIG_FUNC_ERR;
    }

    return HL_UTIL_CONFIG_FUNC_OK;
}

/*
 * EOF
 */