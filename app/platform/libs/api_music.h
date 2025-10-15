#ifndef _API_MUSIC_H
#define _API_MUSIC_H

#define SEEK_SET    0
#define SEEK_CUR    1

#define AVFMT_INVALID              0                        //音频格式无效
#define AVFMT_CORRECT              BIT(0)                   //音频格式有效
#define AVFMT_NOT_SUPPORT         (BIT(0) | BIT(1))         //音频格式有效，但超出解码能力

#define WAV_BUF_LEN                 128     //wav decode samples
#define ADPCM_READ_LEN              64
#define WAV_BUF_SIZE                (WAV_BUF_LEN*4)
#define PEEK_SIZE                   44

typedef enum{
    MSC_LAYER0,
    MSC_LAYER1,
    MSC_LAYER2,
    MSC_LAYER3,
    MSC_LAYER4,
    MSC_LAYER5,
    MSC_LAYER6,
    MSC_TOTAL_LAYER,
}msc_layer_e;

typedef enum{
    LAYER_STOP,
    LAYER_PAUSE,
    LAYER_PLAYING,
}layer_sta_e;

typedef enum{
    B_MP3,
    B_VMP3,
    B_LSBC,
    B_ESBC,
    B_MIDI,
    B_VMIDI,
    B_WAV,
    MSC_TOTAL_TYPE,
}msc_type_e;

typedef enum {
    MSC_NONE,
    MSC_MP3     = BIT(B_MP3),
    MSC_VMP3    = BIT(B_VMP3),
    MSC_LSBC    = BIT(B_LSBC),
    MSC_ESBC    = BIT(B_ESBC),
    MSC_MIDI    = BIT(B_MIDI),
    MSC_VMIDI   = BIT(B_VMIDI),
    MSC_WAV     = BIT(B_WAV),
    MSC_TOTAL   = MSC_TOTAL_TYPE,
} msc_mode_e;

typedef enum {
    NONE_TYPE = 0,
    WAV_TYPE,
    MP3_TYPE,
    VSBC_TYPE,
    MIDI_TYPE,
    ESBC_TYPE,
    WSBC_TYPE,
    LSBC_TYPE,
} enum_msc_type_t;

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

enum {
    //decode msg
    MUSIC_MSG_STOP = 0,
    MUSIC_MSG_PAUSE,
    MUSIC_MSG_PLAY,
    MUSIC_MSG_FRAME,

    //encode msg
    ENC_MSG_MP3 = 32,
    ENC_MSG_AEC,
    ENC_MSG_ALC,
    ENC_MSG_PLC,
    ENC_MSG_SBC,
    ENC_MSG_WAV,            //pcm wave
    ENC_MSG_ADPCM,          //adpcm-ima wave
    ENC_MSG_SPDIF,
    ENC_MSG_I2S,
};

//codec common info
typedef struct  {
    u32 frame_count;                //current frame count
    u32 file_ptr;                   //file ptr of current frame
    u16 samples;                    //sample pionts of frame
    u8  spr;                        //sample rate of enum number
    u32 sample_rate;                //sample rate
    u32 bitrate;                    //bit rate
} codec_info_t;
extern codec_info_t *codec_info;

typedef struct {
    u8 encrypt;
    bool loop_en;
    u16 crc_key;
    u8 *addr;
    u32 pos;
    u32 len;                    //文件长度
    u32 start_skip_len;         //循环播放开头跳过长度
    u32 end_skip_len;           //循环播放结尾跳过长度
} spires_cb_t;

typedef struct {
    u32 input_len;
    u32 output_len;
    u32 quality;
} mav_init_t;

typedef struct AVIOContext {
    unsigned char *buffer;
    int buffer_size;
    unsigned char *buf_ptr;
    unsigned char *buf_end;
    unsigned int peek_size;
    int (*read_packet)(void *buf, unsigned int buf_size);
    bool (*seek)(unsigned int offset, int whence);
    unsigned int pos;
    int eof_reached;
    int error;
} AVIOContext;

typedef struct _WAVContext {
    AVIOContext pb;
    codec_info_t info;

    ///fmt Chunk
    u16 fmt_tag;
    u16 nchannel;
    u32 sample_rate;
    u32 bit_rate;
    u16 block_align;
    u16 bits_per_sample;
    u8  extensible_flag;

    ///data Chunk
    u32 data_size;
    u32 data_saddr;                 //data start address
    u32 data_pos;
    u32 skip;                       //sample rate > 48K, skip samples count

    //ima_adpcm
    struct {
        short   step_index[2];
        short   predictor[2];
        uint    nb_samples;           //samples from one block,  nb_samples=(block_align-4*channels)*2/channels
        short   *pcm_buf;             //Temporary saved 8 sample piont
        uint8_t bit_len[2];           //residue length
        uint    bit_buf[2];           //bit buf
    } adpcm;

} WAVContext;

typedef struct {
    u8  wav_start;
    u16 bdec_num;                   //block decode samples number
    u16 samples_cnt;                //decode sample count
    
    u8 *ptr;
    u8 sta;
    u16 ret;                        //记录保存的数据量
    u16 spr_fix_val;

    // u8 data_rbuf[ADPCM_READ_LEN];
    u16 adpcm_buf[16];
} wav_dec_cb_t;

