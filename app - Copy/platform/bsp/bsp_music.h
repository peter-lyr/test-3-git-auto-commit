#ifndef _BSP_MUSIC_H
#define _BSP_MUSIC_H

enum {
    NORMAL_MODE,
    SINGLE_MODE,
    FLODER_MODE,
    RANDOM_MODE,
};

bool pf_scan_music(u8 new_dev);
void music_playmode_next(void);
#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_init(void);
void bsp_music_breakpoint_save(void);
void bsp_music_breakpoint_clr(void);

#define music_breakpoint_init()   bsp_music_breakpoint_init()
#define music_breakpoint_save()   bsp_music_breakpoint_save()
#define music_breakpoint_clr()    bsp_music_breakpoint_clr()
#else
#define music_breakpoint_init()
#define music_breakpoint_save()
#define music_breakpoint_clr()
#endif // MUSIC_BREAKPOINT_EN


#define MSC_MAX_MERGE_CNT 2 /* 最多同时播多少种音频 */
#define MSC_MODE_MAX_CNT  8 /* 随msc_mode_e的数量改变 */

#define MSC_WAV_LAYER1      BIT(0)      //占用data空间约为824byte
#define MSC_WAV_LAYER2      BIT(1)      //占用data空间约为824byte
#define MSC_WAV_LAYER3      BIT(2)      //占用data空间约为824byte
#define MSC_WAV_LAYER4      BIT(3)      //占用data空间约为824byte
#define MSC_WAV_LAYER5      BIT(4)      //占用data空间约为824byte
#define MSC_WAV_LAYER6      BIT(5)      //占用data空间约为824byte
#define MSC_WAV_SEL         (MSC_WAV_LAYER1 | MSC_WAV_LAYER2)

typedef struct{
    u32 *addr;
    u32 *len;
    u16 sta_skip_frame;     //开头跳过的帧数
    u16 end_skip_frame;     //末尾跳过的帧数
}esbc_link_t;

typedef struct{
    layer_sta_e layer_sta[MSC_TOTAL_LAYER];
    msc_mode_e layer_mode[MSC_TOTAL_LAYER];
}msc_layer_t;

typedef struct {
    msc_mode_e msc_mode;
#if MUSIC_MODE_TABLE_EN
    u8 msc_auto_en;                 /* 是否自动切换下一播放模式 */
    s8 msc_playing_cnt;             /* 当前正在播的音频数量 */
    s8 msc_played_cnt;              /* 已经播完的音频数量 */
    u16 msc_index;                  /* 当前播放模式的索引 */
    u16 msc_index_back;             /* 当前播放模式的备份索引 */
    u16 msc_max;                    /* 一共多少种播放模式 */
#endif
} msc_cb_t;

typedef enum {
    MSC_OPT_NONE = 0,               /* 仅用作占位 */
    MSC_OPT_PLAY = BIT(0),          /* 是否播放 */
    MSC_OPT_LOOP = BIT(1),          /* 是否无缝循环播放 */
} msc_opt_e;                        /* 播放模式针对每一种格式的音频的选项 */

typedef struct {
    u32 *ptr;                       /* 音频地址指针 */
    u32 *len;                       /* 音频长度指针 */
    msc_mode_e mode;                /* 音频格式类型，见msc_mode_e */
    msc_opt_e  opt;                 /* 当前音频播放选项 */
} msc_opt_t;

typedef struct {
    msc_opt_t o[MSC_MAX_MERGE_CNT]; /* 当前模式播放的多个音频的信息 */
    u8 played_cnt_next;             /* 当播完多少音频后切换到下一模式 */
} msc_mode_tbl_t;

typedef struct {
    void (*song_play[MSC_MODE_MAX_CNT])(u32 addr, u32 len);
    void (*song_play_end[MSC_MODE_MAX_CNT])(void);
    void (*song_loop[MSC_MODE_MAX_CNT])(u8 enable);
} msc_func_tbl_t;

uint calc_crc(void *buf, uint len, uint seed);
bool mp3_dec_frame(void);
bool vsbc_dec_frame(u16 in);
bool wav_dec_frame(void);
bool wav_res_decode(wav_layer_t *wav_layer);
bool drc_v3_init(const void *bin, int bin_size);
void test_wav_mp3_merge(void);

void music_res_init(void);                                          //音乐播放公共初始化
void music_mode_enter(msc_layer_e layer_num, msc_mode_e mode);      //进入音乐播放公共
void music_mode_exit(msc_layer_e layer_num, msc_mode_e mode);       //退出音乐播放公共

bool music_layer_mode_set(msc_layer_e layer_num, msc_mode_e mode);
bool music_layer_mode_clr(msc_layer_e layer_num);
msc_mode_e get_msc_layer_mode(u8 layer_num);                        //获取当前layer播放的音频类型
u8 music_layer_play_check_all(void);                                //检查所有layer层是否有音频在播
u8 layer_mode_check(msc_layer_e layer_num, msc_mode_e mode);        //检查某layer层某格式音频是否在播
layer_sta_e layer_play_check(msc_layer_e layer_num);                //检查某layer层是否有音频在播
layer_sta_e layer_n_play_check(void);                               //检查某layer1~n层是否有音频在播
void music_layer_sta_set(msc_layer_e layer_num, layer_sta_e layer_sta); //控制song播放某layer层的播放暂停
layer_sta_e music_layer_sta_get(msc_layer_e layer_num);             //获取某layer层播放状态

void mp3_res_play_do(u32 addr, u32 len, bool sync);
void vmp3_res_play_do(u32 addr, u32 len, bool sync);
void wav_res_play_do(u32 addr, u32 len, bool sync);
void esbc_res_play_do(u32 addr, u32 len, bool sync);

void mp3_res_play(u32 addr, u32 len);                               //提示音独占播放:mp3播放接口
void vmp3_res_play(u32 addr, u32 len);                              //提示音独占播放:vmp3播放接口
void wav_res_play(u32 addr, u32 len);                               //提示音独占播放:wav播放接口
void esbc_res_play(u32 addr, u32 len);                              //提示音独占播放:esbc播放接口
void lsbc_res_play(u32 addr, u32 len);                              //提示音独占播放:lsbc播放接口

void mp3_song_play(u32 addr, u32 len,u8 loop_cnt);                              //后台解码播放:mp3播放接口 loop_cnt为循环次数，不需要循环置0即可
void vmp3_song_play(u32 addr, u32 len,u8 loop_cnt);                             //后台解码播放:vmp3播放接口
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len,u8 loop_cnt);       //后台解码播放:wav播放接口
void esbc_song_play(u32 addr, u32 len,u8 loop_cnt);                             //后台解码播放:esbc播放接口
void lsbc_song_play(u32 addr, u32 len,u8 loop_cnt);                             //后台解码播放:lsbc播放接口

void esbc_link_play_do(const esbc_link_t *elink, const u8 e_num);

void mp3_play_end(void);                                            //mp3/vmp3播放结束回调接口
void wav_play_end(msc_layer_e layer_num);                           //wav/adpcm播放结束回调接口
void esbc_play_end(void);                                           //esbc播放结束回调接口
void midi_play_end(void);                                           //midi播放结束回调接口
void lsbc_play_end(void);                                           //lsbc播放结束回调接口

void music_mode_table_check(void);
void music_stop();

void music_res_pause(u32 addr, u32 len, void *res_play_func);
void music_res_continue(void);
#endif // _BSP_MUSIC_H
