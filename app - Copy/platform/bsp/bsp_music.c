#include "include.h"

#define FS_CRC_SEED         0xffff

#if WARNING_WSOLA_WAV_PLAY_EN
    #define MUL_LEN 4
#else
    #define MUL_LEN 2
#endif

msc_cb_t msc_cb AT(.music_buf);
msc_layer_t msc_layer AT(.music_buf);
music_layer_gain_t msc_gain AT(.music_buf);

#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
wav_layer_t wav_layer1 AT(.wav_buf);
u8 pcm_wav1_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
wav_layer_t wav_layer2 AT(.wav_buf);
u8 pcm_wav2_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
wav_layer_t wav_layer3 AT(.wav_buf);
u8 pcm_wav3_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) == (MSC_WAV_LAYER4))
wav_layer_t wav_layer4 AT(.wav_buf);
u8 pcm_wav4_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) == (MSC_WAV_LAYER5))
wav_layer_t wav_layer5 AT(.wav_buf);
u8 pcm_wav5_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) == (MSC_WAV_LAYER6))
wav_layer_t wav_layer6 AT(.wav_buf);
u8 pcm_wav6_obuf[WAV_BUF_LEN*MUL_LEN] AT(.wav_buf);
#endif

wav_layer_t *wav_layer_tbl[] = {
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
&wav_layer1,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
&wav_layer2,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
&wav_layer3,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) == (MSC_WAV_LAYER4))
&wav_layer4,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) == (MSC_WAV_LAYER5))
&wav_layer5,
#endif
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) == (MSC_WAV_LAYER6))
&wav_layer6,
#endif
};

#if WARNING_TONE_EN
u16 spi_mp3_encrypt_key = SPI_MP3_ENCRYPT_KEY;
#endif //WARNING_TONE_EN

#if FUNC_MUSIC_EN
//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev) {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif
    }

    f_msc.file_total = fs_get_total_files();
    if (!f_msc.file_total) {
        f_msc.dir_total = 0;
        return false;
    }

    if (new_dev) {
#if WARNING_USB_SD
        if (sys_cb.cur_dev <= DEV_SDCARD1) {
            mp3_res_play(RES_BUF_SDCARD_MODE_MP3, RES_LEN_SDCARD_MODE_MP3);
        } else if(sys_cb.cur_dev <= DEV_UDISK){
            mp3_res_play(RES_BUF_USB_MODE_MP3, RES_LEN_USB_MODE_MP3);
        }
#endif
    }

    f_msc.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