typedef struct{
    msc_layer_e wav_layer;
    WAVContext wav_ctx;
    wav_dec_cb_t wav_dec;
    u8  wav_avio_buf[WAV_BUF_SIZE + PEEK_SIZE];
    // u8  pcm_obuf[WAV_BUF_LEN*2*2];
    u8* pcm_obuf;
}wav_layer_t;

typedef struct{
    u16 layer0_gain;
    u16 layer1_gain;
    u16 layer2_gain;
    u16 layer3_gain;
    u16 layer4_gain;
    u16 layer5_gain;
    u16 layer6_gain;
}music_layer_gain_t;

int wav_decode_init(wav_layer_t *wav_layer);
int mp3_decode_init(void);
int spi_mp3_decode_init(void);
int music_decode_init(void);
void music_control(u8 sta);
u8 get_music_dec_sta(void);
void set_music_dec_sta(u8 sta);

u16 music_get_total_time(void);
u16 music_get_cur_time(void);
void music_set_cur_time(u32 cur_time);
void music_set_jump(void *brkpt);
void music_get_breakpiont(void *brkpt);

void mp3_dec_stop(void);
bool wav_res_decode(wav_layer_t *wav_layer);
void wav_dec_stop(wav_layer_t *wav_layer);
void exspiflash_wav_mix_num_kick(u32 num);

bool mp3_res_play_kick(u32 addr, u32 len,u8 loop_cnt);
bool mp3_res_loop_play_kick(u32 addr, u32 len, u32 skip_start_bytes, u32 skip_end_bytes, bool loop_en);     //支持循环播放，头尾跳帧播放
void mp3_res_play_exit(void);
bool vmp3_res_play_kick(u32 addr, u32 len,u8 loop_cnt);
void vmp3_res_play_exit(void);

bool wav_res_play_kick(u32 addr, u32 len, wav_layer_t *wav_layer,u8 loop_cnt);
bool wav_res_loop_play_kick(u32 addr, u32 len, u32 skip_start_bytes, u32 skip_end_bytes, u8 loop_en, wav_layer_t *wav_layer);     //支持循环播放，头尾跳帧播放
void wav_res_play_exit(void);

bool esbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt);
bool esbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en);      //支持循环播放，头尾跳帧播放
void esbc_res_play_exit(void);

bool lsbc_res_play_kick(u32 addr, u32 len, u8 loop_cnt);
bool lsbc_res_loop_play_kick(u32 addr, u32 len, u16 sta_skip_frame, u16 end_skip_frame, bool loop_en);      //支持循环播放，头尾跳帧播放
void lsbc_res_play_exit(void);

void wav_pitch_up_init(void);
void res_play_loop_back(u8 enable);                 //spiflash循环播放接口
void res_play_loop_back_skip_size(u16 skip_star_size, u16 skip_end_size); //外接FLASH音乐循环播放mp3开关,设置循环时候跳过前后多少字节音频数据，用于部分应用避开静音区数
void spiflash_rec_mp3_filelen_reduce(u16 ms);       //剪掉REC尾部 ms(spiflash)

u32 spires_get_pos(void);
void spires_load_pos(u32 pos);

bool dac_set_eq_by_res(u32 *addr, u32 *len);
void set_dac_eq_spr(u8 spr);
u8 get_dac_eq_spr(void);

void mpa_encode_kick_start(void);
void mpa_encode_frame(void);
bool mpa_encode_init(u32 spr, u32 nch, u32 bitrate);
void mpa_encode_exit(void);

void muisc_decode_process(wav_layer_t *wav_layer_tbl[]);

/*--------------------------------------
@brief:     layer0淡入淡出参数设置
@fade_in_frame_num:     layer0淡入帧长设置
@fade_out_frame_num:    layer0淡出帧长设置
@fade_out_tick:         layer0淡出tick时间设置(设置此参数,控制音频播放多少ms后启动淡出,并以fade_out_frame_num为淡出长度计算基准)
@fade_out_type:         layer0淡出方式,0-从音频开始计算fade_out_tick时间后开始淡出,1-从音频剩余fade_out_frame_num帧长时开始淡出
@fade_out_type:         layer0淡出方式为1时仅支持wav和mp3音频
--------------------------------------*/
void set_layer0_fade(u16 fade_in_frame_num, u16 fade_out_frame_num, u32 fade_out_tick, u8 fade_out_type);
/*--------------------------------------
@brief:     layer1淡入淡出参数设置
@fade_in_frame_num:     layer1淡入帧长设置
@fade_out_frame_num:    layer1淡出帧长设置
@fade_out_tick:         layer1淡出tick时间设置(设置此参数,控制音频播放多少ms后启动淡出,并以fade_out_frame_num为淡出长度计算基准)
@fade_out_type:         layer1淡出方式,0-从音频开始计算fade_out_tick时间后开始淡出,1-从音频剩余fade_out_frame_num帧长时开始淡出
@fade_out_type:         layer1淡出方式为1时仅支持wav和mp3音频
--------------------------------------*/
void set_layer1_fade(u16 fade_in_frame_num, u16 fade_out_frame_num, u32 fade_out_tick, u8 fade_out_type);

#endif // _API_MUSIC_H
