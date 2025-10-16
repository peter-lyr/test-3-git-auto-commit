/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

#if !MUSIC_UDISK_EN
void os_sem_usb_init(void) {}
bool udisk_read(void *buf, u32 lba) {return false;}
bool udisk_write(void* buf, u32 lba) {return false;}
bool usb_host_init(void) {return false;}
bool dev_udisk_activation_try(u8 mode) {return false;}
#endif

#if !FUNC_USBDEV_EN
void usb_dev_isr(void) {}
void lock_code_usbdev(void) {}
void unlock_code_usbdev(void) {}
void os_mq_ude_post(uint8_t index) {}
void ude_msg_proc(u8 mq_msg) {}
void uda_put_samples(void *buf, u32 samples, bool is_24bit) {}
#endif

#if !UDE_STORAGE_EN
void uds_init(void *tcb) {}
void uds_run_loop_execute(void *tcb){}
#endif

#if!UDE_MIDI_EN
AT(.usbdev.com)
void usb_midi_bulk_reset(void) {}
#endif

#if (!SD_SUPPORT_EN && !UDE_STORAGE_EN)
void sd_disk_init(void){}
void sdctl_isr(void){}
void sd0_irq_init(void){}
void sd_disk_switch(u8 index){}
bool sd0_stop(bool type){return false;}
void os_sem_sd_init(void){}

bool sd0_init(void){return false;}
bool sd0_read(void *buf, u32 lba){return false;}
bool sd0_write(void* buf, u32 lba){return false;}
#endif

#if !USB_SUPPORT_EN
void usb_isr(void){}
void usb_init(void){}
#endif


#if (SPI1_AUDIO_EN)
void spi_audio_send_test(void *buf, u16 length){}
#endif

#if (!WARNING_WAV_PLAY)
void wav_res_play_init(u32 addr, u32 len, u32 skip_start_bytes, u32 skip_end_bytes, u8 loop_cnt, wav_layer_t *wav_layer){}
bool wav_res_play_kick(u32 addr, u32 len, wav_layer_t *wav_layer,u8 loop_cnt){}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) != (MSC_WAV_LAYER1))
int wav1_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav1_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav1_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) != (MSC_WAV_LAYER2))
int wav2_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav2_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav2_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) != (MSC_WAV_LAYER3))
int wav3_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav3_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav3_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) != (MSC_WAV_LAYER4))
int wav4_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav4_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav4_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) != (MSC_WAV_LAYER5))
int wav5_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav5_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav5_res_ptr(void){return NULL;}
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) != (MSC_WAV_LAYER6))
int wav6_spi_stream_read(void *buf, unsigned int size){return 0;}
bool wav6_spi_stream_seek(unsigned int ofs, int whence){return false;}
spires_cb_t *get_wav6_res_ptr(void){return NULL;}
#endif

#if (EX_SPIFLASH_SUPPORT & EXSPI_MUSIC) || (EX_SPIFLASH_SUPPORT & EXSPI_REC)
void wav_flash_read(void *buf, u32 addr, u32 len)
{
    spiflash1_read(buf, addr, len);
}
#else
void exspiflash_wav_mix_num_kick(u32 num) {}
void wav_mix_dec(u16 samples) {}
bool spiflash_rec_play_last_file(void){return false;}
#endif

#if !EXSPI_WAV_MIX
void wav_mix_kick(u32 addr, u32 len){}
#endif

#if (!WARNING_VSBC_PLAY) && (!EX_SPIFLASH_SUPPORT)
u16 spiflash_vsbc_loop_skip_start_size(void) {return 0;}
u16 spiflash_vsbc_loop_skip_end_size(void) {return 0;}
u16 spiflash_vsbc_skip_start_size(void) {return 0;}
u16 spiflash_vsbc_skip_end_size(void) {return 0;}
#endif

#if !MIDI_EN
u8 get_midi_dec_sta(void){return 0;}
void midi_control(u8 sta){}
void dcache_init_main(void){}
AT(.pdncom_text.lp.sleep)
void dcache_init_l4m(void){}

void vmidi_res_play(u32 addr, u32 len){}
void vmidi_song_play(u32 addr, u32 len){}
void midi_song_play(u32 addr, u32 len){}
#endif

#if !SPK_MIC_APF_CHG_EN
void lock_code_apf_chg(void) {}
#endif

