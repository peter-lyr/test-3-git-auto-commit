#include "include.h"
#include "func.h"
#include "func_usbdev.h"

#if FUNC_USBDEV_EN

f_ude_t f_ude;

AT(.text.func.usbdev)
void ude_mic_start(void)
{
    audio_path_init(AUDIO_PATH_USBMIC);
    audio_path_start(AUDIO_PATH_USBMIC);
    dac_spr_set(SPR_48000);
}

AT(.text.func.usbdev)
void ude_mic_stop(void)
{
    audio_path_exit(AUDIO_PATH_USBMIC);
}

AT(.usbdev.com)
u8 ude_get_sys_volume(void)
{
    return f_ude.db_level;
}

AT(.usbdev.com)
void ude_set_sys_volume(u8 vol)
{
    if (f_ude.db_level != vol) {
        f_ude.db_level = vol;
        msg_enqueue(EVT_UDE_SET_VOL);
    }
}

AT(.usbdev.com)
u16 ude_get_vol_db_value(u8 db_level)
{
    return dac_dvol_tbl_db[db_level];
}

u32 ude_volume_to_db(u8 vol)
{
    u32 level;
    if (vol > VOL_MAX) {
        vol = VOL_MAX;
    }
    level = dac_dvol_table[vol];
    return 60 - level;
}

void ude_set_sys_volume_do(u8 level)
{
    s32 db_num;
    db_num = 60 - level;
    if (db_num > 60) {
        db_num = 60;
    }
    if (db_num < 0) {
        db_num = 0;
    }
    dac_set_dvol(dac_dvol_tbl_db[db_num]);
}

#if UDE_STORAGE_EN
AT(.text.func.usbdev)
void ude_sdcard_switch(u8 dev)
{
//    printf("%s, %d, %d\n", __func__, f_ude.cur_dev, dev);
    if (f_ude.cur_dev != dev) {
        ude_sd_remove();
        f_ude.cur_dev = dev;
        fsdisk_callback_init(f_ude.cur_dev);
        f_ude.dev_change = 1;
    } else {
        sd0_init();
    }
}

AT(.text.func.usbdev)
void ude_sdcard_change_process(void)
{
    static u8 cnt_s = 0;
    if (f_ude.dev_change) {
        cnt_s++;
        if (cnt_s >= 5) {
            f_ude.dev_change = 0;
            sd0_init();
        }
    } else {
        cnt_s = 0;
    }
}
#endif // UDE_STORAGE_EN


AT(.text.func.usbdev)
void func_usbdev_process(void)
{
    func_process();
    usb_device_process();
    f_ude.rw_sta = sd0_get_rw_sta();
}

static void func_usbdev_enter(void)
{
    func_cb.mp3_res_play = NULL;
    f_ude.cur_dev = DEV_SDCARD;
    f_ude.dev_change = 0;
    f_ude.vol = sys_cb.vol;
    func_usbdev_enter_display();
    led_idle();
#if WARNING_FUNC_USBDEV
    mp3_res_play(RES_BUF_PC_MODE_MP3, RES_LEN_PC_MODE_MP3);
#endif

#if UDE_SPEAKER_EN
    dac_spr_set(SPR_48000);         //samplerate 48K
    f_ude.db_level = ude_volume_to_db(VOL_MAX);
    bsp_change_volume(VOL_MAX);
    dac_fade_in();
    uda_sync_reset();               //usb audio调速初始化
    aubuf0_dma_init(1);
#endif // UDE_SPEAKER_EN

#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        if (dev_is_online(DEV_SDCARD)) {
            f_ude.cur_dev = DEV_SDCARD;
        } else if (dev_is_online(DEV_SDCARD1)) {
            f_ude.cur_dev = DEV_SDCARD1;
        }
        fsdisk_callback_init(f_ude.cur_dev);
        fs_mount();
    }
#endif
    usb_device_enter(UDE_ENUM_TYPE);
}

static void func_usbdev_exit(void)
{
    usb_device_exit();
    func_usbdev_exit_display();
#if UDE_SPEAKER_EN
    DACDIGCON0 &= ~BIT(6);
    sys_cb.vol = f_ude.vol;         //recover system volume
    bsp_change_volume(sys_cb.vol);
    dac_fade_out();
#endif // UDE_SPEAKER_EN
#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
    #if I2C_MUX_SD_EN
        sd0_go_idle_state();
    #endif
    }
#endif // UDE_STORAGE_EN
    func_cb.last = FUNC_USBDEV;
}

AT(.text.func.usbdev)
void func_usbdev(void)
{
    if (!dev_is_online(DEV_USBPC)) {
        func_cb.last = FUNC_USBDEV;
        func_cb.sta = FUNC_NULL;
        return;
    }

    printf("%s\n", __func__);
    func_usbdev_enter();
    while (func_cb.sta == FUNC_USBDEV) {
        func_usbdev_process();
        func_usbdev_message(msg_dequeue());
        func_usbdev_display();
    }

    func_usbdev_exit();
}
#endif // FUNC_USBDEV_EN