#if MUSIC_PLAYMODE_NUM
//music播放模式切换
void music_playmode_next(void)
{
    sys_cb.play_mode++;
    if (sys_cb.play_mode >= MUSIC_PLAYMODE_NUM) {
        sys_cb.play_mode = NORMAL_MODE;
    }
    //gui_box_show_playmode();
    //plugin_playmode_warning();
}
#endif // MUSIC_PLAYMODE_NUM

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (f_msc.brkpt_flag) {
        f_msc.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
        if (calc_crc(f_msc.fname, 8, FS_CRC_SEED) == f_msc.brkpt.fname_crc) {
            music_set_jump(&f_msc.brkpt);
        } else {
            clr_flag = 1;
        }
    }
    f_msc.brkpt.fname_crc = calc_crc(f_msc.fname, 8, FS_CRC_SEED);
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    if (clr_flag) {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&f_msc.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN
#endif // FUNC_MUSIC_EN


#if MUSIC_MODE_TABLE_EN
const msc_func_tbl_t msc_func = {
    {
        [MSC_MP3] = mp3_song_play,
        [MSC_VMP3] = vmp3_song_play,
        [MSC_WAV] = wav_song_play,
        [MSC_ESBC] = esbc_song_play,
#if MIDI_EN
        [MSC_MIDI] = midi_song_play,
        [MSC_VMIDI] = vmidi_song_play,
#endif
    },
    {
        [MSC_MP3] = mp3_play_end,
        [MSC_VMP3] = mp3_play_end,
        [MSC_WAV] = wav_play_end,
        [MSC_ESBC] = esbc_play_end,
#if MIDI_EN
        [MSC_MIDI] = midi_play_end,
        [MSC_VMIDI] = midi_play_end,
#endif
    },
    {
      //  [MSC_WAV] = wav_res_play_loop_back, /* v1.0目前只有wav支持无缝循环播放 */
    },
};

const msc_mode_tbl_t msc_mode_tbl[] = {
 /*
  * v1.0
  * 目前只支持MSC_MAX_MERGE_CNT=2，即同时播放两种格式的音频，且只支持[wav/adpcm]和[mp3/vmp3/midi/vmidi]的叠加播放
  * 需要先放[mp3/vmp3/midi/vmidi]的地址和长度等信息，再放[wav/adpcm]的地址和长度等信息
  * { { { [mp3/vmp3/midi/vmidi]地址,        [mp3/vmp3/midi/vmidi]长度,      音频格式类型,   音频播放选项 },
  *     { [wav/adpcm]地址,                  [wav/adpcm]长度,                音频格式类型,   音频播放选项 }, }, 播放完多少种音频之后切到下一个模式 },
  */
    { { { &RES_BUF_EN_POWEROFF_MP3,         &RES_LEN_EN_POWEROFF_MP3,       MSC_MP3,     MSC_OPT_NONE },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_WAV,     MSC_OPT_PLAY }, }, 1 },
    { { { &RES_BUF_EN_POWERON_MP3,          &RES_LEN_EN_POWERON_MP3,        MSC_MP3,     MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_WAV,     MSC_OPT_NONE }, }, 1 },
    { { { &RES_BUF_EN_POWEROFF_MP3,         &RES_LEN_EN_POWEROFF_MP3,       MSC_MP3,     MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_WAV,     MSC_OPT_PLAY }, }, 2 },
    { { { &RES_BUF_MID_DEMO_MIDI1_VMID,     &RES_LEN_MID_DEMO_MIDI1_VMID,   MSC_VMIDI,   MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_WAV,     MSC_OPT_PLAY }, }, 2 },
    { { { &RES_BUF_MID_DEMO_MIDI1_VMID,     &RES_LEN_MID_DEMO_MIDI1_VMID,   MSC_VMIDI,   MSC_OPT_PLAY },
        { &RES_BUF_EN_MAX_VOL_WAV,          &RES_LEN_EN_MAX_VOL_WAV,        MSC_WAV,     MSC_OPT_PLAY | MSC_OPT_LOOP }, }, 1 },
};

void music_mode_inc(void)
{
    msc_cb.msc_index++;
    if (msc_cb.msc_index >= msc_cb.msc_max) {
        msc_cb.msc_index = 0;
    }
}

void music_mode_auto_en(u8 enable)
{
    msc_cb.msc_auto_en = enable;
}

void music_mode_table_check(void)
{
    msc_mode_tbl_t tbl = msc_mode_tbl[msc_cb.msc_index];
    if (msc_cb.msc_auto_en && msc_cb.msc_index_back != msc_cb.msc_index) { /* 是否已切换到下一个模式 */
        tbl = msc_mode_tbl[msc_cb.msc_index];
        msc_cb.msc_index_back = msc_cb.msc_index;
        msc_cb.msc_playing_cnt = 0;
        msc_cb.msc_played_cnt = 0;
        printf("======================msc_cb.msc_index: %d\n", msc_cb.msc_index);
        for (int i=0; i<MSC_MAX_MERGE_CNT; i++) {
            msc_opt_e opt = tbl.o[i].opt;
            if (!(opt & MSC_OPT_PLAY)) {
                continue; /* 只有当选项里或上MSC_OPT_PLAY才会去播放 */
            }
            u8 mode = tbl.o[i].mode;
            if (opt & MSC_OPT_LOOP && msc_func.song_loop[mode]) {
                msc_func.song_loop[mode](1); /* 选项满足MSC_OPT_LOOP时才无缝循环播放 */
            }
            if (msc_func.song_play[mode]) {
                msc_func.song_play[mode](*tbl.o[i].ptr, *tbl.o[i].len); /* 播放当前音频 */
                msc_cb.msc_playing_cnt++;
            }
        }
    }
    if (msc_cb.msc_played_cnt >= tbl.played_cnt_next || msc_cb.msc_playing_cnt <= 0) { /* 是否满足切换到下一个模式的条件：播完所有音频，或播完指定数量的音频 */
        for (int i=0; i<MSC_MAX_MERGE_CNT; i++) { /* 确保退出当前模式所有播放 */
            msc_opt_e opt = tbl.o[i].opt;
            u8 mode = tbl.o[i].mode;
            if (opt & MSC_OPT_LOOP && msc_func.song_loop[mode]) {
                msc_func.song_loop[mode](0);
            }
            if (opt & MSC_OPT_PLAY && msc_func.song_play_end[mode]) {
                msc_func.song_play_end[mode]();
            }
        }
        music_mode_inc(); /* 切换到下一模式 */
    }
}
#endif

