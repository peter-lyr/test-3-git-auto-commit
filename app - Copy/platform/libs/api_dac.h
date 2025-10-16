#ifndef _API_DAC_H_
#define _API_DAC_H_

enum {
    SPR_48000,
    SPR_44100,
    SPR_38000,
    SPR_32000,
    SPR_24000,
    SPR_22050,
    SPR_16000,
    SPR_12000,
    SPR_11025,
    SPR_8000,
    SPR_6000,
    SPR_4000,
};
s16 mic_eq_proc(s16 input);
bool mic_set_eq_by_res(u32 *addr, u32 *len);

void dac_power_on(void);                                    //开启DAC
void dac_power_off(void);                                   //关闭DAC
void dac_restart(void);                                     //重启DAC模块
void dac_spr_set(uint spr);                                 //设置DAC src0通路采样率,spr:SPR_4000~SPR_48000
void dac_set_dvol(u16 vol);                                 //设置DAC数字音量(音量淡入),vol:0~0xffff
void dac_set_max_vol(u16 vol);                              //设置DAC数字音量(立即设置),vol:0~0xffff
void dac_msc_vol_by_src0(void);                             //系统音量改为调SRC0音量, DACVOL控制最大输出
void dac_aubuf_clr(void);                                   //清空DAC播放数据buff
void dac_put_zero(uint len);                                //往DAC推0数据

void dac_cb_init(u32 dac_cfg);                              //DAC功能配置初始化
void dac_digvol_fade(u32 flag, u32 step);                   //DAC数字淡入淡出处理
void dac_fade_out(void);                                    //DAC数字淡出
void dac_fade_in(void);                                     //DAC数字淡入
void dac_fade_wait(void);                                   //DAC淡入淡出等待

bool dac_sclk_is_enable(void);                              //检测DAC的时钟是否使能
void dac_aubuf_init(void *buf, u32 size, u32 samples);      //DAC缓存buff初始化
void dac_fade_process(void);                                //DAC淡入淡出处理
void dac_classd_adaptive_by_vbat(u16 vbat_val);             //DAC ClassD低电量过压处理

void dac_dnr_detect(void);                                  //DAC能量值检测
u16 dac_pcm_pow_get(void);                                  //获取DAC能量值

void dac_src1_init(void);                                   //DAC src1通路初始化
void dac_src1_spr_set(uint spr);                            //设置DAC src1通路采样率,spr:SPR_4000~SPR_48000

void dac_put_sample_16bit(s16 left, s16 right);             //DAC src0输入左右声音单个数据(16bit)
void dac_put_sample_24bit(s32 left, s32 right);             //DAC src0输入左右声音单个数据(24bit)

void aubuf0_dma_init(u32 isr_en);                                           //DAC src0 aubuf0初始化
void aubuf0_dma_kick(void *ptr, u32 samples, u32 nch, bool is_24bit);       //往aubuf0 DMA推音频数据
void aubuf0_dma_w4_done(void);                                              //等待aubuf0 DMA完成
void aubuf1_dma_init(u32 isr_en);                                           //DAC src1 aubuf1初始化
void aubuf1_dma_kick(void *ptr, u32 samples, u32 nch, bool is_24bit);       //往aubuf1 DMA推音频数据
void aubuf1_dma_w4_done(void);                                              //等待aubuf1 DMA完成

bool music_set_drc_by_res(u32 addr, u32 len);                               //DAC DRCc曲线设置
bool music_set_eq_by_res(u32 addr, u32 len);                                //DAC EQ曲线设置
void music_set_eq_by_num(u8 num);                                           //DAC EQ曲线选择
void music_eq_off(void);                                                    //关闭dac EQ
bool music_set_eq(u8 band_cnt, const u32 *eq_param);                        //DAC EQ 参数设置
void music_set_drc(u8 band_cnt, const u32 *drc_param);                      //DAC DRC 参数设置
void music_set_drc_by_online(u8 band_cnt, const u32 *drc_param);            //DAC DRC 在线调试设置

u16 dnr_buf_maxpow(void *ptr, u16 len);     //计算ptr指向的音频数据能量值并返回, ptr:指向音频数据, len:8bit位宽的数据长度

void dac_voutp_set(u8 io_sta);              //VOUTP引脚输出,io_sta: 0-输出低电平, 1-输出高电平(vbat电压)
void dac_voutn_set(u8 io_sta);              //VOUTN引脚输出,io_sta: 0-输出低电平, 1-输出高电平(vbat电压)

void dac_classd_analog_on(void);            //使能DAC classD模拟
void dac_classd_analog_off(void);           //关闭DAC classD模拟
void dac_dc_exp_on(void);                   //DAC 打开DC
void dac_dc_exp_off(void);                  //DAC 关闭DC
#endif
