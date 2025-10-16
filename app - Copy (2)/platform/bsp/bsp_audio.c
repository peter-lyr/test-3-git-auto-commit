#include "include.h"

void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void asr_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

#if (FUNC_SPEAKER_EN && ASR_RECOG_EN)
    #define speaker_sdadc_callback  asr_sdadc_process
#elif FUNC_SPEAKER_EN
    #define speaker_sdadc_callback  speaker_sdadc_process
#else
    #define speaker_sdadc_callback  sdadc_dummy
#endif // FUNC_SPEAKER_EN

#if FUNC_AUX_EN
    #define aux_sdadc_callback  aux_sdadc_process
#else
    #define aux_sdadc_callback  sdadc_dummy
#endif // FUNC_AUX_EN

#if UDE_MIC_EN
    #define usbmic_sdadc_callback   usbmic_sdadc_process
#else
    #define usbmic_sdadc_callback   sdadc_dummy
#endif // UDE_MIC_EN

#if ASR_RECOG_EN
#define SDADC_DMA_SIZE              640         //DMASIZE = 最大样点数
#else
#define SDADC_DMA_SIZE              320         //DMASIZE = 最大样点数
#endif
u16 sdadc_buf[SDADC_DMA_SIZE] AT(.sdadc_buf);

const sdadc_cfg_t rec_cfg_tbl[] = {
/*   通道,                采样率,     模拟增益,      数字增益,    BITS,    通路控制,    样点数,   回调函数*/
#if ASR_RECOG_EN
    {CH_MIC0,           SPR_16000,      1,              30,        1,     ADC2DAC_EN,    320,    speaker_sdadc_callback},      /* ASR     */
#else
    {CH_MIC0,           SPR_16000,      1,              30,        1,     ADC2DAC_EN,    256,    speaker_sdadc_callback},      /* SPEAKER */
#endif
    {CH_MIC0,           SPR_48000,      1,              30,        1,     ADC2DAC_EN,    256,    usbmic_sdadc_process},        /* USBMIC */
    {AUX_CHANNEL_CFG,   SPR_48000,   AUX_ANL_GAIN,  AUX_DIG_GAIN,  1,     ADC2DAC_EN,    256,    aux_sdadc_callback},
};

u16 get_sdadc_dma_size(void)
{
    return SDADC_DMA_SIZE;
}

void audio_path_init(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

    if (path_idx == AUDIO_PATH_SPEAKER || path_idx == AUDIO_PATH_USBMIC) {
        cfg.anl_gain = xcfg_cb.mic_anl_gain;
        cfg.dig_gain = xcfg_cb.mic_dig_gain;
    }

    #if SPK_MIC_WSOLA_REC_EN
        cfg.samples = SPK_MIC_WSOLA_INPUT_LEN*2;
    #endif
    sdadc_init(&cfg);
}

void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
    sdadc_start(cfg.channel);
}

void audio_path_exit(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
    sdadc_exit(cfg.channel);
}