#if (!USER_ADKEY) && (!USER_ADKEY2) && (!USER_IOKEY)
u16 key_multi_press_process(u16 key_val){return 0;}
#endif

#if FFT_PROC_EN
    #if !FFT_512_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_512[1] = {};
    #endif
    #if !FFT_256_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_256[1] = {};
    #endif
    #if !FFT_128_EN
    AT(.fft_rodata.windown)
    const s32 rdft_w_128[1] = {};
    #endif
#endif

#if !MIC_EQ_EN
AT(.com_text.eq)
s16 mic_eq_proc(s16 input) {return 0;}
#endif
bool dac_set_eq_by_res(u32 *addr, u32 *len){return false;}

#if !MUSIC_DECODE_BK_EN
void mp3_song_play(u32 addr, u32 len,u8 loop_cnt){}
void vmp3_song_play(u32 addr, u32 len,u8 loop_cnt){}
void esbc_song_play(u32 addr, u32 len,u8 loop_cnt){}
void lsbc_song_play(u32 addr, u32 len,u8 loop_cnt){}
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len,u8 loop_cnt){}
#endif

#if !MIDI_REC_EN
bool get_midi_rec_en(void){return 0;}
#endif

#if !MIDI_OKON_EN
u8 get_okon_key_cnt(void){return 0;}
void reset_okon_key_cnt(void){}
#endif

#if !MUSIC_ESBC_SUPPORT
int esbc_decode_init(void) {return 0;}
#endif

#if (!WARNING_ESBC_PLAY && !MUSIC_ESBC_SUPPORT)
bool esbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en) {return false;}
bool esbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt){return false;}
void esbc_res_play_exit(void){}
int spi_esbc_decode_init(void){return 0;}

bool esbc_dec_frame(void) {return false;}
int esbc_dec_init(void *read_func, void *seek_func){return 0;}
#endif

#if (!WARNING_LSBC_PLAY && !MUSIC_LSBC_SUPPORT)
bool lsbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en) {return false;}
bool lsbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt) {return false;}
int lsbc_decode_init(void) {return 0;}
int spi_lsbc_decode_init(void) {return 0;}
bool lsbc_dec_frame(void) {return false;}
int lsbc_dec_init(void *read_func, void *seek_func){return 0;}
#endif

#if !REC_MP3_SUPPORT
void mpa_encode_kick_start(void) {}
void mpa_encode_frame(void) {}
bool mpa_encode_init(u32 spr, u32 nch, u32 bitrate) {return false;}
void mpa_encode_exit(void) {}
#endif

#if !FUNC_AUX_EN
void sdadc_analog_aux_start(u8 channel, u8 gain) {}
void sdadc_analog_aux_exit(u8 channel) {}
#endif

#if !DAC_DRC_EN
bool drc_v3_init(const void *bin, int bin_size){return false;}
AT(.drc_text.drc_proc)
s16 drc_v3_calc(s32 sample){return 0;}
#endif

#if !DAC_CLASSD_SW_EN
void dac_voutp_set(u8 io_sta){}
void dac_voutn_set(u8 io_sta){}
#endif

#if !SPK_MIC_ECHO_EN
void bsp_echo_init(void) {}
void echo_process_do(mic_pcm_t *data_buf, u16 samples) {}
#endif

#if !SPK_MIC_REVERB_EN
void reverb_buf_init(rvb_init_sb *p, u8 idx) {}
void reverb_process(s16 *sample, u8 idx) {}
void reverb_process_do(s16 *buf, u16 samples) {}
#endif

#if !SPK_MIC_VOICE_CHANGER_EN && !WARNING_VOICE_CHANGER_PLAY_EN
void voice_changer_init(void) {}
void voice_change_process(s16 *data, u32 samples, u8 idx) {}
#endif

#if !VOICE_CHANGER_FS_EN
void formant_shift_process(s16 *data, u32 samples, u8 idx) {}
#endif

#if !SPK_MIC_WSOLA_REC_EN && !WARNING_WSOLA_WAV_PLAY_EN
void bsp_wsola_init(void) {}
void wsola_process_do(s16 *buf, u16 samples, msc_layer_e wav_layer) {}
#endif

#if !USB_SD_UPDATE_EN
void updateproc(void) {}
bool update_read(void *buf, u32 addr) { return false; }
int updatefile_init(const char *file) { return 0; }
#endif