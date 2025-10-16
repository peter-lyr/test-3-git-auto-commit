#include "include.h"

volatile int pwrkey_detect_flag;            //pwrkey 820K用于复用检测的标志。
uint8_t lvd_irq_en              = LVD_IRQ_EN;
uint8_t dac_drc_en              = DAC_DRC_EN;
uint8_t mic_soft_gain           = 0;
uint8_t dac_soft_gain           = 0;

void plugin_init(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif
    mic_soft_gain = MIC_SOFT_GAIN;
    dac_soft_gain = DAC_SOFT_GAIN;    
}

void plugin_var_init(void)
{

#if LINEIN_DETECT_EN
    LINEIN_DETECT_INIT();
#endif

#if LED_DISP_EN
	led_func_init();
#endif
}

AT(.com_text.port.key)
void plugin_saradc_sel_channel(u16 *adc_ch)
{
#if USER_KEY_KNOB_EN
    u8 mic_vol_val = (u8)(adc_cb.sfr[ADCCH_PE5] >> 2);
    u8 echo_level_val = (u8)(adc_cb.sfr[ADCCH_PE6] >> 2);

    static u8 echo_level = USER_KEY_KNOB_LEVEL, mic_vol = USER_KEY_KNOB_LEVEL;

    key_knob_process(echo_level_val, tbl_knob_level_16, &echo_level);
    if (echo_level != sys_cb.echo_level) {
        sys_cb.echo_level = echo_level;
        msg_enqueue(EVT_ECHO_LEVEL);
    }

    key_knob_process(mic_vol_val, tbl_knob_level_16, &mic_vol);
    if (mic_vol != sys_cb.mic_vol) {
        sys_cb.mic_vol = mic_vol;
        msg_enqueue(EVT_MIC_VOL);
    }
#endif
}

void plugin_saradc_init(u16 *adc_ch)
{

}

void plugin_lowbat_vol_reduce(void)
{
#if LOWPWR_REDUCE_VOL_EN

#endif // LOWPWR_REDUCE_VOL_EN
}

void plugin_lowbat_vol_recover(void)
{
#if LOWPWR_REDUCE_VOL_EN

#endif // LOWPWR_REDUCE_VOL_EN
}

void plugin_music_eq(void)
{
    sys_cb.eq_mode = 0;
    music_set_eq_by_num(sys_cb.eq_mode);
    music_set_drc_by_res(RES_BUF_EQ_DAC_DRC, RES_LEN_EQ_DAC_DRC);
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
#if  VBAT_FILTER_USE_PEAK
    //电源波动比较大的音箱方案, 取一定时间内的电量"最大值"或"次大值",更能真实反应电量.
    #define VBAT_MAX_TIME  (3000/5)   //电量峰值判断时间 3S
    static u16 cnt = 0;
	static u16 vbat_max_cnt = 0;
    static u32 vbat_max[2] = {0,0};
    static u32 vbat_ret = 0;
    u32 vbat_cur = *vbat;
    if (cnt++  < VBAT_MAX_TIME) {
        if (vbat_max[0] < vbat_cur) {
            vbat_max[1] = vbat_max[0];  //vbat_max[1] is less max (次大值)
            vbat_max[0] = vbat_cur;     //vbat_max[0] is max(最大值)
            vbat_max_cnt = 0;
        } else if (vbat_max[0] == vbat_cur) {
            vbat_max_cnt ++;
        }
    } else {
        if (vbat_max_cnt >= VBAT_MAX_TIME/5) {  //总次数的(1/5)都采到最大值,则返回最大值.
            vbat_ret = vbat_max[0];
        } else if (vbat_max[1] != 0) {   //最大值次数较少,则返回次大值(舍弃最大值)
            vbat_ret = vbat_max[1];
        }
        vbat_max[0] = 0;
        vbat_max[1] = 0;
        vbat_max_cnt = 0;
        cnt = 0;
    }
    //返回值
    if (vbat_ret != 0) {
        *vbat = vbat_ret;
    }
#endif
}

#if LVD_IRQ_EN

#define LVD_RESET_EN        1                           //0-关闭LVD复位, 1-打开LVD复位
#define LVD_INT_EN          0                           //0-关闭LVD中断, 1-打开LVD中断
#define LVD_RESET_SEL       xcfg_cb.reset_sel          	//LVD复位电压选择
#define LVD2_INT_EN         1                           //0-关闭LVD2中断, 1-打开LVD2中断
#define LVD2_RESET_SEL      xcfg_cb.reset_sel2          //LVD2中断电压选择

AT(.com_text.isr)
void lvd_isr(void)
{
    u32 lvdpnd = (LVDCON >> 7) & (LVDCON >> 6);
    if (lvdpnd) {                       //LVD pending
        CRSTPND = BIT(7);               //Clear LVD pending

        //此处放置关机前需要处理的代码,尽量简洁,不能处理过于复杂的
        GPIOAFEN &= ~BIT(8);            //test
        GPIOADE  |=  BIT(8);
        GPIOADIR &= ~BIT(8);
        GPIOASET = BIT(8);
        asm("nop");asm("nop");asm("nop");
        GPIOACLR = BIT(8);

        LVDCON = (LVDCON & ~0x07) | 2;  //关机前还原电压设置
    }

#if LVD2_INT_EN
    lvdpnd = (LVDCON2 >> 5) & (LVDCON2 >> 6);
    if (lvdpnd) {                       //LVD2 pending
        CRSTPND = BIT(6);               //Clear LVD2 pending
        /* 此处放置的代码尽量简洁 */
    }
#endif
}

//设置触发LVD中断电压
void lvd_level_set(void)
{
    LVDCON |=  BIT(3);                              //LVD enable
    LVDCON  = (LVDCON & ~0x07) | LVD_RESET_SEL;     //0-1.8V, 1-2V, 2-2.2V, 3-2.4V, 4-2.6V, 5-2.8V, 6-3V, 7-1.7V
#if LVD_RESET_EN
    LVDCON |=  BIT(5) | BIT(4);                     //LVD reset enable //LVD output enable
#else
    LVDCON &= ~BIT(5);                              //LVD reset disable
#endif
#if LVD_INT_EN
    LVDCON |=  BIT(6) | BIT(4);                     //LVD interrupt enable
#else
    LVDCON &= ~BIT(6);                              //LVD interrupt disable
#endif
#if LVD2_INT_EN
    LVDCON2  = (LVDCON2 & ~0x07) | LVD2_RESET_SEL;  //LVD2 select bit
    LVDCON2 |=  BIT(5) | BIT(4) | BIT(3);           //LVD2 interrupt enable
#endif
}
#else
AT(.com_text.isr)
void lvd_isr(void) {}
#endif
