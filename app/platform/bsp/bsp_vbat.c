#include "include.h"

#if VBAT_DETECT_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
u16 getcfg_vddio_val_ofs(void);
void audio_power_update(u8 ud_flag);

AT(.com_rodata.bat)
const char bat_str[] = "VBAT: %d.%03dV\n";

#if VDDIO_FOLLOW_VBAT_EN
u32 vio_fkick = 0;

void vddio_follow_vbat_process(void)
{
    vddio_voltage_update(adc_cb.vbat_val);
    if ((saradc_cb.channel & BIT(ADCCH_BGOP)) && (get_cur_vddio_vol() < adc_cb.vbat_val)) {
        if (tick_check_expire(vio_fkick,1000)) {
            vio_fkick = tick_get();
            saradc_cb.channel &= ~BIT(ADCCH_BGOP);
            PWRCON0 &= ~BIT(12);                   //BG output disable
        }
    }
}
#endif

u16 bsp_get_sys_vbat(void)
{
    return sys_cb.vbat;
}

AT(.com_text.port.vbat)
u16 bsp_vbat_get_voltage(u32 rst_flag)
{
    u8 update_flag = 0;
    u32 vbat;
    u32 vbat2 = saradc_get_value10(VBAT2_ADCCH);
    vbat = (u32)vbat2 * VBG_VOLTAGE * VBAT2_COEF / (adc_cb.vbg * 1000);

    if (rst_flag) {
        adc_cb.vbat_bak = 0;
        adc_cb.vbat_val = vbat;
        adc_cb.vbat_total = adc_cb.vbat_val << 5;
    }

    //不同方案可能采用不同 vbat 滤波算法, 在方案对应的plugin.c中处理
    plugin_vbat_filter(&vbat);

    //默认的取平均值算法.
    adc_cb.vbat_total = adc_cb.vbat_total - adc_cb.vbat_val + vbat;
    adc_cb.vbat_val = adc_cb.vbat_total >> 5;           //平均值
    if (adc_cb.vbat_val > adc_cb.vbat_bak) {
        vbat = adc_cb.vbat_val - adc_cb.vbat_bak;
    } else {
        vbat = adc_cb.vbat_bak - adc_cb.vbat_val;
    }
    if (vbat >= 30) {   //偏差大于一定值则更新
#if VDDIO_FOLLOW_VBAT_EN
        // adc_cb.channel |= BIT(ADCCH_BGOP);
        saradc_set_channel(BIT(ADCCH_BGOP));
        PWRCON0  |= BIT(12);
        vio_fkick = tick_get();
#endif
        adc_cb.vbat_bak = adc_cb.vbat_val;
        update_flag = (adc_cb.vbat_bak > 3000)? 0 : 1;
        audio_power_update(update_flag);
        // printf(bat_str, adc_cb.vbat_val/1000, adc_cb.vbat_val%1000);
    }

    //printf(bat_str, adc_cb.vbat_val/1000, adc_cb.vbat_val%1000);
    return adc_cb.vbat_bak;
}

void bsp_vbat_voltage_init(void)
{
    sys_cb.vbat = bsp_vbat_get_voltage(1);
#if VDDIO_FOLLOW_VBAT_EN && VBAT_DETECT_EN
    if (adc_cb.vbat_val < getcfg_vddio_val_ofs()) {
        return;
    }
    saradc_cb.channel &= ~BIT(ADCCH_BGOP);     //只进行一次BG ADC采集
    PWRCON0  &= ~BIT(12);                   //BG output disable
#endif

}

int bsp_vbat_get_lpwr_status(void)
{
    if (sys_cb.vbat <= LPWR_OFF_VBAT) {
        if (LPWR_OFF_VBAT) {
            if (!sys_cb.lpwr_cnt) {
                sys_cb.lpwr_cnt = 1;
            } else if (sys_cb.lpwr_cnt >= 10) {
                return 2;       //VBAT低电关机
            }
        }
        return 0;               //VBAT低电不关机
    } else {
        sys_cb.lpwr_cnt = 0;
        if (sys_cb.vbat < LPWR_WARNING_VBAT) {
            return 1;           //VBAT低电提醒状态
        } else {
        }
        return 0;
    }
    sys_cb.lpwr_cnt = 0;
    return 0;
}

void bsp_vbat_lpwr_process(void)
{
    int vbat_sta = bsp_vbat_get_lpwr_status();

    if (vbat_sta == 2) {                            //达到低电关机状态
        func_cb.sta = FUNC_PWROFF;                  //低电，进入关机或省电模式
        return;
    } else if (vbat_sta == 1) {                     //低电提醒状态
        //低电提示音播放
        sys_cb.vbat_nor_cnt = 0;
        if (sys_cb.lpwr_warning_cnt > LPWR_WARNING_PERIOD) {
            sys_cb.lpwr_warning_cnt = 0;
            if (sys_cb.lpwr_warning_times) {        //低电语音提示次数
                sys_cb.lowbat_flag = 1;
                plugin_lowbat_vol_reduce();         //低电降低音乐音量
                if (sys_cb.lpwr_warning_times != 0xff) {
                    sys_cb.lpwr_warning_times--;
                }
            }
        }
    } else {
        if ((sys_cb.lowbat_flag) && (sys_cb.vbat > 3800)) {
            sys_cb.vbat_nor_cnt++;
            if (sys_cb.vbat_nor_cnt > 40) {
                sys_cb.lowbat_flag = 0;
                sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
                plugin_lowbat_vol_recover();        //离开低电, 恢复音乐音量
            }
        }
    }
}

#endif  //VBAT_DETECT_EN
