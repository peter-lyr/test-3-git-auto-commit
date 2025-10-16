#include "include.h"
#include "func.h"
//#include "func_music.h"
#include "func_exspiflash_music.h"
#include "api_spiflash_ex.h"
void os_spiflash_program(void *buf, u32 addr, uint len);
void os_spiflash_erase(u32 addr);
uint os_spiflash_read(void *buf, u32 addr, uint len);
void spiflash_program(void *buf, u32 addr, uint len);
void spiflash_erase(u32 addr);
u16 spiflash_rec_read_file(void *buf, u32 len);

#if (EX_SPIFLASH_SUPPORT)
exspiflash_msc_t exspi_msc AT(.ext_msc.exf);

bool exspiflash_init(void)
{
    printf("exspiflash_init\n");
#if !REC_INTERNAL_FLASH
    spiflash1_init();
    if (!is_exspiflash_online()) {
        return false;
    }
#endif
#if  SPIFLASH_MUSIC_BIN_WRITE_TEST    //默认的FLASH_MUSIC.BIN写入SPIFLASH, 可以用于测试外接SPIFLASH播放MP3.
    write_music_bin_to_spiflash();
#endif

#if REC_INTERNAL_FLASH
    cycrec_fs_init(SPIFLASH_REC_BEGIN_ADDR, SPIFLASH_REC_END_ADDR, (void *)os_spiflash_read, (void *)os_spiflash_program, (void *)os_spiflash_erase);
#elif (EX_SPIFLASH_SUPPORT & EXSPI_REC)
    cycrec_fs_init(SPIFLASH_REC_BEGIN_ADDR, SPIFLASH_REC_END_ADDR, (void *)spiflash1_read, (void *)spiflash1_write, (void *)spiflash1_erase);
#endif
    return true;
}

AT(.text.func.music)
void func_exspiflash_music_process(void)
{
    func_process();
    if(!layer_play_check(MSC_LAYER0)){
#if SPIFLASH_MUSIC_PLAY_REC
        if (!exspi_msc.rec_no_file) {
            exspiflash_rec_switch_file(1);
        }
#else
    #if SPIFLASH_MUSIC_BREAK_LOOP
        exspiflash_music_switch_file(1);
    #else
        exspiflash_music_num_kick(exspi_msc.cur_num);
    #endif
#endif
    }
    //wav_mix_dec(192);
}

AT(.text.func.music)
void func_exspiflash_music_display(void)
{

}

AT(.text.func.music)
void exspiflash_music_switch_file(u8 direction)   //1 next , 0 prev
{
    music_control(MUSIC_MSG_STOP);
    printf("exspiflash_music_switch_file: dir:%d, %d->", direction, exspi_msc.cur_num);
    if (direction) {
        if (++exspi_msc.cur_num > exspi_msc.total_num) {
            exspi_msc.cur_num = 1;
        }
    } else {
        if (exspi_msc.cur_num >= 1) {
            exspi_msc.cur_num--;
        }
        if (exspi_msc.cur_num == 0) {
            exspi_msc.cur_num = exspi_msc.total_num;
        }
    }
    printf("%d/%d\n", exspi_msc.cur_num, exspi_msc.total_num);
    exspiflash_music_num_kick(exspi_msc.cur_num);
}


#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
AT(.text.func.music)
void exspiflash_rec_switch_file(u8 direction)   //1 next , 0 prev
{
    u32 addr = 0, len = 0;
    u8 ret = 1;
    printf("rec next file, direction = %d\n",direction);
    music_control(MUSIC_MSG_STOP);
    if (direction) {
        ret = spiflash_rec_open_next_file(&addr, &len);
    } else {
        ret = spiflash_rec_open_prev_file(&addr, &len);
    }
    exspi_msc.rec_no_file = ret;
    printf("open file ret = %d, addr = 0x%X, len = %d\n",ret, addr, len);
    if (!ret) {
         mp3_res_play_kick(addr, len, 0);
    }
}

//播放SPI0 flash或SPI1 flash中的最后一个录音
bool spiflash_rec_play_last_file(void)
{
    u32 addr, len;
    if (spiflash_rec_open_last_file(&addr,&len)) {      //打开最后一个录音文件.
        return false;
    }
#if REC_PLAY_MODE_SEL
    func_cb.mp3_res_play = func_speaker_mp3_song_play;  //使用song_play后台播放
#else
    func_cb.mp3_res_play = func_speaker_mp3_res_play;   //使用res_play独占播放
#endif
    if (func_cb.mp3_res_play != NULL) {
        printf("spiflash_rec_play_last_file : addr = %d, len = %d\n",addr,len);
#if REC_INTERNAL_FLASH
        register_spi_read_function((void *)os_spiflash_read);
        register_wav_spi_read_function(NULL);
        register_rec_read_function(spiflash_rec_read_file);
#else
        register_spi_read_function(spiflash1_read);
        register_wav_spi_read_function(spiflash1_read);
#endif
        func_cb.mp3_res_play(addr, len);
#if !REC_PLAY_MODE_SEL
        if (func_cb.sta != FUNC_EXSPIFLASH_MUSIC) {
            register_spi_read_function(NULL);
            register_wav_spi_read_function(NULL);
        }
#endif
    }
    return true;
}
#endif


