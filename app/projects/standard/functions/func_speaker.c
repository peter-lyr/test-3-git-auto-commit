#include "include.h"
#include "func.h"

#if FUNC_SPEAKER_EN
extern uint8_t dac_drc_en;
extern uint8_t mic_soft_gain;
extern const int dac_digital_gain_tbl[64];

void kws2nd_pcm_callback(u8 *ptr, u16 len);
s16 drc_v3_calc(s32 sample);
int s_clip16(int x);
void angle_change_process(s16 *data);
void spk_msc_src1_mix_frame(u8 *buf, u32 samples);
void spk_msc_src1_out_frame(void);
void msc_src1_out_start(void);
void dump_putbuf(u8 *buf,  int buf_len, u8 file_idx);
void wsola_process_do(s16 *buf, u16 samples, msc_layer_e wav_layer);
int dnr_fre_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);

#if FUNC_REC_EN
void adpcm_encode_process(void);
u8 mpa_is_encoding(void);
void mpa_encode_frame(void);
extern u8 mp3enc_flag;
extern u8 adpcm_enc_flag;

#if MIC_DET_REC
func_tomcat_t f_tomcat;
u32 tick;
#endif

u8 adpcm_buf_res[498] AT(.adpcm_buf);
u16 res_current_len = 0;  //res buf长度
u16 read_ptr = 0;         //读mic中断buf指针
#endif

func_speaker_t f_spk;

u8 spk_src1_out_frame_check(void)
{
    if(f_spk.pause == 1 || f_spk.mic_dac_en == 0){
        return 0;
    }
    return 1;
}

void speaker_rec_start(void)
{
}

void speaker_rec_stop(void)
{
}

AT(.com_text.speaker)
void soft_eq_drc_proc(s16 *ptr, u32 samples)
{
    s32 sample = 0;
    s16 *pcm_s16 = (s16*)ptr;
    s32 pcm_s32;
    for(int j = 0; j < samples; ++j) {
        sample = ptr[j];
#if SPK_MIC_APF_CHG_EN
        sample = allpass_filter_change(sample, 0);
#endif
        ptr[j] = sample;
        pcm_s32 = (s32)ptr[j];
        if (dac_drc_en) {
            pcm_s32 = drc_v3_calc(pcm_s32);
        }
        pcm_s16[j] = s_clip16(pcm_s32);
    }
}

AT(.text.bsp.speaker)
void func_speaker_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

    if (!f_spk.pause) {
        func_speaker_stop();
#if WARNING_MP3_PLAY
        mp3_res_play(addr, len, 0);
#endif
        func_speaker_start();
    } else {
#if WARNING_MP3_PLAY
        mp3_res_play(addr, len, 0);
#endif
    }
}

AT(.text.bsp.speaker)
void func_speaker_mp3_song_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }
#if SPK_MIC_DNR_FRE_EN
    bsp_dnr_fre_disable();
#endif
    if (!f_spk.pause) {
        func_speaker_stop();
#if WARNING_MP3_PLAY
        mp3_song_play(addr, len, 0);
#endif
    } else {
#if WARNING_MP3_PLAY
        mp3_song_play(addr, len, 0);
#endif
    }
}

AT(.com_text.func.speaker)
void speaker_mic_eq(s16 *ptr, u32 samples)
{
    int i;
    s16 pcm;

    for (i = 0; i < samples; i++) {
        pcm = ptr[i];
        ptr[i] = mic_eq_proc(pcm);
    }
}

#if (MIC_DET_REC && SPK_MIC_MAXPOW_EN)


AT(.com_text.speaker)
void noice_detect(u16 pow)
{
    if (pow < SILENCE_THRESHOLD_POW) {
       f_tomcat.noice_cnt = 0;
       if (f_tomcat.mic_sta != MIC_SILENCE) {
           f_tomcat.silence_cnt++;
           if (f_tomcat.silence_cnt >= 20) {
               f_tomcat.mic_sta = MIC_SILENCE;
               f_tomcat.silence_cnt = 0;
           }
       } else {
           f_tomcat.silence_cnt = 0;
       }
   } else if (pow >= NOISE_THRESHOLD_POW) {
       f_tomcat.silence_cnt = 0;
       if (f_tomcat.mic_sta != MIC_SOUND) {
           f_tomcat.noice_cnt++;
           if (f_tomcat.noice_cnt >= 10) {
               f_tomcat.mic_sta = MIC_SOUND;
               f_tomcat.noice_cnt = 0;
               f_tomcat.vad_time_cnt = 0;   //计时休眠清0
                msg_enqueue(KU_REC);        //由静音变有声开启录音
           }
       } else {
           f_tomcat.noice_cnt = 0;
       }
   }
}
#endif

