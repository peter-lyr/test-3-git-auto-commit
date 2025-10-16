#include "include.h"

void echo_init(echo_init_t *p);
void echo_process(mic_pcm_t *ldata);
void reverb_process(s16 *sample, u8 idx);
void reverb_buf_init(rvb_init_sb *p, u8 idx);
void fft_user_init(void);
void voice_change_init(vc_mav_init_t *p, u8 idx);
void voice_change_process(s16 *data, u32 samples, u8 idx); //samples必须等同EPSTEP
void wsola_voice_init(wsola_init_t *p, u8 idx, msc_layer_e wav_layer);
void wsola_voice_process(s16 *data, u32 samples, u8 idx, msc_layer_e wav_layer);
void dnr_fre_init(dnr_fre_cb_t *dnr_fre_cb);

#if SPK_MIC_ECHO_EN
#define M_PI		            3.14159265f
#define SAMPLE_PERIOD		    0.0000625f
#define MAX_DELAY_LENGTH        4000

echo_init_t echo_init_cfg;
s16 delay_lbuf[MAX_DELAY_LENGTH] AT(.echo_buf.echo);

#define QCONST32(x,bits) ((s32)(.5+(x)*(((s32)1)<<(bits))))

s32 qconst32(void)
{
    return QCONST32(SAMPLE_PERIOD*M_PI, 15);
}

AT(.text.echo_proc)
void bsp_echo_init(void)
{
    my_printf("bsp_echo_init\n");
    //音效参数初始化设置
    echo_init_cfg.skip_flag         = 1;
    echo_init_cfg.lp_filter_en      = 1;
    echo_init_cfg.attenuation_set   = SPK_MIC_ECHO_LEVEL;       //ECHO_LEVEL;
    echo_init_cfg.delay_set         = SPK_MIC_ECHO_DELAY;       //echo_cfg.delay_level * echo_cfg.delay_len_step;
    echo_init_cfg.dry               = 32767;                    //ECHO_DRY_USER;
    echo_init_cfg.wet               = 20000;                    //ECHO_WET_USER;
    echo_init_cfg.cutoffFreq_set    = 5000;
    echo_init_cfg.sample_rate       = 16000;                    //16k
    memset(delay_lbuf, 0, sizeof(delay_lbuf));
    echo_init_cfg.delay_lbuf        = delay_lbuf;
    echo_init(&echo_init_cfg);
}

AT(.text.echo_proc)
void echo_process_do(mic_pcm_t *data_buf, u16 samples)
{
    int i;
    for (i = 0; i < samples; i++) {
        echo_process(&data_buf[i]);
    }
}

#endif

#if SPK_MIC_REVERB_EN

rvb_init_sb rvb_t;

AT(.text.reverb_proc)
void bsp_reverb_init(void)
{
    my_printf("bsp_reverb_init\n");
    u8 idx;

	rvb_t.damping_set = 10384;//阻尼系数，越大吸收的高频越多
	rvb_t.decay_set = 93;//衰减率
	rvb_t.dry = 32767;//干度
	rvb_t.wet = 16363;//湿度
	rvb_t.hpl = 5;/*高通滤波器等级0表示关闭,1-10:50hz-500hz,步进50hz*/

	idx = 0; //0或1，左声道或右声道

	//reverb_buf_init(&rvb_t, 0);
    reverb_buf_init(&rvb_t, idx);
}

AT(.text.reverb_proc)
void reverb_process_do(s16 *buf, u16 samples)
{
    for(int i=0; i<samples; i++){
        reverb_process(&buf[i], 0);
    }
}

#endif

#if SPK_MIC_VOICE_CHANGER_EN || WARNING_VOICE_CHANGER_PLAY_EN
extern voice_changer_t* p_voice_changer_ctl;

voice_changer_t voice_changer_ctl;
static vc_mav_init_t mav_init;

#if VOICE_CHANGER_FS_EN
    #define FORMANT_WINLEN        256
    s16 formant_buf_fix[FORMANT_WINLEN] AT(.vc_buf_fft.buf);
    s32 formant_buf2_fix[FORMANT_WINLEN] AT(.vc_buf_fft.buf);
    u8 robot_vc_en = VOICE_CHANGER_ROBOT_EN;
#endif

enum{
    MALE_VOICE_TYPE,
    CHILD_VOICE_TYPE,
    FEMALE_VOICE_TYPE,
    SENIOR_VOICE_TYPE,
};

AT(.rodata.magic)
const u8 magic_table[4][2] = {
    {100,  3},            //男声
    {170,  3},            //儿童声
    {200,  1},            //女声
    {60,   1},            //长者声
    //{100,  100},        //原声
};

WEAK
void bsp_voice_changer_init(u32 pitchup, u32 quality)
{
    my_printf("pitchup:%d, quality:%d\n", pitchup, quality);
    u8 idx = 0;
    memset(&mav_init, 0, sizeof(mav_init_t));
    mav_init.pitch_rate             = pitchup * (1 << 15) / 100;;		//变调 建议范围均在1.0-3.1 Q15
    mav_init.quality	            = quality;							//1-8变速质量，越大效果越差，但速度更快
    mav_init.gain_Q15	            = 100 * (1 << 15)/ 100;;			//输出增益

    #if VOICE_CHANGER_FS_EN
    mav_init.formant_shift          = 120 * (1 << 20)/100;				//共振峰修正系数//建议0.3-1.5 Q20
    mav_init.robot_en               = robot_vc_en;						//机器音
    mav_init.p_formant_buf_fix	    = formant_buf_fix;				
    mav_init.p_formant_buf2_fix	    = formant_buf2_fix;				
    fft_user_init();
    if(sys_clk_get() < SYS_96M){
        sys_clk_set(SYS_96M);
    }
    #endif
    voice_change_init(&mav_init, idx);//算法初始化
}

