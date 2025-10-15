#ifndef _TKEY_H
#define _TKEY_H

typedef struct {
    u16 tkcd_pr;
    u8 tk_dmacnt;
} tk_cb_t;

void tkey_init_process(tk_cb_t *tk_cfg);
void tkey_gpio_config(u8 io_num);
void tkey_gpio_exit(u8 io_num);
void tkey_off(void);
void tkey_cir_scan_en(void);
void tkey_press_update(void);
extern bool tkey_finish_flag;
extern volatile u32 tk_buf[16];
extern u8 tkey_array[];
#endif