//音乐播放公共初始化
void music_res_init(void)
{
    memset(&msc_cb, 0, sizeof(msc_cb_t));
    memset(&msc_layer, 0, sizeof(msc_layer_t));
    memset(&msc_gain, 0, sizeof(music_layer_gain_t));

    msc_gain.layer0_gain = DIG_N0DB;
    msc_gain.layer1_gain = DIG_N0DB;
    msc_gain.layer2_gain = DIG_N0DB;
    msc_gain.layer3_gain = DIG_N0DB;
    msc_gain.layer4_gain = DIG_N0DB;
    msc_gain.layer5_gain = DIG_N0DB;
    msc_gain.layer6_gain = DIG_N0DB;

#if MUSIC_MODE_TABLE_EN
    msc_cb.msc_max = sizeof(msc_mode_tbl) / sizeof(msc_mode_tbl[0]);
    printf("msc_cb.msc_max: %d\n", msc_cb.msc_max);
    msc_cb.msc_index_back = 0xffff;
    msc_cb.msc_auto_en = 1;
#endif
}

//进入音乐播放公共
void music_mode_enter(msc_layer_e layer_num, msc_mode_e mode)
{
    if(!music_layer_play_check_all()){
#if !(FUNC_SPEAKER_EN || FUNC_USBDEV_EN || FUNC_MUSIC_EN)
        dac_classd_analog_on();             //打开dac模拟
#endif
    }
    if(get_music_dec_sta() != MUSIC_STOP && (MSC_LAYER0 == layer_num)) {
        music_control(MUSIC_MSG_STOP);
    }
    switch(mode){
        case MSC_LSBC:
            #if (WARNING_LSBC_PLAY)
            if(MSC_LAYER0 == layer_num){
                msc_gain.layer0_gain = DIG_N0DB;
                set_layer0_fade(0, 0, 0, 0);       //lsbc软件淡入淡出
            }
            #endif
            break;
        case MSC_WAV:
            if(MSC_LAYER1 == layer_num){
                #if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
                wav_layer1.pcm_obuf = pcm_wav1_obuf;
                msc_gain.layer1_gain = DIG_N0DB;
                set_layer1_fade(0, 0, 0, 1);
                #endif
            }else if(MSC_LAYER2 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
                wav_layer2.pcm_obuf = pcm_wav2_obuf;
#endif
                msc_gain.layer2_gain = DIG_N0DB;
            }else if(MSC_LAYER3 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
                wav_layer3.pcm_obuf = pcm_wav3_obuf;
#endif
                msc_gain.layer3_gain = DIG_N0DB;
            }else if(MSC_LAYER4 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) == (MSC_WAV_LAYER4))
                wav_layer4.pcm_obuf = pcm_wav4_obuf;
#endif
                msc_gain.layer4_gain = DIG_N0DB;
            }else if(MSC_LAYER5 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) == (MSC_WAV_LAYER5))
                wav_layer5.pcm_obuf = pcm_wav5_obuf;
#endif
                msc_gain.layer5_gain = DIG_N0DB;
            }else if(MSC_LAYER6 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) == (MSC_WAV_LAYER6))
                wav_layer6.pcm_obuf = pcm_wav6_obuf;
