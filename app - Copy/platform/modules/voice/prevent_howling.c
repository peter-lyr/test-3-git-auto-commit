#include "include.h"
#include "prevent_howling.h"

void angle_change_init(angle_change_cb_t *angle_change_cb);

/*
 * 文件名称: prevent_howling.c
 * 功能描述: 本文件为防啸叫处理模块

 ****************************************************************************************
    code :
    buf  : 10000*2 Bytes
    time : 160M 下 120 个点处理 147us
*/

allp_freq_t allp_freq;

#define CLIP(x,n)   (x>=(1<<n)) ? (1<<n)-1 : (x<(-(1<<n))) ? -(1<<n) : x
void allpass_filter_change_set(u8 idx, s32 change_time)
{
    lock_code_apf_chg();
    allp_freq.fs = 16000;
    allp_freq.shift_bit = 8;
    allp_freq.smooth_len = 256;
    allpass_filter_change_init(idx, change_time, &allp_freq);
}


//默认不用这个接口过，用跟随drc一起过
AT(.text.apf_chg_proc)
void allpass_test(s16 *ptr,u8 samples)
{
    s32 sample_32 =0;
    for(u8 j=0; j<samples;j++) {
        sample_32 = ptr[j];
        sample_32 = allpass_filter_change(sample_32, 0);
		sample_32 = CLIP(sample_32, 15);
		ptr[j] = sample_32;
    }
}

void bsp_angle_change_init(void)
{
    angle_change_cb_t angle_change_cb;
    memset(&angle_change_cb, 0, sizeof(angle_change_cb_t));

    angle_change_cb.change_range = 25736;
    angle_change_cb.mode		 = 2;
    angle_change_cb.fs			 = 16000;
    angle_change_cb.freq_step	 = 50;
    // my_printf("angle_change_cb.fs:%d\n",angle_change_cb.fs);
    angle_change_init(&angle_change_cb);
}

void bsp_freq_shift_init(void)
{
    holwing_ex_init(16000,14,7);
}