AT(.text.func.music)
bool exspiflash_music_image_check(void)
{
    if (is_exspiflash_online()) {
        exspi_msc.total_num = exspiflash_music_get_file_total();
        exspi_msc.msc2_en   = exspiflash_msc_version();
        printf("exspi_msc.total_num = %d, msc2: %d\n",exspi_msc.total_num, exspi_msc.msc2_en);
        if (exspi_msc.total_num > 0) {
            return true;
        }
    }
    return false;
}

AT(.text.func.music)
void func_exspiflash_mp3_res_play(u32 addr, u32 len)
{
    msc_breakpiont_t brkpt;
    music_get_breakpiont(&brkpt);           //保存当前播放位置
    u32 cur_time = music_get_cur_time();
    music_control(MUSIC_MSG_STOP);
    register_spi_read_function(NULL);       //恢复内部SPI读接口.
    mp3_res_play(addr, len);
    register_spi_read_function(spiflash1_read);
    music_set_cur_time(cur_time);
    exspiflash_music_num_kick(exspi_msc.cur_num);
    music_set_jump(&brkpt);                 //恢复播放位置
}

AT(.text.func.music)
static void func_exspiflash_music_enter(void)
{
    msg_queue_clear();
#if SPIFLASH_MUSIC_PLAY_REC
    u32 addr, len;
    u32 rec_total = spiflash_rec_get_total_file();
    printf("func_exspiflash_music_enter, rec_total = %d\n",rec_total);
    func_cb.mp3_res_play = func_exspiflash_mp3_res_play;
    register_spi_read_function(spiflash1_read);
    register_wav_spi_read_function(spiflash1_read);
    if ((!rec_total) ||spiflash_rec_open_last_file(&addr,&len)) {  //打开最后一个录音文件.
        func_cb.sta = FUNC_NULL;
        return;
    }
    bsp_change_volume(sys_cb.vol);
    exspi_msc.pause = false;
    mp3_res_play_kick(addr, len, 0);
#else
    if ((!exspiflash_music_image_check())){
        func_cb.sta = FUNC_NULL;
        return;
    }
    func_cb.mp3_res_play = func_exspiflash_mp3_res_play;
    register_spi_read_function(spiflash1_read);
    register_wav_spi_read_function(NULL);
    bsp_change_volume(sys_cb.vol);
    exspi_msc.cur_num = 1;            //默认从1首开始播放
    exspi_msc.pause = false;

#if SPIFLASH_MUSIC_BREAK_MEMORY
    param_spiflash_breakpoint_read();             //进入flash_music模式，恢复到上次退出时的播放位置 ，
    if(0 == exspi_msc.save_num){
        exspi_msc.cur_num = 1;
    }
    else{
        exspi_msc.cur_num = exspi_msc.save_num;
    }
    #if SPIFLASH_MUSIC_SEAMLESS_LOOP
        res_play_loop_back_skip_size(0x2000, 0x2000);
    #endif
    music_set_cur_time(exspi_msc.save_time);
    exspiflash_music_num_kick(exspi_msc.cur_num);
    music_set_jump(&(exspi_msc.brkpt));
    #if SPIFLASH_MUSIC_SEAMLESS_LOOP
        res_play_loop_back(1);
    #endif
#else
    exspiflash_music_num_kick(exspi_msc.cur_num);
#endif // SPIFLASH_MUSIC_BREAK_MEMORY
#endif // SPIFLASH_MUSIC_PLAY_REC
}

AT(.text.func.music)
static void func_exspiflash_music_exit(void)
{
    #if SPIFLASH_MUSIC_SEAMLESS_LOOP
    res_play_loop_back(1);
    #endif

    #if SPIFLASH_MUSIC_BREAK_MEMORY
    //退出时保存当前播放位置
    music_get_breakpiont(&(exspi_msc.brkpt));
    exspi_msc.save_time = music_get_cur_time();
    exspi_msc.save_num = exspi_msc.cur_num;
    #endif // SPIFLASH_MUSIC_BREAK_MEMORY

    music_control(MUSIC_MSG_STOP);
    register_spi_read_function(NULL);       //恢复layer0 SPI0读接口.
    register_wav_spi_read_function(NULL);   //恢复layer1~n SPI0读接口.
    func_cb.last = FUNC_EXSPIFLASH_MUSIC;
}

AT(.text.func.music)
void non_volatile_memory(void)
{
    #if SPIFLASH_MUSIC_BREAK_MEMORY
    static u32 ticks = 0;
    u32 curFileLen = 0;
    u32 ptrTemp = 0;
    if(tick_check_expire(ticks,100)){      //100ms记忆一次
        ticks = tick_get();
        music_get_breakpiont(&(exspi_msc.brkpt));

        curFileLen = exspiflash_music_get_cur_file_len();
        if(exspi_msc.brkpt.file_ptr >= curFileLen)
        {
            ptrTemp = (exspi_msc.brkpt.file_ptr % curFileLen);
            exspi_msc.brkpt.file_ptr = ptrTemp;
        }

        exspi_msc.save_time = music_get_cur_time();
        exspi_msc.save_num = exspi_msc.cur_num;
        param_spiflash_breakpoint_write();
        param_sync();
    }
    #endif // SPIFLASH_MUSIC_BREAK_MEMORY
}

AT(.text.func.music)
void func_exspiflash_music(void)
{
    printf("%s\n", __func__);
    func_exspiflash_music_enter();
    while (func_cb.sta == FUNC_EXSPIFLASH_MUSIC) {
        func_exspiflash_music_process();
        func_exspiflash_music_message(msg_dequeue());
        func_exspiflash_music_display();
    }
    func_exspiflash_music_exit();
}
#endif

