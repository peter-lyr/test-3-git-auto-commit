#ifndef _FUNC_USBDEV_H
#define _FUNC_USBDEV_H

//USB Device HID Comsumer usage id
#define UDE_HID_PLAYPAUSE       BIT(0)                  //0xcd
#define UDE_HID_VOLUP           BIT(1)                  //0xe9
#define UDE_HID_VOLDOWN         BIT(2)                  //0xea
#define UDE_HID_RESV            BIT(3)                  //0xcf
#define UDE_HID_HOME            BIT(4)                  //0x40
#define UDE_HID_NEXTFILE        BIT(5)                  //0xb5
#define UDE_HID_PREVFILE        BIT(6)                  //0xb6
#define UDE_HID_STOP            BIT(7)                  //0xb7

typedef struct {
    u8 vol;
    u8 db_level;
    u8 cur_dev;
    u8 dev_change;
    u8 rw_sta;
} f_ude_t;
extern f_ude_t f_ude;

void func_usbdev_message(u16 msg);
void ude_sdcard_switch(u8 dev);
void ude_sdcard_change_process(void);
void ude_set_sys_volume_do(u8 level);

#if (GUI_SELECT != GUI_NO)
void func_usbdev_display(void);
void func_usbdev_enter_display(void);
void func_usbdev_exit_display(void);
#else
#define func_usbdev_display()
#define func_usbdev_enter_display()
#define func_usbdev_exit_display()
#endif

#endif // _FUNC_USBDEV_H
