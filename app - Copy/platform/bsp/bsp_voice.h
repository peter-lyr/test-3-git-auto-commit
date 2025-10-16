#ifndef _BSP_VOICE_H
#define _BSP_VOICE_H

/*初始化参数结构体*/
typedef struct {
    u8   skip_flag;			/*开启后相同buf最大delay可多一倍*/
	u8   lp_filter_en;       /*滤波器使能*/
	u8   attenuation_set;    /*1-9 ->10percent-90percent*/
	u16  delay_set;          /*1-500ms*/
    u16  dry;
    u16  wet;
    u32  cutoffFreq_set;     /*截至频率选择*/
    u16  sample_rate;
	s16*  delay_lbuf;
} echo_init_t;

///echo算法结构体以及相关API声明
#define ENABLE_24BIT            0

#if ENABLE_24BIT
typedef s32			mic_pcm_t;
#else
typedef s16			mic_pcm_t;
#endif

typedef struct
{
	u16 damping_set;        /*高频阻尼，去除高频部分的齿音，擦类的刺耳声*/
	u16 decay_set;          /*衰减率*/
	u16 wet;
	u16 dry;
	u8 hpl;                 /*高通滤波器等级0表示关闭,1-10:50hz-500hz,步进50hz*/
}rvb_init_sb;

typedef struct {
	u32	  epstep;
	u32   hs_len;
	u32   input_len;
	u32	  output_len;
	u32   quality;
	u32   resamples_outlen;
	u32 pitch_rate;
	u32 formant_shift;
	s16* p_formant_buf_fix;
	s32* p_formant_buf2_fix;
	u32 gain_Q15;
	u8 robot_en;
}vc_mav_init_t;

typedef struct{
    u8 mp3_vc_en;
    u8 vmp3_vc_en;
    u8 esbc_vc_en;
    u8 lsbc_vc_en;
    union {
        struct {
            u8 wav_vc_layer1_en      :   1;
            u8 wav_vc_layer2_en      :   1;
            u8 wav_vc_layer3_en      :   1;
            u8 wav_vc_layer4_en      :   1;
            u8 wav_vc_layer5_en      :   1;
            u8 wav_vc_layer6_en      :   1;
        };
        u8 wav_vc_en;
    };
    void (*picth_up_do)(s16 *buf, u16 samples);
}voice_changer_t;


typedef struct {
	u32   input_len;
	u32	  output_len;
	u32   quality;
}wsola_init_t; //外部传参进来初始化使用

typedef struct{
    union {
        struct {
            u8 wav_wsola_layer1_en      :   1;
            u8 wav_wsola_layer2_en      :   1;
            u8 wav_wsola_layer3_en      :   1;
            u8 wav_wsola_layer4_en      :   1;
            u8 wav_wsola_layer5_en      :   1;
            u8 wav_wsola_layer6_en      :   1;
        };
        u8 wav_wsola_en;
    };
    union {
        struct {
			u8 layer1_speed;
			u8 layer2_speed;
			u8 layer3_speed;
			u8 layer4_speed;
			u8 layer5_speed;
			u8 layer6_speed;
        };
        u8 layer_speed[6];
    };
    void (*wav_wsolo_do)(s16 *buf, u16 samples, msc_layer_e wav_layer);
}wav_wsola_t;

typedef struct {

	//s32 noise_db2;
	//s32 noise_db3;
	s16 denoiseBound;
	s32 overdrive;
	u8  smooth_en;
	s64 enr_thres;
	s16 smooth_v;
	u8  enr_mean_max_en;
	s16 music_lev;
	s32 enr_nr_thr;
	s16 low_fre_range;
	s16 prior_opt_idx;
	s32 in_attack;
	s32 in_release;
	s32 fs;
	//u16 pitch_filter_range;
} dnr_fre_cb_t;

void bsp_echo_init(void);
void echo_process_do(mic_pcm_t *data_buf, u16 samples);

void bsp_reverb_init(void);
void reverb_process_do(s16 *buf, u16 samples);

void voice_changer_init(void);
void picth_up_process_do(s16 *buf, u16 samples);

void bsp_wsola_init(void);
void wsola_process_do(s16 *buf, u16 samples, msc_layer_e wav_layer);

void bsp_dnr_fre_init(void);
void bsp_dnr_fre_disable(void);
void bsp_dnr_fre_enable(void);

#endif // _BSP_ECHO_H
