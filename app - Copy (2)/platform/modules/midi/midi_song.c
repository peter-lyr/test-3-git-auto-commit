#include "include.h"
#include "midi_song.h"

#if MIDI_SONG_EN && MIDI_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern cbuf_t midi_ptr;

/*--------------------------------------
@brief: midi解码轨道结束回调
@cur_track_num: 当前轨道
--------------------------------------*/
void midi_track_end_callback(u8 cur_track_num)
{
    TRACE("Track End: %d\n", cur_track_num);
}

/*--------------------------------------
@brief:     midi note发音前回调(9n kk vv/8n kk vv)
@status:    开音码/关音码(n为midi通道0~16)
@param:     音符号9n/8n(kk为音高0~127)
@velocity:  力度(vv为音量0~127)
--------------------------------------*/
void midi_note_start_callback(u8 status, u8 param, u8 velocity)
{
    TRACE("%02X %02X %02X\n", status, param, velocity);
}

/*--------------------------------------
@brief:     midi note发音后回调(9n kk vv/8n kk vv)
@status:    开音码/关音码(n为midi通道0~16)
@param:     音符号9n/8n(kk为音高0~127)
@velocity:  力度(vv为音量0~127)
--------------------------------------*/
void midi_note_end_callback(u8 status, u8 param, u8 velocity)
{
    // TRACE("%02X %02X %02X\n", status, param, velocity);
}

/*--------------------------------------
@brief:     控制器码处理前回调Control change(An ss cc/Bn ss cc)
@status:    控制器码An/Bn(n为midi通道0~16)
@param:     控制器号(ss为0~127)
@velocity:  控制值(cc为0~127)
--------------------------------------*/
void midi_control_start_callback(u8 status, u8 param, u8 velocity)
{
    // TRACE("%02X %02X %02X\n", status, param, velocity);
}

/*--------------------------------------
@brief:     控制器码处理后回调Control change(An ss cc/Bn ss cc)
@status:    控制器码An/Bn(n为midi通道0~16)
@param:     控制器号(ss为0~127)
@velocity:  控制值(cc为0~127)
--------------------------------------*/
void midi_control_end_callback(u8 status, u8 param, u8 velocity)
{
    // TRACE("%02X %02X %02X\n", status, param, velocity);
}

/*--------------------------------------
@brief:     音色码处理前回调Program change(Cn pp)
@status:    音色码Cn(n为midi通道0~16)
@param:     音色编号(pp为0~127)
--------------------------------------*/
void midi_prog_start_callback(u8 status, u8 param)
{
    // TRACE("%02X %02X\n", status, param);
}

/*--------------------------------------
@brief:     音色码处理后回调Program change(Cn pp)
@status:    音色码Cn(n为midi通道0~16)
@param:     音色编号(pp为0~127)
--------------------------------------*/
void midi_prog_end_callback(u8 status, u8 param)
{
    // TRACE("%02X %02X\n", status, param);
}

/*--------------------------------------
@brief:     通道压力码处理前回调Channel After Touch(Dn vv)
@status:    通道码Dn(n为midi通道0~16)
@param:     压力值(pp为0~127)
--------------------------------------*/
void midi_chan_at_start_callback(u8 status, u8 param)
{
    // TRACE("%02X %02X\n", status, param);
}

/*--------------------------------------
@brief:     通道压力码处理后回调Channel After Touch(Dn vv)
@status:    通道码Dn(n为midi通道0~16)
@param:     压力值(pp为0~127)
--------------------------------------*/
void midi_chan_at_end_callback(u8 status, u8 param)
{
    // TRACE("%02X %02X\n", status, param);
}

/*--------------------------------------
@brief:     弯音码处理前回调Pitch bend(En bL bH)
@status:    弯音码En(n为midi通道0~16)
@vel_l:     弯音值低位(bL为0~127)
@vel_h:     弯音值高位(bH为0~127)
--------------------------------------*/
void midi_pitch_bend_start_callback(u8 status, u8 vel_l, u8 vel_h)
{
    // TRACE("%02X %02X %02X\n", status, vel_l, vel_h);
}

/*--------------------------------------
@brief:     弯音码处理后回调Pitch bend(En bL bH)
@status:    弯音码En(n为midi通道0~16)
@vel_l:     弯音值低位(bL为0~127)
@vel_h:     弯音值高位(bH为0~127)
--------------------------------------*/
void midi_pitch_bend_end_callback(u8 status, u8 vel_l, u8 vel_h)
{
    // TRACE("%02X %02X %02X\n", status, vel_l, vel_h);
}

void vmidi_res_play_do(u32 addr, u32 len)
{
    u16 msg;
    if (len == 0) {
        return;
    }
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    if(get_midi_dec_sta() != MIDI_STOP){
        midi_control(MIDI_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    dac_fade_in();
    vmidi_res_play_kick(addr, len);
    while(get_midi_dec_sta() != MIDI_STOP){
        WDT_CLR();
        if(!midi_decode()){
            break;
        }
        msg = msg_dequeue();
        if (msg != NO_MSG) {
            // msg_enqueue(msg);       //还原未处理的消息
            func_message(msg);      //立即处理消息(注意不要函数嵌套调用)
        }
    }
    vmidi_res_play_exit();
}

//vmidi歌曲-->提示音播放(独占播放)
void vmidi_res_play(u32 addr, u32 len)
{
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    sys_clk_req(INDEX_MIDI, SYS_160M);
    music_mode_enter(MSC_LAYER0, MSC_VMIDI);
    vmidi_res_play_do(addr, len);
    sys_clk_free(INDEX_MIDI);
    music_mode_exit(MSC_LAYER0, MSC_VMIDI);
}

//vmidi歌曲-->后台播放(非独占播放)
void vmidi_song_play(u32 addr, u32 len)
{
#if MIDI_DEC_BK_EN
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_VMIDI);
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    if(MIDI_PLAYING == get_midi_dec_sta()){
        vmidi_song_play_exit();
        vmidi_song_play_kick(addr, len);
    }
    if(MIDI_STOP == get_midi_dec_sta()){
        vmidi_song_play_kick(addr, len);
        midi_control(MIDI_PLAYING);
        music_control(MUSIC_MSG_PLAY);
    }
#endif
}

//标准midi歌曲-->后台播放(非独占播放)
void midi_song_play(u32 addr, u32 len)
{
#if MIDI_DEC_BK_EN
    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    music_mode_enter(MSC_LAYER0, MSC_MIDI);
    if(get_music_dec_sta() != MUSIC_STOP) {
        music_control(MUSIC_MSG_STOP);
    }
    if(MIDI_PLAYING == get_midi_dec_sta()){
        midi_song_play_exit();
        midi_song_play_kick(addr, len);
    }
    if(MIDI_STOP == get_midi_dec_sta()){
        midi_song_play_kick(addr, len);
        midi_control(MIDI_PLAYING);
        music_control(MUSIC_MSG_PLAY);
    }
#endif
#if MIDI_OKON_EN
    set_midi_track_stat(OKON_TRACK_EN);             //设置需要使能的Track
    set_midi_channel_stat(OKON_CH_EN);              //设置需要使能的Channel
#endif
}

//midi歌曲播放结束回调接口
void midi_play_end(void)
{
    if(get_msc_layer_mode(MSC_LAYER0) == MSC_MIDI){
        music_mode_exit(MSC_LAYER0, MSC_MIDI);
    }
    if(get_msc_layer_mode(MSC_LAYER0) == MSC_VMIDI){
        music_mode_exit(MSC_LAYER0, MSC_VMIDI);
    }
}

#endif
