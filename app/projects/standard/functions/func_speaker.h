#ifndef _FUNC_SPEAKER_H
#define _FUNC_SPEAKER_H

#define SILENCE_THRESHOLD_POW       2000
#define NOISE_THRESHOLD_POW         4000

typedef struct {
    u8 pause;
    u8 rec_start;
    u8 mic_voice_changer_en;        //变音使能
    u8 mic_wsola_rec_en;            //录变速使能
    u8 mic_echo_en;                 //echo混响使能
    u8 mic_reverb_en;               //reverb使能
    u8 mic_dac_en;                  //推dac使能
    u8 mic_dnr_fre_en;              //dnr降噪使能
} func_speaker_t;
extern func_speaker_t f_spk;

typedef struct {
    u8  pause;
    u8  rec_start;
    u8  mode;               //0--播音乐，1--录音,开启mic能量检测，2--放音
    u8  mic_sta;            //状态
    u8  mic_sta_pre;        //上一个状态
    u8  noice_cnt;          //有声计数
    u8  silence_cnt;        //无声计数
    u16 mic_max_pow;        //能量值
    u8 vad_time_cnt;
    u8 record_tiem_cnt;
} func_tomcat_t;
extern func_tomcat_t f_tomcat;

//u16 ticks_vad;

typedef enum{
    MIC_SILENCE = 0x0,
    MIC_SOUND   = 0x1,
}func_tomcat_mic_sta;

void func_speaker(void);
void func_speaker_message(u16 msg);
void func_speaker_start(void);
void func_speaker_stop(void);
void func_speaker_mp3_res_play(u32 addr, u32 len);
void func_speaker_mp3_song_play(u32 addr, u32 len);
#endif
