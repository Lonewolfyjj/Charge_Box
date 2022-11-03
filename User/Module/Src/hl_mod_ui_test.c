/**
 * @file hl_mod_ui_test.c
 * @author yijiujun (jiujun.yi@hollyland-tech.com)
 * @brief 
 * @version V1.0
 * @date 2022-10-31
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
 * <tr><td>2022-10-31     <td>v1.0     <td>yijiujun     <td>内容
 * </table>
 * 
 */ 
/* Define to prevent recursive inclusion -------------------------------------*/
/* Includes ------------------------------------------------------------------*/

#include "hl_mod_ui.h"
#include "rtthread.h"

/* typedef -------------------------------------------------------------------*/
/* define --------------------------------------------------------------------*/
/* variables -----------------------------------------------------------------*/
/* Private function(only *.c)  -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

static void hl_mod_ui_test(int argc, char *argv[])
{
    uint8_t val;
    hl_mod_ui_charge_soc_st soc_st;

    if (argc <= 1) {
        rt_kprintf("format: cmd 1(page) 1(mod/charge_mod) 1(soc_type)\n");
        return;
    }

    hl_mod_ui_init(RT_NULL);

    if (atoi(argv[1]) == 1) {       //充电页面                          

        if (atoi(argv[2]) == 1) {
            soc_st.charge_state = HL_MOD_UI_OUT_CHARGE;     //未充电状态    (cmd  1   1  0/1/2/3)
        } else {
            soc_st.charge_state = HL_MOD_UI_CHARGING;       //充电状态      (cmd  1   2  0/1/2/3)
        }
        
        switch (atoi(argv[3])) {
            case 0:
                soc_st.soc_level = HL_MOD_UI_SOC_LEVEL_0;
                hl_mod_ui_ctrl(HL_MOD_UI_BATTERY_PAGE, &soc_st, sizeof(hl_mod_ui_charge_soc_st));
                break;
            case 1:
                soc_st.soc_level = HL_MOD_UI_SOC_LEVEL_1;
                hl_mod_ui_ctrl(HL_MOD_UI_BATTERY_PAGE, &soc_st, sizeof(hl_mod_ui_charge_soc_st));
                break;
            case 2:
                soc_st.soc_level = HL_MOD_UI_SOC_LEVEL_2;
                hl_mod_ui_ctrl(HL_MOD_UI_BATTERY_PAGE, &soc_st, sizeof(hl_mod_ui_charge_soc_st));
                break;
            case 3:
                soc_st.soc_level = HL_MOD_UI_SOC_LEVEL_3;
                hl_mod_ui_ctrl(HL_MOD_UI_BATTERY_PAGE, &soc_st, sizeof(hl_mod_ui_charge_soc_st));
                break;
            default:
                break;
        }
    } else if (atoi(argv[1]) == 2) {     //升级页面 (cmd  2   0/1/2)
        val = atoi(argv[2]) + 6;
        hl_mod_ui_ctrl(HL_MOD_UI_UPGRADE_PAGE, &val, sizeof(uint8_t));
    } else if (atoi(argv[1]) == 3) {     //错误页面 (cmd  3   0/1/2/3)
        val = atoi(argv[2]) + 9;
        hl_mod_ui_ctrl(HL_MOD_UI_ERROR_PAGE, &val, sizeof(uint8_t));
    } else {
        rt_kprintf("format: cmd 1(page) 1(mod/charge_mod) 1(soc_type)\n");
    }
    
    
}

MSH_CMD_EXPORT(hl_mod_ui_test, ui mod test);
/*
 * EOF
 */