#endif
                msc_gain.layer6_gain = DIG_N0DB;
            }
            break;
        case MSC_MIDI:
        case MSC_VMIDI:
            #if MIDI_EN
            msc_gain.layer0_gain = DIG_N0DB;
            set_layer0_fade(0, 0, 0, 0);          //midi软件淡入淡出
            if(get_msc_layer_mode(MSC_LAYER0) != MSC_MIDI){
                midi_init();
            }
            #endif
            break;
        case MSC_MP3:
        case MSC_VMP3:
            #if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
            msc_gain.layer0_gain = DIG_N0DB;
            set_layer0_fade(0, 0, 0, 1);
            #endif
            break;
        case MSC_ESBC:
            #if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
            msc_gain.layer0_gain = DIG_N0DB;
            #endif
            break;
        default:
            break;
    }
    music_layer_mode_set(layer_num, mode);
    music_layer_sta_set(layer_num, LAYER_PLAYING);
    sys_clk_req(INDEX_DECODE, SYS_128M);
}

//退出音乐播放公共
void music_mode_exit(msc_layer_e layer_num, msc_mode_e mode)
{
//当需要增加音频播放退出时的处理,在此增加
    switch(mode){
        case MSC_LSBC:
            #if (WARNING_LSBC_PLAY)
            #endif
            break;
        case MSC_WAV:
            #if (WARNING_WAV_PLAY || EXSPI_WAV_EN)
            #endif
            break;
        case MSC_MIDI:
        case MSC_VMIDI:
            #if MIDI_EN
            // midi_mode_exit();
            #endif
            break;
        case MSC_MP3:
        case MSC_VMP3:
            #if (WARNING_MP3_PLAY || WARNING_VMP3_PLAY || EXSPI_MP3_EN)
            #endif
            break;
        case MSC_ESBC:
            #if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
            #endif
            break;
        default:
            break;
    }
    music_layer_mode_clr(layer_num);
    music_layer_sta_set(layer_num, LAYER_STOP);
    if (!music_layer_play_check_all()) {
        dac_dc_exp_off();
#if (!(FUNC_SPEAKER_EN || FUNC_USBDEV_EN || FUNC_MUSIC_EN) && DAC_CLASSD_EN)
        dac_classd_analog_off();            //无音乐时关闭dac模拟
#endif
        sys_clk_set(SYS_CLK_SEL);
    }
}

//设置某layer层当前解码的模式
bool music_layer_mode_set(msc_layer_e layer_num, msc_mode_e mode)
{
    if(layer_num >= MSC_TOTAL_LAYER){
        return false;
    }
    if(mode != msc_layer.layer_mode[layer_num]){
        msc_layer.layer_mode[layer_num] = mode;
    }
    return true;
}

//清空某layer层当前解码的模式
bool music_layer_mode_clr(msc_layer_e layer_num)
{
    if(layer_num >= MSC_TOTAL_LAYER){
        return false;
    }
    msc_layer.layer_mode[layer_num] = 0;
    return true;
}

//获取当前layer播放的音频类型
msc_mode_e get_msc_layer_mode(u8 layer_num)
{
    msc_mode_e layer_tmp;
    layer_tmp = msc_layer.layer_mode[layer_num];
    return layer_tmp;
}

//检查某layer层某格式音频是否在播
AT(.music_text)
u8 layer_mode_check(msc_layer_e layer_num, msc_mode_e mode)
{
    if(((msc_layer.layer_mode[layer_num] & mode) == mode) && (msc_layer.layer_sta[layer_num] != LAYER_STOP)){
        return 1;
    }
    return 0;
}

//检查所有layer层是否有音频在播
AT(.music_text)
u8 music_layer_play_check_all(void)
{
    u8 msc_tmp = 0;
    for(u8 i = 0; i < MSC_TOTAL_LAYER; i++){
        if(msc_layer.layer_sta[i] == LAYER_PLAYING || msc_layer.layer_sta[i] == LAYER_PAUSE){
            msc_tmp = 1;
        }
    }
    return msc_tmp;
}

//检查某layer层是否有音频在播
layer_sta_e layer_play_check(msc_layer_e layer_num)
{
    if ((msc_layer.layer_sta[layer_num]) != LAYER_STOP) {
        return 1;
    }
    return 0;
}

