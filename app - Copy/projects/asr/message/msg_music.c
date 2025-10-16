#include "include.h"
#include "func.h"
#include "func_music.h"

#if FUNC_MUSIC_EN
AT(.text.func.idle.msg)
void func_music_message(u16 msg)
{
#if MIDI_KEYBOARD_EN
    u16 key_num = (msg & 0x00FF);
    if(key_num >= KEY_MAT_1 && key_num <= KEY_MAT_88){
        midi_keyboard_msg_proc(msg);
        return;
    }
#endif

    switch (msg) {

        case KU_PLAY:
        case KU_PLAY_HSF:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            if (f_msc.pause) {
                f_msc.pause = 0;
                music_control(MUSIC_MSG_PLAY);
                led_music_play();
                printf("music continue playing\n");
            } else {
                f_msc.pause = 1;
                bsp_clr_mute_sta();
                music_control(MUSIC_MSG_PAUSE);
                led_idle();
                printf("music pause\n");
            }
            break;

        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            bsp_clr_mute_sta();
            func_music_switch_file(0);
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            bsp_clr_mute_sta();
            func_music_switch_file(1);
            break;

#if MUSIC_QSKIP_EN
        case KL_NEXT:
        case KH_NEXT:
            bsp_clr_mute_sta();
            music_qskip(QSKIP_BACKWARD, 3);             //快进
            break;

        case KL_PREV:
        case KH_PREV:
            bsp_clr_mute_sta();
            music_qskip(QSKIP_FORWARD, 3);               //快退
            break;

        case KLU_NEXT:
        case KLU_PREV:
            music_qskip_end();
            break;
#endif // MUSIC_QSKIP_EN

#if MUSIC_FOLDER_SELECT_EN
        case KD_VOL_DOWN_PREV:
            func_music_switch_dir(0);
            break;

        case KD_VOL_UP_NEXT:
            func_music_switch_dir(1);
            break;
#endif // MUSIC_FOLDER_SELECT_EN

        case KL_PLAY:
        case KU_REPEAT:
            music_playmode_next();
            break;

        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
            bsp_clr_mute_sta();
            if ((f_msc.cur_dev == DEV_UDISK) && (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1))) {
                f_msc.dev_change = 1;
                music_breakpoint_save();
            } else if ((f_msc.cur_dev == DEV_SDCARD1) && (dev_is_online(DEV_SDCARD))) {
                f_msc.dev_change = 1;
                music_breakpoint_save();
            } else {
                func_cb.sta = FUNC_NULL;
            }
            break;

        case KU_HSF:
        case KU_USB_SD:
            //双设备切换
            if (dev_is_online(DEV_UDISK) && (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1))) {
                bsp_clr_mute_sta();
                f_msc.dev_change = 1;
                music_breakpoint_save();
            }
            break;

        case EVT_UDISK_REMOVE:
            func_music_remove_device(DEV_UDISK);
            break;

        case EVT_SD_REMOVE:
            func_music_remove_device(DEV_SDCARD);
            break;

        case EVT_UDISK_INSERT:
            //防止插入苹果手机打断SD卡播放
            if (!dev_udisk_activation_try(0)) {
                break;
            }
            func_music_insert_device(DEV_UDISK);
            break;

        case EVT_SD_INSERT:
            func_music_insert_device(DEV_SDCARD);
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_MUSIC_EN
