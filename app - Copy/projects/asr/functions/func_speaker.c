#include "include.h"
#include "func.h"

#if FUNC_SPEAKER_EN
extern uint8_t dac_drc_en;
extern uint8_t dac_soft_gain;

void kws2nd_pcm_callback(u8 *ptr, u16 len);
s16 drc_v3_calc(s32 sample);
int s_clip16(int x);
void angle_change_process(s16 *data);

#if FUNC_REC_EN
void adpcm_encode_process(void);
u8 mpa_is_encoding(void);
void mpa_encode_frame(void);
extern u8 mp3enc_flag;
extern u8 adpcm_enc_flag;

u8 adpcm_buf_res[498] AT(.adpcm_buf);
u16 res_current_len = 0;  //res buf长度
u16 read_ptr = 0;         //读mic中断buf指针
#endif

func_speaker_t f_spk;

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
        mp3_res_play(addr, len);
#endif
        func_speaker_start();
    } else {
#if WARNING_MP3_PLAY
        mp3_res_play(addr, len);
#endif
    }
}

AT(.com_text.speaker)
void speaker_sdadc_process(u8 *ptr,u32 samples,int ch_mode)
{
    soft_eq_drc_proc((s16 *)ptr, samples);
#if SPK_MIC_FREQ_SHIFT2_EN
    howlingex_process((short *)ptr, (short *)ptr, samples);
#endif

#if SPK_MIC_ANGLE_CHANGE_EN
    angle_change_process((s16 *)ptr);
#endif

#if REC_SPEAKER_EN
    if (f_spk.rec_start) {
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif

#if SPK_MIC_ECHO_EN
    echo_process_do((s16*)ptr, samples);
#endif

#if SPK_MIC_REVERB_EN
    reverb_process_do((s16*)ptr, samples);
#endif

#if HUART_DUMP_SPK_EN
    huart_dump_tx(0, 0, ptr, (samples << (1 + ch_mode)));
#endif
    aubuf0_dma_w4_done();
    aubuf0_dma_kick(ptr, samples, 1, false);
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
    func_speaker_start();
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
