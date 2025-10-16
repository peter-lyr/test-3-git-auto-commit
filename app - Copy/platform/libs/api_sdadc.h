#ifndef _API_SDADC_H
#define _API_SDADC_H

#define CHANNEL_L           0x0F
#define CHANNEL_R           0xF0

#define ADC2DAC_EN          0x01        //ADC-->DAC
#define ADC2SRC_EN          0x02        //ADC-->SRC

#define CH_AUX_PA13         0x01    //AUX0(PA13) -> left aux  -> sdadc left channel
#define CH_AUX_PA14         0x02    //AUX1(PA14) -> left aux  -> sdadc left channel
#define CH_MIC0             0x0c    //MIC(PF0)   -> left mic  -> sdadc left channel
#define CH_MIC1             0x0d    //MIC DIFF(PF1) -> left mic  -> sdadc left channel

typedef void (*pcm_callback_t)(u8 *ptr, u32 samples, int ch_mode);

typedef struct {
    u8 channel;
    u8 sample_rate;
    u8 anl_gain;
    u8 dig_gain;
    u8 bits_mode;                       //ADC BITS选择；0: 24bits, 1: 16bits, 0xff: 跟随DAC的BIT MODE
    u8 out_ctrl;
    u16 samples;
    pcm_callback_t callback;
} sdadc_cfg_t;


void sdadc_dummy(u8 *ptr, u32 samples, int ch_mode);
void sdadc_pcm_2_dac(u8 *ptr, u32 samples, int ch_mode);
void sdadc_var_init(void);

int sdadc_init(const sdadc_cfg_t *p_cfg);
int sdadc_start(u8 channel);
int sdadc_exit(u8 channel);

void mic_bias_var_init(void);
void mic_bias_trim(void);

#endif // _API_SDADC_H

