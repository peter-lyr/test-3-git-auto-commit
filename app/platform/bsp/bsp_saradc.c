#include "include.h"

adc_cb_t adc_cb AT(.buf.key.cb);

AT(.com_text.saradc.process)
bool bsp_saradc_process(u32 auto_en)
{
    if (!saradc_is_finish()) {
        return false;
    }
    if (saradc_adc15_is_vrtc()) {
        adc_cb.vrtc_val = saradc_get_value10(ADCCH_VRTC);
    }
    if (saradc_adc15_is_bg()) {
        adc_cb.vbg = saradc_get_value10(ADCCH_BGOP);
    }
    if (saradc_adc15_is_mic()) {
        adc_cb.mic_volt = saradc_get_value10(ADCCH_MIC);
    }
#if TS_MODE_EN
    if (saradc_adc15_is_ts()) {
        adc_cb.vts = saradc_get_value10(ADCCH_VTS);
    }
#endif
    if (auto_en) {
        saradc_adc15_analog_next();
        saradc_kick_start(0);                                       //启动下一次ADC转换
    }
    return true;
}

void bsp_saradc_init(void)
{
    memset(&adc_cb, 0, sizeof(adc_cb));
    saradc_init();

    //始化时采一次BG电压
    saradc_adc15_analog_select(ADCCH15_ANA_BG);
    delay_us(600);
    saradc_kick_start(0);
    while(!bsp_saradc_process(0));

//    saradc_adc15_ana_set_channel(ADCCH15_ANA_VRTC);
#if TS_MODE_EN
    saradc_adc15_ana_set_channel(ADCCH15_ANA_TS);
#endif

#if VBAT_DETECT_EN
    saradc_adc15_ana_set_channel(ADCCH15_ANA_BG);
    bsp_vbat_voltage_init();
#endif
}

uint16_t bsp_saradc_exit(void)
{
    return saradc_exit();
}

u32 mic2sar_get_voltage(void)
{
    return (u32)adc_cb.mic_volt * VBG_VOLTAGE / adc_cb.vbg;
}


u16 saradc_get_bg_volt_val(void)
{
    return adc_cb.vbg;
}