//检查某layer1~n层是否有音频在播
layer_sta_e layer_n_play_check(void)
{
    for(u8 i = MSC_LAYER1; i < MSC_TOTAL_LAYER; i++){
        if((msc_layer.layer_sta[i]) != LAYER_STOP){
            return 1;
        }
    }
    return 0;
}

//控制song播放某layer层的播放暂停
void music_layer_sta_set(msc_layer_e layer_num, layer_sta_e layer_sta)
{
    if(msc_layer.layer_sta[layer_num] != layer_sta){
        msc_layer.layer_sta[layer_num] = layer_sta;
        if(msc_layer.layer_sta[layer_num] == LAYER_PAUSE || msc_layer.layer_sta[layer_num] == LAYER_STOP){
            if(MSC_LAYER0 == layer_num){
                // set_layer0_fade(0, 10, 1, 0);    //设置1ms后淡出
            }else if(MSC_LAYER1 == layer_num){
                // set_layer1_fade(0, 10, 1, 0);    //设置1ms后淡出
            }
        }else if(msc_layer.layer_sta[layer_num] == LAYER_PLAYING){
            if(MSC_LAYER0 == layer_num){
                set_layer0_fade(0, 0, 0, 0);     //清空淡入淡出参数
            }else if(MSC_LAYER1 == layer_num){
                set_layer1_fade(0, 0, 0, 0);     //清空淡入淡出参数
            }
        }
    }
}

//获取某layer层播放状态
layer_sta_e music_layer_sta_get(msc_layer_e layer_num)
{
    return msc_layer.layer_sta[layer_num];
}

#if VMP3_BREAK_POINT_EN
static u32 vmp3_break_point_offset = 0;

u32 get_mp3_bytes_by_second(u16 second);

void vmp3_break_point_save(void)
{
    static u32 write_ticks_addr_pos = 0;
    u32 pos = 0;
    u32 temp;
    if (tick_check_expire(write_ticks_addr_pos, 100 * VMP3_BREAK_POINT_SAVE_PORIOD)) {
        temp = get_mp3_bytes_by_second(VMP3_BREAK_POINT_BACK_SECOND);
        write_ticks_addr_pos = tick_get();
        pos = vmp3_break_point_offset + spires_get_pos();
        if (pos > temp) {
            param_vmp3_break_pos_write(pos - temp);
        } else {
            param_vmp3_break_pos_write(0);
        }
        param_sync();
    }
}

u32 vmp3_break_point_restore(u32 addr)
{
    u32 offset = 0;
    if (addr == param_vmp3_break_addr_read()) {
        offset = param_vmp3_break_pos_read();
    }
    param_vmp3_break_addr_write(addr);
    param_sync();
    return offset;
}
#endif

#if WARNING_TONE_EN

#if WARNING_PAUSE_PLAY
typedef struct {
    bool pause_flag;
    u32 addr;
    u32 len;
    u32 pos;
    void (*music_res_play)(u32 addr, u32 len);
}pause_save_t;
pause_save_t pause_save;

void music_res_pause(u32 addr, u32 len, void *res_play_func)
{
    pause_save_t *p = &pause_save;
    p->addr = addr;
    p->len = len;
    p->pos = spires_get_pos();
    p->music_res_play = res_play_func;
}

void music_res_continue(void)
{
    pause_save_t *p = &pause_save;
    if(p->pos) {
        p->pause_flag = true;
        p->music_res_play(p->addr, p->len);
    }
}

static void music_res_jump(void)
{
    pause_save_t *p = &pause_save;
    if (p->pause_flag) {
        spires_load_pos(p->pos);
        memset(&pause_save, 0, sizeof(pause_save_t));
    }
}
#endif // WARNING_PAUSE_PLAY

