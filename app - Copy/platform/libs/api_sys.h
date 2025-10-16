#ifndef _API_SYS_H_
#define _API_SYS_H_

#define IRQ_TMR0_VECTOR                 3
#define IRQ_TMR1_VECTOR                 4
#define IRQ_TMR2_VECTOR                 5
#define IRQ_SD_VECTOR                   8
#define IRQ_MIDI_VECTOR                 9
#define IRQ_UART_VECTOR                 14
#define IRQ_HUART_VECTOR                15
#define IRQ_PWM_VECTOR                  16
#define IRQ_SPI_VECTOR                  20
#define IRQ_KEY_VECTOR                  21
#define IRQ_TKEY_VECTOR                 23
#define IRQ_FREQDET_VECTOR              23
#define IRQ_PORT_VECTOR                 26
#define IRQ_RTC_VECTOR                  29
#define IRQ_IIC_VECTOR                  30

typedef struct {
    u8  vbg;                    //trim到0.8V的寄存器配置值
    u16 vbg_volt;               //vbg的测量值, 基准1500
    u16 vbat_coef;
    u8  cp_flag;
    u8  ft_flag;
    u32 delt_d;
    u32 d2_delt;
} sys_trim_t;
extern sys_trim_t sys_trim;

typedef struct
{
    u8  res_buf1[4];

    u32 cp_vbg_trim             : 5,
        cp_vbg_offset           : 6,
        cp_vbat_offset          : 7,
        ft_dcd1_offset0         : 1,
        pll_band                : 3,
        ft_dcd1_offset1         : 4,
        ft_dcd1_sign            : 1,
        pll_div0_res1           : 5;

    u8  pll_div0_buf2[2];

    u8  pll_div0_res2            : 3,
        ft_dcd2_offset0          : 5;

    u8  pll_div1_buf[3];

    u16 res_buf2                 : 7,
        ft_dcd2_offset1          : 3,
        ft_dcd2_sign             : 1,
        gpio_rst                 : 2,
        mclr                     : 2,
        cp_flag                  : 1;

}efuse_trim;

#define VBG_VOLTAGE             sys_trim.vbg_volt
#define VBAT2_COEF              sys_trim.vbat_coef

typedef void (*isr_t)(void);
void sys_set_tmr_enable(bool tmr5ms, bool tmr1ms);
u32 sys_get_rand_key(void);

/**
* @brief  system reset init
* @return system reset source
reset source:   BIT(31):PWRUP_RST
                BIT(24):SW_RST
                BIT(19):WK Pin 10S_RST
                BIT(18):WAKEUP_RST
                BIT(17):VUSB_RST
                BIT(16):WDT_RST
                BIT(20)|BIT(7):LVD_RST
*/
u32 sys_rst_init(void);

/**
* @brief  dump the system reset source info
* @param  reason: system reset source, can be get by the return value of the function sys_rst_init
*/
void sys_rst_dump(u32 reason);

/*
*   pr = 1, 高优先级中断, 中断入口函数isr放公共区且要加FIQ, 例如：
*   AT(.com_text.isr) FIQ
*   void timer3_isr(void) {}
*
*   pr = 0, 低优先级中断,不用加FIQ, 函数放入公共区, 例如：
*   AT(.com_text.isr)
*   void timer3_isr(void) {}
*
*   中断函数需要尽量精简，否则影响系统效率。推荐用低优先级中断，需要很高响应速度时才用高优先级中断
*/
bool sys_irq_init(int vector, int pr, isr_t isr);           //初始化一个中断, pr为优先级
void lvd_irq_init(void);

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数
void sys_clk_output(u32 type, u32 mapping);                 //输出时钟   type: 1->12M, 3->xosc32K

u16 convert_uni2gbk(u16 code, u32 addr, u32 len);           //输入utf16, 返回gbk编码, 找不到编码，使用'□'
u8 utf8_char_size(u8 code);                                 //返回UTF8占用几字节
u16 utf8_convert_to_unicode(u8 *in, u8 char_size);          //UTF-8转成UTF-16 LE编码
void dbg_clk_out(u32 clk_src, u32 clk_div);
void adpll_init(u32 sys_clk);

void my_spi_init(void);
void my_spi_putc(u8 ch);

uint os_spiflash_read(void *buf, u32 addr, uint len);       //SPI0 flash读取
void os_spiflash_program(void *buf, u32 addr, uint len);    //SPI0 flash编程
void os_spiflash_erase(u32 addr);                           //SPI0 flash擦除

bool spiflash_read_kick(void *buf, u32 addr, uint len);     //SPI0 flash简易读取
bool spiflash_read_wait(void);                              //SPI0 wait pending

/*****************************************************************************
* @name    void trim_write(u8 *buf)
* @brief   trim值写入函数，写入长度固定2byte,写入地址自动分配,要求系统时钟为48M
* @param   buf: 数据存储buf
* @return  reserve
*****************************************************************************/
void trim_write(u8 *buf);
/*****************************************************************************
* @name    void trim_read(u8 *buf)
* @brief   trim值读取函数，读取长度固定2byte,写入地址自动分配
* @param   buf: 数据存储buf
* @return  reserve
*****************************************************************************/
void trim_read(u8 *buf);

#endif // _API_SYS_H_