#if REC_MIC_WAV_OVERLAY_EN
AT(.com_text.speaker)
void src1_pcm_out_cb(void *buf, u32 samples)
{
    u8 *ptr = (u8*)buf;
    if (f_spk.rec_start) {
        puts_rec_obuf(ptr, samples<<1);
    }
}
#endif

AT(.com_text.speaker)
void spk_mic_alg_process(u8 *ptr, u32 samples)
{
#if SPK_MIC_FREQ_SHIFT2_EN
    howlingex_process((short *)ptr, (short *)ptr, samples);
#endif

#if SPK_MIC_ANGLE_CHANGE_EN
    angle_change_process((s16 *)ptr);
#endif

#if SPK_MIC_ECHO_EN
    if (f_spk.mic_echo_en) {
        echo_process_do((s16*)ptr, samples);
    }
#endif

#if SPK_MIC_REVERB_EN
    if (f_spk.mic_reverb_en) {
        reverb_process_do((s16*)ptr, samples);
    }
#endif

#if SPK_MIC_VOICE_CHANGER_EN
    if (f_spk.mic_voice_changer_en) {
        picth_up_process_do((s16*)ptr, samples);
    }
#endif

#if SPK_MIC_DNR_FRE_EN
    if (f_spk.mic_dnr_fre_en) {
        dnr_fre_process((s16 *)ptr, (s32 *)ptr, 0);
    }
#endif
}