void mp3_res_play_do(u32 addr, u32 len, bool sync)
{
#if WARNING_MP3_PLAY
    u16 msg;

    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
#if VMP3_BREAK_POINT_EN
    vmp3_break_point_offset = vmp3_break_point_restore(addr);
    addr += vmp3_break_point_offset;
    len -= vmp3_break_point_offset;
#endif
    mp3_res_play_kick(addr, len,0);
#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER0)) {
        WDT_CLR();
#if VMP3_BREAK_POINT_EN
        vmp3_break_point_save();
#endif
        msg = msg_dequeue();
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用music_res_continue恢复提示音播放
            music_res_pause(addr, len, mp3_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    mp3_res_play_exit();
#endif
}

void vmp3_res_play_do(u32 addr, u32 len, bool sync)
{
#if WARNING_VMP3_PLAY
    u16 msg;

    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
#if VMP3_BREAK_POINT_EN
    vmp3_break_point_offset = vmp3_break_point_restore(addr);
    addr += vmp3_break_point_offset;
    len -= vmp3_break_point_offset;
#endif
    vmp3_res_play_kick(addr, len,0);
#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER0)) {
        WDT_CLR();
#if VMP3_BREAK_POINT_EN
        vmp3_break_point_save();
#endif
        msg = msg_dequeue();
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用music_res_continue恢复提示音播放
            music_res_pause(addr, len, vmp3_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    vmp3_res_play_exit();
#endif
}

void esbc_res_play_do(u32 addr, u32 len, bool sync)
{
#if WARNING_ESBC_PLAY
    u16 msg;
    u8 *ptr = (u8 *)addr;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0 || ptr[0] != 0x8B) {
        printf("esbc format err\n");
        return;
    }
    if (get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }

    bsp_change_volume(WARNING_VOLUME);
    esbc_res_play_kick(addr, len, 0);
    while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER0)) {
        WDT_CLR();
        msg = msg_dequeue();
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    esbc_res_play_exit();
#endif
}

void esbc_link_play_do(const esbc_link_t *elink, const u8 e_num)
{
#if WARNING_ESBC_PLAY
    u16 msg;
    u8 i, *ptr;
    u32 *paddr, *plen;
    for(i = 0; i < e_num; i++){
        ptr  = (u8 *)(*elink[i].addr);
        plen = (u32 *)(*elink[i].len);
        if (plen == 0 || ptr[0] != 0x8B) {
            return;
        }
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    for(i = 0; i < e_num; i++){
        paddr = (u32 *)(*elink[i].addr);
        plen  = (u32 *)(*elink[i].len);
        esbc_res_loop_play_kick((u32)paddr, (u32)plen, elink[i].sta_skip_frame, elink[i].end_skip_frame, 0);
        while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER0)) {
            WDT_CLR();
            msg = msg_dequeue();
            if (msg != NO_MSG) {
                msg_enqueue(msg);       //还原未处理的消息
            }
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    esbc_res_play_exit();
#endif
}

void lsbc_res_play_do(u32 addr, u32 len, bool sync)
{
#if WARNING_LSBC_PLAY
    u16 msg;
    u8 *ptr = (u8 *)addr;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0 || ptr[0] != 0x69) {
        printf("Lsbc format err\n");
        return;
    }
    if (get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    lsbc_res_play_kick(addr, len,0);
    //lsbc_res_loop_play_kick(addr, len, 0, 0, false);
    while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER0)) {
        WDT_CLR();
        msg = msg_dequeue();
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    lsbc_res_play_exit();
#endif
}

