#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_USBDEV_EN
AT(.text.func.usbdev.msg)
void func_usbdev_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            ude_hid_key_send(UDE_HID_PLAYPAUSE, 5);
            break;

        case KD_PLAY:
        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            ude_hid_key_send(UDE_HID_PREVFILE, 5);
            break;

        case KTH_PLAY:
        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            ude_hid_key_send(UDE_HID_NEXTFILE, 5);
            break;

        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
        case KL_PREV_VOL_DOWN:
        case KH_PREV_VOL_DOWN:
        case KU_VOL_DOWN_PREV:
            ude_hid_key_send(UDE_HID_VOLDOWN, 5);
            break;

        case KU_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
        case KL_NEXT_VOL_UP:
        case KH_NEXT_VOL_UP:
        case KU_VOL_UP_NEXT:
            ude_hid_key_send(UDE_HID_VOLUP, 5);
            break;

        case KU_STOP:
            ude_hid_key_send(UDE_HID_STOP, 5);
            break;

#if UDE_STORAGE_EN
        case EVT_SD_INSERT:
            ude_sdcard_switch(DEV_SDCARD);
            break;

        case EVT_SD_REMOVE:
            break;

#endif

        case EVT_UDE_SET_VOL:
            ude_set_sys_volume_do(f_ude.db_level);
            break;

        case EVT_PC_REMOVE:
            func_cb.sta = func_cb.last;
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif
