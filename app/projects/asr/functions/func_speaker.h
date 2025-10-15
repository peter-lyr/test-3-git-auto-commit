#ifndef _FUNC_SPEAKER_H
#define _FUNC_SPEAKER_H

typedef struct {
    u8 pause;
    u8 rec_start;
} func_speaker_t;
extern func_speaker_t f_spk;

void func_speaker(void);
void func_speaker_message(u16 msg);
void func_speaker_start(void);
void func_speaker_stop(void);
void func_speaker_mp3_res_play(u32 addr, u32 len);
#endif