void wav_res_play_do(u32 addr, u32 len, bool sync)
{
#if WARNING_WAV_PLAY
    u16 msg;
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    wav_layer1.wav_layer = MSC_LAYER1;
    wav_res_play_kick(addr, len, &wav_layer1,0);
    music_control(MUSIC_MSG_PLAY);
#if WARNING_PAUSE_PLAY
    music_res_jump();
#endif
    while (get_music_dec_sta() != MUSIC_STOP && layer_play_check(MSC_LAYER1)) {
        WDT_CLR();
        muisc_decode_process(wav_layer_tbl);
        msg = msg_dequeue();
#if WARNING_PAUSE_PLAY
        if (msg == KU_PLAY_POWER) {     //此处可自定义msg暂停提示音播放并回到主循环，调用mp3_res_continue恢复提示音播放
            music_res_pause(addr, len, wav_res_play);
            msg_enqueue(msg);
            break;
        }
#endif
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    wav_res_play_exit();
#endif
}

#else
void mp3_res_play_do(u32 addr, u32 len, bool sync){}
void esbc_res_play_do(u32 addr, u32 len, bool sync){}
void esbc_link_play_do(const esbc_link_t *elink, const u8 e_num){}
void wav_res_play_do(u32 addr, u32 len, bool sync){}
void lsbc_res_play_do(u32 addr, u32 len, bool sync){}
#endif //WARNING_TONE_EN

//mp3/vmp3播放结束回调接口
void mp3_play_end(void)
{
    printf("mp3_play_end\n");
#if WAV_PITCHUP_EN
    wav_pitch_up_init(); // 播放MP3之后变调需要重新初始化
#endif
#if REC_PLAY_MODE_SEL && FUNC_REC_EN
    if(f_spk.pause == 1 && func_cb.sta == FUNC_SPEAKER){
        func_speaker_start();
        #if SPK_MIC_DNR_FRE_EN
        bsp_dnr_fre_enable();
        #endif
    }
    if (func_cb.sta != FUNC_EXSPIFLASH_MUSIC) {
        register_spi_read_function(NULL);
    }
    register_rec_read_function(NULL);
#endif
    mp3_dec_stop();
    music_mode_exit(MSC_LAYER0, MSC_MP3);
}

//wav/adpcm播放结束回调接口
void wav_play_end(msc_layer_e layer_num)
{
    printf("wav_play_end, layer: %d\n", layer_num);
#if (WARNING_WAV_PLAY)
    if(MSC_LAYER1 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
        wav_dec_stop(&wav_layer1);
        music_mode_exit(MSC_LAYER1, MSC_WAV);
#endif
    }else if(MSC_LAYER2 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
        wav_dec_stop(&wav_layer2);
        music_mode_exit(MSC_LAYER2, MSC_WAV);
#endif
    }else if(MSC_LAYER3 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
        wav_dec_stop(&wav_layer3);
        music_mode_exit(MSC_LAYER3, MSC_WAV);
#endif
    }else if(MSC_LAYER4 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) == (MSC_WAV_LAYER4))
        wav_dec_stop(&wav_layer4);
        music_mode_exit(MSC_LAYER4, MSC_WAV);
#endif
    }else if(MSC_LAYER5 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) == (MSC_WAV_LAYER5))
        wav_dec_stop(&wav_layer5);
        music_mode_exit(MSC_LAYER5, MSC_WAV);
#endif
    }else if(MSC_LAYER6 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) == (MSC_WAV_LAYER6))
        wav_dec_stop(&wav_layer6);
        music_mode_exit(MSC_LAYER6, MSC_WAV);
#endif
    }
#endif
}

//Lsbc播放结束回调接口
void lsbc_play_end(void)
{
    printf("lsbc_play_end\n");
#if (WARNING_LSBC_PLAY)
    music_mode_exit(MSC_LAYER0, MSC_LSBC);
#endif
}

//esbc播放结束回调接口
void esbc_play_end(void)
{
    printf("esbc_play_end\n");
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
    music_mode_exit(MSC_LAYER0, MSC_ESBC);
#endif
}

//mp3提示音播放接口
void mp3_res_play(u32 addr, u32 len)
{
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    mp3_res_play_do(addr, len,0);
    music_mode_exit(MSC_LAYER0, MSC_MP3);
}

//vmp3提示音播放接口
void vmp3_res_play(u32 addr, u32 len)
{
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    vmp3_res_play_do(addr, len,0);
    music_mode_exit(MSC_LAYER0, MSC_MP3);
}

//esbc播放接口
void esbc_res_play(u32 addr, u32 len)
{
    music_mode_enter(MSC_LAYER0, MSC_ESBC);
    esbc_res_play_do(addr, len, 0);
    music_mode_exit(MSC_LAYER0, MSC_ESBC);
}

//lsbc播放接口
void lsbc_res_play(u32 addr, u32 len)
{
    music_mode_enter(MSC_LAYER0, MSC_LSBC);
    lsbc_res_play_do(addr, len, 0);
    music_mode_exit(MSC_LAYER0, MSC_LSBC);
}

