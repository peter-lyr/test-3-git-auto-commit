#ifndef _BSP_VBAT_H
#define _BSP_VBAT_H

u16 bsp_vbat_get_voltage(u32 rst_flag);
void bsp_vbat_voltage_init(void);
int bsp_vbat_get_lpwr_status(void);         //0: 正常电压, 1:低电提醒状态, 2:低电关机状态
void bsp_vbat_lpwr_process(void);


#endif // _BSP_VBAT_H
