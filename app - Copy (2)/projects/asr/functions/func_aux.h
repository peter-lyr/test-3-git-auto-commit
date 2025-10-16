#ifndef _FUNC_AUX_H
#define _FUNC_AUX_H

typedef struct {
    u8  pause       : 1,
        start       : 1;
    u8 rec_start;
} func_aux_t;
extern func_aux_t f_aux;

void func_aux_pause_play(void);
void func_aux_message(u16 msg);
void func_aux(void);
bool aux_is_start(void);
void aux_sdadc_process(u8 *ptr,u32 samples,int ch_mode);

#endif // _FUNC_AUX_H