AT(.com_rodata.bat)
const char mic_pow_str[] = "mic_max_pow: %d\n";
AT(.com_text.speaker)
void speaker_sdadc_process(u8 *ptr,u32 samples,int ch_mode)
{
    spk_mic_alg_process(ptr, samples);

#if MIC_EQ_EN
    speaker_mic_eq((s16 *)ptr,samples);
#endif // MIC_EQ_EN
    soft_eq_drc_proc((s16 *)ptr, samples);

    pcm_soft_vol_process((s16 *)ptr, dac_digital_gain_tbl[mic_soft_gain & 0x3f], samples);

#if (MIC_DET_REC && SPK_MIC_MAXPOW_EN) //mic能量检测
    f_tomcat.mic_max_pow = dnr_buf_maxpow(ptr, samples);

    if(tick_check_expire(tick,120)){     //上电延时120ms避免静音误判
        noice_detect(f_tomcat.mic_max_pow);
    }
#elif SPK_MIC_MAXPOW_EN
	 u16 mic_max_pow = dnr_buf_maxpow(ptr, samples);
	 my_printf(mic_pow_str, mic_max_pow);
#endif

#if (REC_SPEAKER_EN && MIC_DET_REC)
    if (f_spk.rec_start && (f_tomcat.mic_sta == MIC_SOUND)) {
        if (f_spk.mic_wsola_rec_en){
            wsola_process_do((s16 *)ptr, samples);
            samples = 64;
        }
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#elif REC_SPEAKER_EN && !REC_MIC_WAV_OVERLAY_EN
    if (f_spk.rec_start) {
        if (f_spk.mic_wsola_rec_en){
            wsola_process_do((s16 *)ptr, samples, 0);
            samples = 64;
        }
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif

#if MIC_DET_REC
    if((f_tomcat.mic_sta_pre == MIC_SOUND) && (f_tomcat.mic_sta == MIC_SILENCE)){
        msg_enqueue(KU_REC);            //由有声变静音停止录音并播放
    }
    f_tomcat.mic_sta_pre = f_tomcat.mic_sta;
#endif

#if HUART_DUMP_SPK_EN
    huart_dump_tx(0, 0, ptr, (samples << (1 + ch_mode)));
#endif

#if (!MIC_DET_REC)   //即录即放不直推dac
    if (f_spk.mic_dac_en) {
        spk_msc_src1_mix_frame(ptr, samples);
        spk_msc_src1_out_frame();
    }
#endif
}

///SDADC DMA中断起的AUPCM线程
AT(.com_text.asr.proc)
void asr_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if HUART_DEUMP_EN
    dump_putbuf(ptr, samples, 0);
    dump_putbuf(&ptr[samples], samples, 0);
#endif

#if SPK_SPI_DUMP
    for (u32 i = 0; i < (samples*2); i++) {
        my_spi_putc(ptr[i]);
    }
#endif
}

AT(.text.bsp.speaker)
void func_speaker_start(void)
{
    f_spk.pause = 0;
#if REC_SPEAKER_EN
    rec_src.spr = SPR_16000;
    rec_src.nchannel = 1;
    rec_src.rec_type = REC_MP3;
    rec_src.bitrate = 24000;
    rec_src.source_start = speaker_rec_start;
    rec_src.source_stop  = speaker_rec_stop;
#endif
#if HUART_DUMP_SPK_EN
    huart_dump_init();
#endif
#if ASR_RECOG_EN
    bsp_asr_start();
#endif
    dac_fade_wait();                    //等待淡出完成
    bsp_change_volume(VOL_MAX);
    dac_src1_init();                    //通过src1推dac
    dac_src1_spr_set(SPR_16000);
    msc_src1_out_start();
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
    dac_fade_in();
}

AT(.text.bsp.speaker)
void speaker_mic_alg_init(void)
{
#if SPK_MIC_APF_CHG_EN
    allpass_filter_change_set(0,100);
#endif
#if SPK_MIC_FREQ_SHIFT2_EN
    bsp_freq_shift_init();
#endif
#if SPK_MIC_ANGLE_CHANGE_EN
    bsp_angle_change_init();
#endif
#if SPK_MIC_ECHO_EN
    f_spk.mic_echo_en = 1;
#endif
#if SPK_MIC_REVERB_EN
    f_spk.mic_reverb_en = 1;
#endif
#if SPK_MIC_VOICE_CHANGER_EN
    f_spk.mic_voice_changer_en = 1;
#endif
#if SPK_MIC_WSOLA_REC_EN
    f_spk.mic_wsola_rec_en = 1;
#endif
#if SPK_MIC_DNR_FRE_EN
    f_spk.mic_dnr_fre_en = 1;
#endif
}

AT(.text.bsp.speaker)
static void func_speaker_enter(void)
{
    memset(&f_spk, 0, sizeof(f_spk));
#if SPK_MIC_FREQ_SHIFT2_EN
    sys_clk_set(SYS_160M);
#endif
    speaker_mic_alg_init();
#if ASR_RECOG_EN
    bsp_asr_init();
#endif
#if MIC_EQ_EN
    mic_set_eq_by_res(&RES_BUF_EQ_MIC_16K_EQ, &RES_LEN_EQ_MIC_16K_EQ);
#endif
    f_spk.mic_dac_en = 1;
    func_speaker_start();

#if (MIC_DET_REC && SPK_MIC_MAXPOW_EN)
    memset(&f_tomcat, 0, sizeof(f_tomcat));
    f_tomcat.mic_sta = MIC_SILENCE;
    tick = tick_get();
#endif

}

AT(.text.bsp.speaker)
void func_speaker_stop(void)
{
    f_spk.pause = 1;
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_SPEAKER);
    bsp_change_volume(sys_cb.vol);
    sys_clk_set(SYS_CLK_SEL);
}

static void func_speaker_exit(void)
{
    func_speaker_stop();
    func_cb.last = FUNC_SPEAKER;
}

AT(.text.func.speaker)
void func_speaker_process(void)
{
#if REC_SPEAKER_EN
    bsp_record_process();
#endif
    func_process();
}

AT(.text.func.speaker)
void func_speaker(void)
{
    printf("%s\n", __func__);

    func_speaker_enter();

    while (func_cb.sta == FUNC_SPEAKER) {
        func_speaker_process();
        func_speaker_message(msg_dequeue());
    }

    func_speaker_exit();
}
#endif