void bsp_voice_changer_switch(u8 voice_select)
{
    u32 pitch_rate, quality;

    pitch_rate = magic_table[voice_select][0];
    quality = magic_table[voice_select][1];
    bsp_voice_changer_init(pitch_rate, quality);
}

void picth_up_process_do(s16 *buf, u16 samples)
{
    for(int i=0; i<(samples/64) ;i++){
        voice_change_process(&buf[i*64], 64, 0);
    }
}

//控制底层提示音播放是否变音
void voice_changer_play_ctl_init()
{
    p_voice_changer_ctl = &voice_changer_ctl;
    voice_changer_t *p = &voice_changer_ctl;
    p->mp3_vc_en  = 0;
    p->vmp3_vc_en = 0;
    p->esbc_vc_en = 0;
    p->lsbc_vc_en = 1;
    p->wav_vc_layer1_en = 1;
    p->wav_vc_layer2_en = 0;
    p->wav_vc_layer3_en = 0;
    p->wav_vc_layer4_en = 0;
    p->wav_vc_layer5_en = 0;
    p->wav_vc_layer6_en = 0;
    p->picth_up_do = picth_up_process_do;
}

WEAK
void voice_changer_init(void)
{
    bsp_voice_changer_switch(FEMALE_VOICE_TYPE);
#if WARNING_VOICE_CHANGER_PLAY_EN
    voice_changer_play_ctl_init();
#endif
}
#endif

#if SPK_MIC_WSOLA_REC_EN || WARNING_WSOLA_WAV_PLAY_EN
extern wav_wsola_t *p_wav_wsola_ctr;

#define HS_LEN    64
u8 wsola_speed = SPK_MIC_WSOLA_INPUT_LEN;
static wsola_init_t wsola_init;
wav_wsola_t wav_wsola_ctr;

//控制底层wav提示音播放是否变速
void wav_wsola_play_ctl_init()
{
    p_wav_wsola_ctr = &wav_wsola_ctr;
    wav_wsola_t *p = &wav_wsola_ctr;
    p->wav_wsola_layer1_en = 1;
    p->wav_wsola_layer2_en = 0;
    p->wav_wsola_layer3_en = 0;
    p->wav_wsola_layer4_en = 0;
    p->wav_wsola_layer5_en = 0;
    p->wav_wsola_layer6_en = 0;
    p->layer1_speed = SPK_MIC_WSOLA_INPUT_LEN;
    p->layer2_speed = 80;
    p->layer3_speed = SPK_MIC_WSOLA_INPUT_LEN;
    p->layer4_speed = SPK_MIC_WSOLA_INPUT_LEN;
    p->layer5_speed = SPK_MIC_WSOLA_INPUT_LEN;
    p->layer6_speed = SPK_MIC_WSOLA_INPUT_LEN;

    p->wav_wsolo_do = wsola_process_do;
}

WEAK
void bsp_wsola_init(void)
{
    wav_wsola_t *p = &wav_wsola_ctr;

    my_printf("bsp_wsola_init\n");
    u8 idx = 0;
    memset(&wsola_init, 0, sizeof(wsola_init_t));
    wav_wsola_play_ctl_init();

    for(int i = 1; i <= 6; i++){
        wsola_init.input_len = p->layer_speed[i-1];             //变速倍数：input_len / HS_LEN,  注意：变速录音时样点要对齐bsp_audio的样点数
        wsola_init.quality = 3;                                 //1-8变速质量，越大效果越差，但速度更快
        wsola_init.output_len = HS_LEN;
        wsola_voice_init(&wsola_init, idx, i);
        wsola_voice_init(&wsola_init, 1, i);
    }
}

WEAK
void wsola_process_do(s16 *buf, u16 samples, msc_layer_e wav_layer)
{
    wsola_voice_process(buf, samples, wav_layer%2, wav_layer);     //暂时只支持同时两层变不同速，例如wav_1层变1.5倍，wav_2层变1.8倍
}
#endif

#if SPK_MIC_DNR_FRE_EN
dnr_fre_cb_t dnr_fre_cb;

AT(.text.dnr_fre_small_init) WEAK
void bsp_dnr_fre_init(void)
{
    my_printf("bsp_dnr_fre_init\n");
    memset(&dnr_fre_cb, 0, sizeof(dnr_fre_cb_t));
	dnr_fre_cb.overdrive			    = 32768;
	dnr_fre_cb.smooth_en			    = 1;
	dnr_fre_cb.enr_thres				= 0;
	dnr_fre_cb.prior_opt_idx			= 10;
	dnr_fre_cb.low_fre_range			= 0;
	dnr_fre_cb.smooth_v				    = 31129; // 0.9f
	dnr_fre_cb.music_lev				= 16;

	dnr_fre_cb.denoiseBound		        = SPK_MIC_DNR_FRE_LEVEL;  //降噪量：20*log10(denoiseBound/32768) -> 16384就相当于噪声降6db
	dnr_fre_cb.enr_nr_thr				= -40;//dB  门限：低于此门限为噪声
	dnr_fre_cb.in_attack				= 32768;//Q15 ms 检测的att时间，peak， 32768=1ms
	dnr_fre_cb.in_release				= 3276800;//Q15 ms 检测的release时间，peak， 32768=1ms
	dnr_fre_cb.fs						= 16000;
    fft_user_init();
    if(sys_clk_get() < SYS_128M){
        sys_clk_set(SYS_128M);
    }
	dnr_fre_init(&dnr_fre_cb);
}

void bsp_dnr_fre_disable(void)
{
    f_spk.mic_dnr_fre_en = 0;
}

void bsp_dnr_fre_enable(void)
{
    f_spk.mic_dnr_fre_en = 1;
}
#endif

