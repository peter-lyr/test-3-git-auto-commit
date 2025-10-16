#ifndef _API_SYSCLK_H_
#define _API_SYSCLK_H_

enum {
    SYS_RC2M,
    SYS_24M,
    SYS_48M,
    SYS_64M,
    SYS_96M,
    SYS_128M,
    SYS_160M,       //384/2.5 = 153.6MHz
    SYS_192M, 
};

enum {
    INDEX_VOICE,
    INDEX_DECODE,
    INDEX_MIDI,
    INDEX_RES_PLAY,
    INDEX_ENCODE,
    INDEX_RSVD1,
    INDEX_RSVD2,
    INDEX_RSVD3,
    INDEX_MAX_NB = 8,
};

extern bool cfg_spiflash_speed_up_en;
extern bool cfg_spiflash_quard_en;
extern bool cfg_spiflash_2wire_en;

u8 get_cur_sysclk(void);
u32 get_sysclk_nhz(void);
uint8_t sys_clk_get(void);
void sys_clk_set_do(u32 sys_clk);                       //上电启动未启用OS时调用
bool sys_clk_set(uint8_t sys_clk);                      //main线程调用，一般给客户调用
bool sys_clk_req(uint8_t index, uint8_t sys_clk);       //其它任务调用，一般在库内调用
bool sys_clk_free(uint8_t index);                       //跟sys_clk_req成对使用
bool sys_clk_free_all(void);
u32 sys_get_rc2m_clkhz(u32 type);
u32 sys_get_osc32k_clkhz(void);
u32 get_sysclk_nhz(void);                   //获取系统时钟(单位：Hz)
u32 rc2m_clk_nhz_get(u8 type);              //Get RC2M clk frequency, type: 0: system rc2m  1:rtc rc2m
u32 rc2m_clk_nhz_get_l4m(u8 type);
#endif
