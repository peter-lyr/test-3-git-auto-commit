#ifndef __BSP_SARADC_H
#define __BSP_SARADC_H

typedef struct {
    u16 vrtc_val;
    u16 vbg;
    u16 vbat2;
    u16 vbat_val;
    u16 vbat_bak;
#if TS_MODE_EN
    u16 vts;
    s16 ntc_temp;
#endif
    u16 mic_volt;
    u32 vbat_total;
} adc_cb_t;
extern adc_cb_t adc_cb;

void bsp_saradc_init(void);
bool bsp_saradc_process(u32 auto_en);
uint16_t bsp_saradc_exit(void);

#endif // __BSP_SARADC_H

