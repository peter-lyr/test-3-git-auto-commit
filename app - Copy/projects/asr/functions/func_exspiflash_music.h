#ifndef FUNC_EXSPIFLASH_MUSIC_H
#define FUNC_EXSPIFLASH_MUSIC_H


#include "spiflash/spiflash1.h"


typedef struct {
    u8  msc2_en;            //V2�汾music�����־
    u16 total_num;
    u16 cur_num;
    u8  pause;
    u8  rec_no_file;
    #if SPIFLASH_MUSIC_BREAK_MEMORY
    msc_breakpiont_t brkpt;//���ڱ���ͻָ�����λ�õĽṹ��
    u16 save_time;//�����˳�flashģʽʱ�����ʱ��
    u16 save_num;//�����˳�ʱ���浱ǰ���Ÿ��������
    #endif // SPIFLASH_MUSIC_BREAK_MEMORY
} exspiflash_msc_t;
extern exspiflash_msc_t exspi_msc;


void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));
void func_exspiflash_music_message(u16 msg);
void exspiflash_music_switch_file(u8 direction);
void exspiflash_rec_switch_file(u8 direction);
extern bool mp3_res_play_kick(u32 addr, u32 len);
void non_volatile_memory(void);

#endif


