#include "include.h"
#include "tkey.h"

#if TKEY_MUL_SCAN_EN
#define TKEY_ADJ_LEVEL_MAX    100

//两个按键同时按下触发的消息
#define TKEY_MUL_CHANNEL_MSG       0

#define TKEY_WRITE_FLASH_TIME      5

#define TKEY_DEBUG_EN            0
#if     TKEY_DEBUG_EN
#define TRACE(...)          my_printf(__VA_ARGS__)
AT(.com_text.rodata)
const char tk_test1[] = "buf[0] %d\n";

AT(.com_text.rodata)
const char tk_test2[] = "buf[1] %d\n";

AT(.com_text.rodata)
const char tk_test3[] = "buf[2] %d\n";

AT(.com_text.rodata)
const char tk_test4[] = "buf[3] %d\n";
#else
#define TRACE(...)
#endif

//定义touchkey按键按下灵敏度
u16 tkey_press_cnt[16];
u8 tkey_array[] = TKEY_ARRAY;
u8 tkey_tkcnt_adj_en = TKEY_TKCNT_ADJ_EN;
const u16 tkey_tkcnt_adj_times = TKEY_TKCNT_ADJ_TIMES;
const u8 tkey_tkcnt_adj_level = TKEY_TKCNT_ADJ_LEVEL;

u16 tkey_val_clr_cnt = 0;
u32 tkey_adj_val[16];
//extern const u16 tbl_double_key[];


AT(.com_rodata.tkey.table)
const u8 tkey_table[] = {
    NO_KEY,      //第一位不能改
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_VOL_UP,
    KEY_PREV,
    KEY_NEXT,
    KEY_MODE,
    KEY_HSF,
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_VOL_UP,
    KEY_PREV,
    KEY_NEXT,
    KEY_MODE,
    KEY_HSF,
    KEY_VOL_DOWN,
    KEY_PLAY,
    KEY_IR_POWER,
};

void tkey_press_update(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,TKEY_PRESS_UPDATE_TIME)) {
        ticks = tick_get();
        tkey_val_clr_cnt = 0;
    }
}

#if TKEY_MUL_CHANNEL_MSG
#define TKEY_CHANNEL0       0
#define TKEY_CHANNEL1       1
#define MUL_MSG             2

AT(.com_text.tkey.proc)
u8 mul_channel_proc(void)
{
    if((tk_buf[TKEY_CHANNEL1] > tkey_press_cnt[TKEY_CHANNEL1]) && (tk_buf[TKEY_CHANNEL0] > tkey_press_cnt[TKEY_CHANNEL0])){
        return tkey_table[MUL_MSG];
    }
    return tkey_table[0];
}
#endif

#if TKEY_SCAN_SWITCH_EN
AT(.com_text.tkey.proc)
void tkey_cir_scan_en(void)
{
    u8 key_val = NO_KEY;

    if(tkey_finish_flag) {
        if(key_val == NO_KEY) {
            key_val = get_tkey();
        }
        msg_enqueue(key_val);
    }
}
#endif

AT(.com_text.tkey.proc)
bool tkey_get_scan_num(void)
{
    for(u8 i = 0; i <= sizeof(tkey_array); i++) {
        if(tk_buf[i] > tkey_press_cnt[i]) {
        return true;
        }
    }
    return false;
}

AT(.com_text.tkey.proc)
u8 tkey_get_scan()
{
    while(tkey_get_scan_num()) {
#if TKEY_MUL_CHANNEL_MSG
        return mul_channel_proc();
#endif // TKEY_MUL_CHANNEL_MSG
        for(u8 i = 0; i < sizeof(tkey_array); i++) {
#if TKEY_FILTER_EN
            static u16 tkey_val =0;
            for(u8 j = 0; j < TKEY_FILTER_TIMES; j++) {
                tkey_val += tk_buf[i];
                if(j == (TKEY_FILTER_TIMES - 1)) {
                    tkey_val = tkey_val / TKEY_FILTER_TIMES;
                }
            }
            if(tkey_val > tkey_press_cnt[i]) {
                tkey_val = 0;
                return tkey_table[i+1];
            }
            tkey_val = 0;
#else
            if(tk_buf[i] > tkey_press_cnt[i]) {
                return tkey_table[i+1];
            }
#endif
        }
    }
    return tkey_table[0];
}

#if TKEY_TKCNT_ADJ_EN

//自动检测tkey的值
AT(.com_text.tkey)
void tkey_val_clr(u32 *key_val)
{
    u32 *p = key_val;
    if(tkey_val_clr_cnt < tkey_tkcnt_adj_times)
    {
        for(u8 i = 0; i < sizeof(tkey_array); i++) {                         //累加tkey_adj_val
            tkey_adj_val[i] = tkey_adj_val[i] + (*(p + i));
        }
    }
    if(tkey_val_clr_cnt == tkey_tkcnt_adj_times) {
        for(u8 i = 0; i < sizeof(tkey_array); i++) {
            tkey_adj_val[i] = tkey_adj_val[i] / tkey_tkcnt_adj_times;       //求平均
            tkey_press_cnt[i] = tkey_adj_val[i] + (tkey_adj_val[i] / TKEY_ADJ_LEVEL_MAX) * tkey_tkcnt_adj_level;   //设定灵敏度
            //my_printf(testa2,tkey_press_cnt[i]);
         }
    }
}
#endif // TKEY_TKCNT_ADJ_EN


//tkey中断
AT(.com_text.irq)
void tkey_isr(void)
{
    if(TKCON & BIT(16)){
        TKCON |= BIT(17);                      //clear pending
        TRACE(tk_test1,tk_buf[0]);
        TRACE(tk_test2,tk_buf[1]);
        TRACE(tk_test3,tk_buf[2]);
        TRACE(tk_test4,tk_buf[3]);
#if TKEY_TKCNT_ADJ_EN
        if(tkey_val_clr_cnt <tkey_tkcnt_adj_times) {
            tkey_val_clr_cnt++;
            tkey_val_clr((u32*)tk_buf);
        }
#endif // TKEY_TKCNT_ADJ_EN
    }
}

AT(.com_text.bsp.tkey)
u8 get_tkey(void)
{
    u8 value = 0;
    value = tkey_get_scan();
    return value;
}

void tkey_exit(void)
{
    printf("%s\n",__func__);
    for(u8 i = 0; i < sizeof(tkey_array); i++) {
        tkey_gpio_exit(tkey_array[i]);
    }
    tkey_off();
}

void tkey_init(void)
{
    printf("%s\n",__func__);

    for(u8 i = 0; i < sizeof(tkey_array); i++) {
        tkey_gpio_config(tkey_array[i]);
    }
    memset(tkey_press_cnt,0,sizeof(tkey_press_cnt));
    memset(tkey_adj_val,0,sizeof(tkey_adj_val));
    tk_cb_t tk_cfg;
    tk_cfg.tkcd_pr = TKEY_TKCD_PR;
    tk_cfg.tk_dmacnt = sizeof(tkey_array);
    tkey_init_process(&tk_cfg);
}

#endif // TKEY_MUL_SCAN_EN
