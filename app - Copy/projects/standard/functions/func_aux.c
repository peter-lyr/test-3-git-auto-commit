#include "include.h"
#include "func.h"
#include "func_aux.h"

#if FUNC_AUX_EN
func_aux_t f_aux;

void aux_rec_start(void)
{
}

void aux_rec_stop(void)
{
}

AT(.com_text.aux.proc)
void aux_sdadc_process(u8 *ptr,u32 samples,int ch_mode)
{
#if REC_AUX_EN
    if (f_aux.rec_start) {
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif
#if HUART_DUMP_AUX_EN
    huart_dump_tx(0, 0, ptr, (samples << (1 + ch_mode)));
#endif
    aubuf0_dma_w4_done();
    aubuf0_dma_kick(ptr, samples, 1, false);
}

AT(.text.bsp.aux)
void func_aux_stop(void)
{
    if (!f_aux.start) {
        return;
    }
//    printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_AUX);
    f_aux.start = 0;
}

AT(.text.bsp.aux)
void func_aux_start(void)
{
    if (f_aux.start) {
        return;
    }
//    printf("%s\n", __func__);

#if REC_AUX_EN
    rec_src.spr = SPR_48000;
    rec_src.nchannel = 1;
    rec_src.rec_type = REC_MP3;
    rec_src.bitrate = 128000;
    rec_src.source_start = aux_rec_start;
    rec_src.source_stop  = aux_rec_stop;
#endif

    dac_fade_wait();                    //等待淡出完成
    audio_path_init(AUDIO_PATH_AUX);
    audio_path_start(AUDIO_PATH_AUX);
    dac_fade_in();
    f_aux.start = 1;
}

AT(.text.bsp.aux)
bool aux_is_start(void)
{
    return f_aux.start;
}

AT(.text.func.aux)
void func_aux_pause_play(void)
{
    if (f_aux.pause) {
        led_aux_play();
        func_aux_start();
        f_aux.pause = 0;
    } else {
        led_aux_idle();
        func_aux_stop();
        f_aux.pause = 1;
    }
}

AT(.text.func.aux)
void func_aux_setvol_callback(u8 dir)
{
    if (f_aux.pause) {
        func_aux_pause_play();
    }
}

AT(.text.func.aux)
void func_aux_process(void)
{
#if REC_AUX_EN
    bsp_record_process();
#endif
    func_process();
}

static void func_aux_enter(void)
{
#if LINEIN_DETECT_EN
    if (!dev_is_online(DEV_LINEIN)) {
        func_cb.sta = FUNC_NULL;
        return;
    }
#endif
    memset(&f_aux, 0, sizeof(func_aux_t));
    func_cb.set_vol_callback = func_aux_setvol_callback;
    msg_queue_clear();

#if HUART_DUMP_AUX_EN
    huart_dump_init();
#endif
    func_aux_start();
    led_aux_play();
}

static void func_aux_exit(void)
{
    func_aux_stop();
    func_cb.last = FUNC_AUX;
}

AT(.text.func.aux)
void func_aux(void)
{
    printf("%s\n", __func__);

    func_aux_enter();

    while (func_cb.sta == FUNC_AUX) {
        func_aux_process();
        func_aux_message(msg_dequeue());
    }

    func_aux_exit();
}
#endif  //FUNC_AUX_EN
