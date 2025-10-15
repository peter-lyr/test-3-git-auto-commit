#ifndef __BSP_PARAM_H
#define __BSP_PARAM_H

#define PARAM_MSC_NUM_SD            0
#define PARAM_MSC_NUM_USB           2
#define PARAM_SYS_VOL               4
#define PARAM_HSF_VOL               5
#define PARAM_LANG_ID               6
#define PARAM_SYS_MODE              0x0c        //系统模式记忆
#define PARAM_MSC_NUM_SD1           0x0d        //2BYTE

#define PARAM_MSC_BRKPT_SD          0x10        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_USB         0x1a        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_SD1         0x24        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)

#define PARAM_FMRX_CHCUR            0x30
#define PARAM_FMRX_CHCNT            0x31
#define PARAM_FMRX_CHBUF            0x32        //26Byte
#define PARAM_FMTX_FREQ             0x4C        //FM TX freq 2 Byte
#define PARAM_ECHO_LEVEL            0x4E        //echo level 1 Byte
#define PARAM_ECHO_DELAY            0x4F        //echo delay 1 Byte

#define PARAM_SPIFLASH_MEMORY       0x50        //使用10byte   // SPIFLASH断电记忆功能
#define PARAM_SPIFLASH_SAVE_TIME    0x5a        //使用2byte
#define PARAM_SPIFLASH_SAVE_NUM     0x5c        //使用2byte

#define PARAM_RES_BREAK_ADDR        0x60        //4byte
#define PARAM_RES_BREAK_POS         0x64        //4byte

//#define PARAM_MASTER_ADDR_VALID     0x50        //用于记录主耳的地址是否有效
//#define PARAM_MASTER_ADDR           0x51        //6BYTE
//#define PARAM_NEW_LOCAL_NAME        0x57        //64byte=len+sum+62BYTE

#define RTCRAM_PWROFF_FLAG          63         //软关机的标识放在RTCRAM的最后一BYTE



void param_init(bool reset);
void param_sync(void);
void param_msc_num_write(void);
void param_msc_num_read(void);
void param_msc_breakpoint_write(void);
void param_msc_breakpoint_read(void);
void param_fmrx_chcur_write(void);
void param_fmrx_chcur_read(void);
void param_fmrx_chcnt_write(void);
void param_fmrx_chcnt_read(void);
void param_fmrx_chbuf_write(void);
void param_fmrx_chbuf_read(void);
void param_sys_vol_write(void);
void param_sys_vol_read(void);
void param_lang_id_write(void);
void param_lang_id_read(void);
void param_random_key_write(void);
void param_random_key_read(u8 *key);
u8 param_sys_mode_read(void);
void param_sys_mode_write(u8 mode);
void param_fmtx_freq_write(void);
void param_fmtx_freq_read(void);
void bsp_param_write(u8 *buf, u32 addr, uint len);
void bsp_param_read(u8 *buf, u32 addr, uint len);
void bsp_param_sync(void);
void param_spiflash_breakpoint_write(void);
void param_spiflash_breakpoint_read(void);
u32  param_vmp3_break_addr_read(void);
void param_vmp3_break_addr_write(u32 break_addr);
u32  param_vmp3_break_pos_read(void);
void param_vmp3_break_pos_write(u32 break_pos);
#endif // __BSP_PARAM_H

