#include "include.h"

#if LED_DISP_EN
led_func_t bled_func AT(.buf.led);
led_func_t rled_func AT(.buf.led);
gpio_t bled_gpio AT(.buf.led);
gpio_t rled_gpio AT(.buf.led);

AT(.text.led_disp)
void led_power_up(void)
{
    led_set_sta(0x00, 0xff, 1, 0);          //���Ƴ���
}

AT(.text.led_disp)
void led_power_down(void)
{
    led_set_sta(0x00, 0x00, 0, 0);          //��ƣ�������
}

AT(.text.led_disp)
void led_idle(void)
{
    led_set_sta(0x00, 0xff, 1, 0);               //���Ƴ���
}

AT(.text.led_disp)
void led_music_busy(void)
{
     led_set_sta(0x00, 0xaa, 4, 0);            //����200ms���ڿ���˸
}

AT(.text.led_disp)
void led_music_play(void)
{
    led_set_sta(0x00, 0xaa, 20, 0);           //����1s������˸
}

AT(.text.led_disp)
void led_aux_play(void)
{
     led_set_sta(0x00, 0xaa, 20, 0);           //����1s������˸
}

AT(.text.led_disp)
void led_aux_idle(void)
{
     led_set_sta(0x00, 0xff, 1, 0);            //���Ƴ���
}


AT(.text.led_disp)
void led_fm_play(void)
{
     led_set_sta(0x00, 0xaa, 20, 0);           //����1s������˸
}

AT(.text.led_disp)
void led_fm_scan(void)
{
    led_set_sta(0x00, 0xaa, 4, 0);            //����200ms���ڿ���˸
}

AT(.text.led_disp)
void led_record(void)
{
    led_set_sta(0x00, 0xaa, 20, 0);           //����1s������˸
}

void led_func_init(void)
{
    bsp_gpio_cfg_init(&bled_gpio, xcfg_cb.bled_io_sel);
    bsp_gpio_cfg_init(&rled_gpio, xcfg_cb.rled_io_sel);

//#if LED_PWR_EN
//    if (xcfg_cb.bled_io_sel == xcfg_cb.rled_io_sel) {
//        //1��IO��������
//        if (bled_gpio.sfr != NULL) {
//            sys_cb.port2led_en = 1;
//        }
//        bled_func.port_init = led2_port_init;
//        bled_func.set_on = bled2_set_on;
//        bled_func.set_off = bled2_set_off;
//
//        rled_func.port_init = led2_port_init;
//        rled_func.set_on = rled2_set_on;
//        rled_func.set_off = rled2_set_off;
//    } else
//#endif // LED_PWR_EN
    {
        sys_cb.port2led_en = 0;
        bled_func.port_init = led_cfg_port_init;
        bled_func.set_on = led_cfg_set_on;
        bled_func.set_off = led_cfg_set_off;
#if USER_ADKEY_MUX_LED
        bled_func.port_init = led_mux_cfg_port_init;
        bled_func.set_on = led_mux_cfg_set_on;
        bled_func.set_off = led_mux_cfg_set_off;
#endif // USER_ADKEY_MUX_LED

        rled_func.port_init = led_cfg_port_init;
        rled_func.set_on = led_cfg_set_on;
        rled_func.set_off = led_cfg_set_off;
    }
}

u8 led_get_cfg_port(void)
{
    if(xcfg_cb.led_disp_en) {
        return xcfg_cb.bled_io_sel; //����ѡ������
    } else if(xcfg_cb.led_pwr_en) {
        return xcfg_cb.rled_io_sel; //���ѡ����
    }
    return 0;                       //����ʾ
}
#endif
