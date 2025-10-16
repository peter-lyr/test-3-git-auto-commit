#ifndef __PREVENT_HOWLING_H
#define __PREVENT_HOWLING_H

typedef struct{
	u8  shift_bit;
	u16 smooth_len;
	s32 fs;
}allp_freq_t;

typedef struct {
	s32 change_range;
	u8  mode;
	s32 fs;
	s16 freq_step;
} angle_change_cb_t;

//allpass_filter_change算法结构体以及相关API声明
void allpass_filter_change_init(u8 idx, s32 change_time, allp_freq_t *p_voice);
s32 allpass_filter_change(s32 input, u8 idx);
void allpass_filter_change_set(u8 idx, s32 change_time);
void lock_code_apf_chg(void);

void bsp_angle_change_init(void);
void bsp_freq_shift_init(void);

#endif
