#ifndef _API_USB_H
#define _API_USB_H

enum {
    USBCHK_ONLY_HOST,
    USBCHK_ONLY_DEVICE,
    USBCHK_OTG,
};

enum {
    USB_DISCONNECT,
    USB_UDISK_CONNECTED,
    USB_PC_CONNECTED,
    USB_DP_GND_CONNECTED,           //假USB设备
    USB_DPDM_SC_CONNECTED,          //Short-Circuit
};

void usb_init(void);                                        //USB初始化使能
void usb_disable(void);                                     //USB关闭
u8 usbchk_connect(u8 mode);                                 //USB模式检测

void udisk_remove(void);                                    //移除U盘设备使能
void udisk_insert(void);                                    //插入U盘设备使能
void udisk_invalid(void);                                   //USB主机状态还原
void udisk_suspend(void);                                   //挂起USB主机设备
void udisk_resume(void);                                    //恢复挂起的USB主机设备

void uda_sync_reset(void);                                  //USB从机调速复位
void usb_device_enter(u8 enum_type);                        //USB从机功能使能 enum_type：UDE_STORAGE_EN...
void usb_device_exit(void);                                 //退出USB从机
void usb_device_process(void);                              //USB从机设备主循环
void ude_tmr_isr(void);                                     //USB从机
void pc_remove(void);                                       ////USB从机退出
bool ude_hid_key_send(u16 hid_val, int auto_release);       //发送一个HID COMSUMER按键

#endif // _API_USB_H