//wav提示音播放接口
void wav_res_play(u32 addr, u32 len)
{
    music_mode_enter(MSC_LAYER1, MSC_WAV);
    wav_res_play_do(addr, len, 0);
    music_mode_exit(MSC_LAYER1, MSC_WAV);
}

#if MUSIC_DECODE_BK_EN
//mp3播放接口
void mp3_song_play(u32 addr, u32 len,u8 loop_cnt)
{
    printf("%s: addr: %x, len: %x , loop_cnt: %d\n", __func__, addr, len,loop_cnt);
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    mp3_res_play_kick(addr, len,loop_cnt);
}

//vmp3播放接口
void vmp3_song_play(u32 addr, u32 len,u8 loop_cnt)
{
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_MP3);
    vmp3_res_play_kick(addr, len,loop_cnt);
}

//esbc播放接口
void esbc_song_play(u32 addr, u32 len,u8 loop_cnt)
{
#if (WARNING_ESBC_PLAY || EXSPI_ESBC_EN)
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_ESBC);
    esbc_res_play_kick(addr, len, loop_cnt);
#endif
}

//lsbc播放接口
void lsbc_song_play(u32 addr, u32 len, u8 loop_cnt)
{
#if (WARNING_LSBC_PLAY)
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_LSBC);
    lsbc_res_play_kick(addr, len,loop_cnt);
#endif
}

//wav/adpcm播放接口
void wav_song_play(msc_layer_e layer_num, u32 addr, u32 len,u8 loop_cnt)
{
#if (WARNING_WAV_PLAY)
    printf("%s: addr: %x, len: %x, layer: %d , loop_cnt: %d\n", __func__, addr, len, layer_num, loop_cnt);
    music_mode_enter(layer_num, MSC_WAV);
    if(MSC_LAYER1 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER1) == (MSC_WAV_LAYER1))
        wav_layer1.wav_layer = MSC_LAYER1;
        wav_res_play_kick(addr, len, &wav_layer1,loop_cnt);
#endif
    }else if(MSC_LAYER2 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER2) == (MSC_WAV_LAYER2))
        wav_layer2.wav_layer = MSC_LAYER2;
        wav_res_play_kick(addr, len, &wav_layer2,loop_cnt);
#endif
    }else if(MSC_LAYER3 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER3) == (MSC_WAV_LAYER3))
        wav_layer3.wav_layer = MSC_LAYER3;
        wav_res_play_kick(addr, len, &wav_layer3,loop_cnt);
#endif
    }else if(MSC_LAYER4 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER4) == (MSC_WAV_LAYER4))
        wav_layer4.wav_layer = MSC_LAYER4;
        wav_res_play_kick(addr, len, &wav_layer4,loop_cnt);
#endif
    }else if(MSC_LAYER5 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER5) == (MSC_WAV_LAYER5))
        wav_layer5.wav_layer = MSC_LAYER5;
        wav_res_play_kick(addr, len, &wav_layer5,loop_cnt);
#endif
    }else if(MSC_LAYER6 == layer_num){
#if ((MUISC_WAV_LAYER_EN & MSC_WAV_LAYER6) == (MSC_WAV_LAYER6))
        wav_layer6.wav_layer = MSC_LAYER6;
        wav_res_play_kick(addr, len, &wav_layer6,loop_cnt);
#endif
    }
#endif
}

wav_layer_t *get_wav_layer1_ptr(void)
{
    return &wav_layer1;
}
#endif

#if 0   //esbc拼接播放测试示例
#define MSC_ESBC_LINK1_NUM  3
esbc_link_t msc_esbc_link1[MSC_ESBC_LINK1_NUM] = {
    {&RES_BUF_SBC_K003_SBC, &RES_LEN_SBC_K003_SBC, 5, 50},
    {&RES_BUF_SBC_K004_SBC, &RES_LEN_SBC_K004_SBC, 5, 50},
    {&RES_BUF_SBC_K005_SBC, &RES_LEN_SBC_K005_SBC, 5, 50},
};
void esbc_link_test(void)
{
    esbc_link_play_do(msc_esbc_link1, MSC_ESBC_LINK1_NUM);
}
#endif
