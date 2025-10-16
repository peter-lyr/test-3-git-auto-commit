#include "include.h"
#include "func.h"

#define TRACE_EN                1
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

extern u32 __comm_vma, __comm_lma, __comm_size;
extern u32 __pdn_comm_vma, __pdn_comm_lma, __pdn_comm_size;
extern u32 rc2m_freq_cal;

bool power_off_check(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);
void lp_sleep(void (*sleep_cb)(void), void (*wakeup_cb)(void));
void boot_uart_init(void);
void spiflash_init(u8 sf_read, u8 dummy, bool mode4io);
void timer0_init(void);
void dcache_init_l4m(void);

#define VRTC_SEL                4       //All Mode: VRTC档位(set: 0~7)
#define VDDIO_AON_SEL           1       //All Mode: 关机后VDDIO电压档位,0-2.4V, 1-2.8V, 2-3.1V, 3-3.3V
#define SPIFLASH_DP_PDN4_EN     0       //L4M模式: flash深度睡眠使能宏,能降低一些功耗
#define PWR_RTC1S_WK_MODE       1       //RTC1S模式: 1-wakeup form boot, 0-wakeup next address
#define PWR_RTC1S_WK_TIME       0xB7D7  //RTC1S模式: CLK来源-->rtc_rc2m/64
#define PWR_ALARM_WK_TIME       10      //ALARM模式: 唤醒时间设置,单位:秒

#define SPI_CS1_PDIR            GPIOGDIR
#define SPI_CS1_PORTSET         GPIOGSET
#define SPI_CS1_PORTCLR         GPIOGCLR
#define SPI_CS1                 2
#define SPI_CS1_EN()            SPI_CS1_PORTCLR = BIT(SPI_CS1)
#define SPI_CS1_DIS()           SPI_CS1_PORTSET = BIT(SPI_CS1)

#define SPIFLASH_DP             0xB9
#define SPIFLASH_RDP            0xAB

pwroff_wk_t wk_cfg AT(.pdncom_buf.wakeup);

#if (POWER_DOWN_MODE == PWR_L4M) || (POWER_DOWN_MODE == PWR_L4M_ALM) || (!SOFT_POWER_ON_OFF) || (POWER_DOWN_MODE == PWR_RTC1S) || MIX_PWR_DOWN_EN
static sleep_sfr_t sp_sfr AT(.pdncom_buf.sleep);
#endif

//软开机唤醒源打印输出
u32 lowpwr_get_wakeup_source(void)
{
    u32 reason = RTCCON9;
    if(RTCCON9){
        if(RTCCON9 & WK_PWROFF_BGUV){
            printf("BGUV wakeup\n");
        }
        if(RTCCON9 & WK_PWROFF_GPIO){
            printf("GPIO wakeup\n");
        }
        if(RTCCON9 & WK_PWROFF_RTC1S){
            printf("RTC 1s wakeup\n");
        }
        if(RTCCON9 & WK_PWROFF_ALARM){
            printf("alarm wakeup\n");
        }
    }
    return reason;
}

//软关机PMU电源配置
AT(.pdncom_text.pmu)
void pwroff_pmu_voltage_config(void)
{
    PWRCON0 |= BIT(20);                         //pmu_normal
    RTCCON0 &= ~(0xff<<8);                      //close CLK2M
    RTCCON0 &= ~BIT(0);                         //disable pmu RC
    RTCCON0 &= ~(BIT(0)|BIT(1)|BIT(2)|BIT(18)); //rcosc
    RTCCON1 &= ~BIT(25);                        //viopd_disable
#ifdef SNF_RC_EN
    RTCCON0 &= ~(0xff<<8);
    RTCCON0 |= BIT(0);                          //enable SNIFF RC
#endif // SNF_RC_EN

    CLKCON0 &= ~0xC;                            //SYS clock switch to RC
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    CLKGAT1 &= ~BIT(15);                        //disable 26M
    PLL0CON = 0;                                //disable PLL

    //配置关机下，VRTC数字的工作电压；RTCCON BIT0为0则选择RTCCON1 BIT8:6; 为1则选择BIT 11:9
    RTCCON1 = (RTCCON1 & ~(7<<6)) | (VRTC_SEL<<6) | (7 << 9);

    //VDDIO LDO从正常LDO切换到VDDIO_AON
    PWRCON0 = (PWRCON0 & ~(0x1f<<5)) | (0x13<<5);       //VDDIO=2.925v,正常运行时的vddio稍微降低点再切换来源
    RTCCON1 = (RTCCON1 & ~(3<<2)) | (VDDIO_AON_SEL<<2); //VDDIO_AON/V,关机后的vddio电压
    RTCCON3 |= BIT(7);                                  //VDDIO_AON enable
}

//软关机,关机前RTC配置
AT(.pdncom_text.rtc)
void pwroff_rtc_core_config(pwroff_wk_t *wkcfg)
{
    if(MCLR_RESRT_MDOE == 2){
        RTCCON12 |= 0xf3;       //disable GPIO reset
        RTCCON12 &= ~0xc;       //PB10 reset
    }else{
        RTCCON12 |= 0xff;       //disable GPIO reset & PB10 reset
    }
}

//红外关机进入sleep mode  需要开启freqdetRC2M的频率
ALIGNED(512)
AT(.pdncom_text.sleepmode)
void func_sleepmode(void)
{
    printf("%s\n", __func__);
#if (IRRX_SW_EN && FREQ_DET_EN) || (TKEY_MUL_SCAN_EN && TKEY_SLEEP_MODE)
    u16 gpioa_fen, gpiob_fen, gpiog_fen;
    u16 gpioa_dir, gpiob_dir, gpiog_dir;
    u16 gpioa_de, gpiob_de, gpiog_de;
    u16 gpioa_drv, gpiob_drv, gpiog_drv;
    u16 gpioa_pu, gpiob_pu, gpiog_pu;
    u16 gpioa_pd, gpiob_pd, gpiog_pd;

    u16 saradc_ch;
    u8 sys_clk;
    u32 cpu_ie;
    u32 sddaccon;
    u32 clkdivcon0;
    u32 pll0con;
    u32 pwrcon0;
    //PLL0CON = ~BIT(18);                //pll10 sdm enable disable

    //io save
    gpioa_fen = GPIOAFEN;
    gpiog_fen = GPIOGFEN;
    gpioa_dir = GPIOADIR;
    gpiog_dir = GPIOGDIR;
    gpioa_de  = GPIOADE;
    gpiog_de  = GPIOGDE;
    gpioa_drv = GPIOADRV;
    gpiog_drv = GPIOGDRV;
    gpioa_pu  = GPIOAPU;
    gpiog_pu  = GPIOGPU;
    gpioa_pd  = GPIOAPD;
    gpiog_pd  = GPIOGPD;
    gpiob_fen = GPIOBFEN;
    gpiob_dir = GPIOBDIR;
    gpiob_de  = GPIOBDE;
    gpiob_drv = GPIOBDRV;
    gpiob_pu  = GPIOBPU;
    gpiob_pd  = GPIOBPD;
    pwrcon0 = PWRCON0;
    GPIOAFEN = 0;
    GPIOADIR = 0;
    GPIOADE = 0;
    GPIOADRV = 0;
    GPIOAPU = 0;
    GPIOAPD = 0;

    GPIOBFEN = 0;
    GPIOBDIR = 0;
    GPIOBDE = 0;
    GPIOBDRV = 0;
    GPIOBPU = 0;
    GPIOBPD = 0;

    GPIOGDE = 0x3F;     //MCP FLASH

#if IRRX_SW_EN
    IR_CAPTURE_PORT();

	CLKGAT0 &= ~(BIT(31) | BIT(29) | BIT(30) | BIT(25) | BIT(18) | BIT(16) | BIT(14) | BIT(13) | BIT(11) | BIT(10) | BIT(9) | BIT(6) | BIT(5) | BIT(4));
    CLKGAT1 &= ~(BIT(30) | BIT(28) | BIT(27) | BIT(17)| BIT(15) | BIT(14) | BIT(13)  |  BIT(8) | BIT(4) | BIT(2) | BIT(0));
#endif // IRRX_SW_EN

#if TKEY_SLEEP_MODE
    for(u8 i = 0; i < sizeof(tkey_array); i++) {
        tkey_gpio_config(tkey_array[i]);
    }

	CLKGAT0 &= ~(BIT(31) | BIT(29) | BIT(30) | BIT(25) | BIT(18) | BIT(16) | BIT(14) | BIT(13) | BIT(11) | BIT(10) | BIT(9) | BIT(6) | BIT(5) | BIT(4));
    CLKGAT1 &= ~(BIT(30) | BIT(28) | BIT(27) | BIT(17)| BIT(15) | BIT(14) | BIT(13)  |  BIT(8) | BIT(4) | BIT(2));
    CLKGAT1 |= BIT(1);
#endif

    cpu_ie = PICCON & BIT(0);
//    PICCONCLR = BIT(0);     //关总中断

    gui_display(DISP_OFF);
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    sddaccon = SDDACCON;

    sys_clk = get_cur_sysclk();
    sys_clk_set(SYS_RC2M);
#if (IRRX_SW_EN && FREQ_DET_EN)
    TMR0PR  = (rc2m_freq_cal/2) - 1;                        //timer 1ms
	TMR0CON = (4 << 1) | (0 << 4) | BIT(7) | BIT(0);    //选择rc2m时钟源

    TMR1CON = (0 << 1) | (3 << 4) | (2 << 14) | BIT(7) | BIT(0);
    sys_set_tmr_enable(0, 0);
#endif

#if TKEY_SLEEP_MODE
    TMR0PR = 3000 - 1;
    TMR0CON = (4 << 1) | (0 << 4) | BIT(7) | BIT(0);    //选择rc2m时钟源
#endif
    PWRCON0 = (PWRCON0 & ~(0x1f<<5))  | (0x13 << 5);          //DI_VCORES 0x10 = 2.925
    dac_power_off();                    //dac power down
    CLKGAT0 &= ~BIT(12);
    saradc_ch = bsp_saradc_exit();        //close saradc及相关通路模拟
    led_off();
    rled_off();
    WDT_DIS();
    RTC_WDT_DIS();

    clkdivcon0 = CLKDIVCON0;
    pll0con = PLL0CON;
    PLL0CON &= ~(BIT(15)|BIT(16));
    CLKDIVCON0 = 0;

    while(1) {
        asm("nop");asm("nop");asm("nop");
#if (IRRX_SW_EN && FREQ_DET_EN)
        if(get_irkey() == KEY_IR_POWER) {
            ir_key_clr();
            break;
        }
#endif

#if TKEY_SLEEP_MODE
        if(msg_dequeue() == KU_IR_POWER) {
            break;
        }
#endif
    }

    PLL0CON = pll0con;
    CLKDIVCON0 = clkdivcon0;

    WDT_EN();
    PICCON |= cpu_ie;

    GPIOAFEN = gpioa_fen;
    GPIOGFEN = gpiog_fen;
    GPIOADIR = gpioa_dir;
    GPIOGDIR = gpiog_dir;
    GPIOADE  = gpioa_de;
    GPIOGDE  = gpiog_de;
    GPIOADRV = gpioa_drv;
    GPIOGDRV = gpiog_drv;
    GPIOAPU  = gpioa_pu;
    GPIOGPU  = gpiog_pu;
    GPIOAPD  = gpioa_pd;
    GPIOGPD  = gpiog_pd;
    GPIOBFEN = gpiob_fen;
    GPIOBDIR = gpiob_dir;
    GPIOBDE  = gpiob_de;
    GPIOBDRV = gpiob_drv;
    GPIOBPU  = gpiob_pu;
    GPIOBPD  = gpiob_pd;

    PWRCON0 = pwrcon0;
    CLKGAT0 |= (BIT(31) | BIT(29) | BIT(30) | BIT(25) | BIT(18) | BIT(16) | BIT(14) | BIT(13) | BIT(11) | BIT(10) | BIT(9) | BIT(6) | BIT(5) | BIT(4));
    CLKGAT1 |= (BIT(30) | BIT(28) | BIT(27) | BIT(17)| BIT(15) | BIT(14) | BIT(13)  |  BIT(8) | BIT(4) | BIT(2) | BIT(0));

#if (IRRX_SW_EN && FREQ_DET_EN)
    TMR0PR  = 3000 - 1;                                 //timer 1ms
	TMR0CON = (2 << 1) | (3 << 4) | BIT(7) | BIT(0);    //timer0 clk = xosc24m / 8
    sys_clk_set(sys_clk);              //还原系统时钟
    TMR1CON = (2 << 1) | (3 << 4) | (2 << 14) | BIT(7) | BIT(0);
    sys_set_tmr_enable(1, 1);
#endif

#if TKEY_SLEEP_MODE
    TMR0PR  = 3000 - 1;                                 //timer 1ms
	TMR0CON = (2 << 1) | (3 << 4) | BIT(7) | BIT(0);    //timer0 clk = xosc24m / 8
    sys_clk_set(sys_clk);
#endif
    //dac_restart();
    //dac_power_on();

#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
    CLKGAT0 |= BIT(12);
    SDDACCON = sddaccon;

    saradc_set_channel(saradc_ch);
    bsp_saradc_init();
     //gui_init();
    gui_display(DISP_POWERON);

#if 0
    delay_us(100);
    PLL0CON |= BIT(18);                 //PLL1 SDM enable
    // PLL0CON |= BIT(6);                 //PLL1 Analog enable
    // wakeup_disable();
    //enable adpll
    // PLL0CON |= BIT(12);                //PLL1 LDO enable
    dac_restart();
    bsp_change_volume(sys_cb.vol);
#endif // 0
#endif // IRRX_SW_EN && FREQ_DET_EN
    bsp_change_volume(sys_cb.vol);
    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}

#if SPIFLASH_DP_PDN4_EN
AT(.pdncom_text.lp.sleep)
void spiflash_sendbyte_pdn4(u8 date)
{
    SPI0CON &= ~BIT(4);                             //TX
    SPI0BUF  = date;
    while (!(SPI0CON & BIT(16)));                   //Wait pending
}

AT(.pdncom_text.lp.sleep)
void flashcs_dis_pdn4(void)
{
    GPIOGSET = BIT(2);      //cs
}

AT(.pdncom_text.lp.sleep)
void flashcs_en_pdn4(void)
{
    GPIOGCLR = BIT(2);      //cs
}

AT(.pdncom_text.lp.sleep)
void spiflash0_enter_dp(void)
{
    flashcs_en_pdn4();
    spiflash_sendbyte_pdn4(SPIFLASH_DP);     //enter deep power down
    flashcs_dis_pdn4();
    delay_us(20);
}

AT(.pdncom_text.lp.sleep)
void spiflash0_release_dp(void)
{
    flashcs_en_pdn4();
    spiflash_sendbyte_pdn4(SPIFLASH_RDP);    //release deep power down
    flashcs_dis_pdn4();
    delay_us(20);
}
#endif // SPIFLASH_DP_PDN4_EN

#if (PWR_L3M == POWER_DOWN_MODE) || (PWR_L3M_ALM == POWER_DOWN_MODE)
AT(.pdncom_text.pwroff)
void pwroff_save_to_cache(u8 *src_addr, u32 length)
{
    //需要把RAM0的程序copy到ICACHE RAM上面跑
    u32 i;
    u8 *cp_dst_ptr, *cp_src_ptr;
    cp_src_ptr = (u8*) src_addr;
    cp_dst_ptr = (u8*) 0x72000;
    for(i = 0; i < length; i++){
        *cp_dst_ptr++ = *cp_src_ptr++;
    }
    asm("j 0x72000");
    while(1);
}
#endif

#if (PWR_L4M == POWER_DOWN_MODE) || (PWR_L4M_ALM == POWER_DOWN_MODE) || (POWER_DOWN_MODE == PWR_RTC1S)
AT(.pdncom_text.rtc_save)
void pdn_rtc_sta_save(void)
{
    sp_sfr.piccon       = PICCON;
    sp_sfr.picen        = PICEN;
    sp_sfr.picpr        = PICPR;
    sp_sfr.picpr1       = PICPR1;
    sp_sfr.nmicon       = NMICON;
    sp_sfr.clkgat0      = CLKGAT0;
    sp_sfr.clkgat1      = CLKGAT1;

    sp_sfr.pwrcon0      = PWRCON0;
    sp_sfr.pwrcon1      = PWRCON1;

    sp_sfr.pll0con      = PLL0CON;
    sp_sfr.pll0div      = PLL0DIV;

    sp_sfr.clkcon0      = CLKCON0;
    sp_sfr.clkcon1      = CLKCON1;
    sp_sfr.clkdivcon0   = CLKDIVCON0;
    sp_sfr.auangcon0    = AUANGCON0;

    sp_sfr.cachcon0     = CACHCON0;
    sp_sfr.cachcon1     = CACHCON1;
    sp_sfr.iclock       = ICLOCK;
    sp_sfr.dclock       = DCLOCK;

    sp_sfr.spi0con      = SPI0CON;
    sp_sfr.spi0baud     = SPI0BAUD;

    sp_sfr.funcoutmcon  = FUNCOUTMCON;
    sp_sfr.funcoutcon   = FUNCOUTCON;
    sp_sfr.funcincon    = FUNCINCON;
    sp_sfr.funcomcon[0] = FUNCMCON0;
    sp_sfr.funcomcon[1] = FUNCMCON1;

    sp_sfr.udetbaud     = UART0BAUD;
    sp_sfr.udetcon      = UART0CON;
    sp_sfr.adc_ch       = saradc_get_channel();

    sp_sfr.gpioa_fen     = GPIOAFEN;
    sp_sfr.gpioa_de      = GPIOADE;
    sp_sfr.gpioa_dir     = GPIOADIR;
    sp_sfr.gpioa_drv     = GPIOADRV;
    sp_sfr.gpioa_pu      = GPIOAPU;
    sp_sfr.gpioa_pd      = GPIOAPD;
    sp_sfr.gpioa_pu500k  = GPIOAPU500K;
    sp_sfr.gpioa_pd500k  = GPIOAPD500K;

    sp_sfr.gpiob_fen     = GPIOBFEN;
    sp_sfr.gpiob_de      = GPIOBDE;
    sp_sfr.gpiob_dir     = GPIOBDIR;
    sp_sfr.gpiob_drv     = GPIOBDRV;
    sp_sfr.gpiob_pu      = GPIOBPU;
    sp_sfr.gpiob_pd      = GPIOBPD;
    sp_sfr.gpiob_pu500k  = GPIOBPU500K;
    sp_sfr.gpiob_pd500k  = GPIOBPD500K;

    sp_sfr.gpiog_fen     = GPIOGFEN;
    sp_sfr.gpiog_de      = GPIOGDE;
    sp_sfr.gpiog_dir     = GPIOGDIR;
    sp_sfr.gpiog_drv     = GPIOGDRV;
    sp_sfr.gpiog_pu      = GPIOGPU;
    sp_sfr.gpiog_pd      = GPIOGPD;
    sp_sfr.gpiog_pu500k  = GPIOGPU500K;
    sp_sfr.gpiog_pd500k  = GPIOGPD500K;
}

AT(.pdncom_text.rtc_save)
void pdn_rtc_sta_restore(void)
{
    //唤醒后的处理
    RTCCON12 = 0xFF;                                //disable all reset

    RTCCON |= BIT(1);                               //vrtc voltage select
    RTCCON  = (RTCCON  & ~(0x3<<1))  | (0x3<<1);    //RTC clk
    PWRCON0 = (PWRCON0 & ~(0x1f<<5)) | (0x18<<5);   //VDDIO=3.3v

    RTCCON3 |= 0x7;                                 //ldo enable
    PWRCON0 = (PWRCON0 & ~0x1F) | 0x10;             //VDDCORE is 1.1V
    RTCCON3 &= ~(BIT(18) | BIT(23));

    RTCCON3 |= BIT(5) | BIT(21);
    RTCCON3 &= ~(BIT(3) | BIT(20));

    RTCCON0 |= BIT(26);                             //LP if enable(io enable)
    RTCCON0 |= BIT(27);                             //SRAM0 enable

    CLKGAT0     = sp_sfr.clkgat0;
    CLKGAT1     = sp_sfr.clkgat1;

    RSTCON0 = 0x1f;
    RSTCON0 |= BIT(7);                              // adda clk rstn
    RSTCON0 |= 1<<4;                                //pllsdm reset release

    PWRCON0     = sp_sfr.pwrcon0;
    PWRCON1     = sp_sfr.pwrcon1;

    //第一时间拉高MCP Flash的CS脚，以免CLK翻转影响SPIFlash
    SPI_CS1_DIS();
    SPI_CS1_PDIR &= ~BIT(SPI_CS1);

    PLL0CON     = sp_sfr.pll0con;
    PLL0CON    |= BIT(18);                          //pll sdm enable
    PLL0DIV     = sp_sfr.pll0div;
    PLL0CON    |= BIT(20);                          //update pll0div to pll_clk

    GPIOAFEN    = sp_sfr.gpioa_fen;
    GPIOADE     = sp_sfr.gpioa_de;
    GPIOADIR    = sp_sfr.gpioa_dir;
    GPIOADRV    = sp_sfr.gpioa_drv;
    GPIOAPU     = sp_sfr.gpioa_pu;
    GPIOAPD     = sp_sfr.gpioa_pd;
    GPIOAPU500K = sp_sfr.gpioa_pu500k;
    GPIOAPD500K = sp_sfr.gpioa_pd500k;

    GPIOBFEN    = sp_sfr.gpiob_fen;
    GPIOBDE     = sp_sfr.gpiob_de;
    GPIOBDIR    = sp_sfr.gpiob_dir;
    GPIOBDRV    = sp_sfr.gpiob_drv;
    GPIOBPU     = sp_sfr.gpiob_pu;
    GPIOBPD     = sp_sfr.gpiob_pd;
    GPIOBPU500K = sp_sfr.gpiob_pu500k;
    GPIOBPD500K = sp_sfr.gpiob_pd500k;

    GPIOGFEN    = sp_sfr.gpiog_fen;
    GPIOGDE     = sp_sfr.gpiog_de;
    GPIOGDIR    = sp_sfr.gpiog_dir;
    GPIOGDRV    = sp_sfr.gpiog_drv;
    GPIOGPU     = sp_sfr.gpiog_pu;
    GPIOGPD     = sp_sfr.gpiog_pd;
    GPIOGPU500K = sp_sfr.gpiog_pu500k;
    GPIOGPD500K = sp_sfr.gpiog_pd500k;

    AUANGCON0   = sp_sfr.auangcon0;                 //reset micadc bg

    CLKDIVCON0  = sp_sfr.clkdivcon0;
    CLKCON1     = sp_sfr.clkcon1;
    asm("nop");
    asm("nop");
    asm("nop");
    CLKCON0     = (sp_sfr.clkcon0 & (~0x0c));
    asm("nop");
    asm("nop");
    asm("nop");
    CLKCON0 = sp_sfr.clkcon0;

    LVDCON = (LVDCON & ~0x07) | 1;                  //LVD=2.2V,  LVDCON[2:0]

    PICADR = 0x80000;                               //set rom vector
    ICLOCK = sp_sfr.iclock;
    DCLOCK = sp_sfr.dclock;

    CACHCON0 = BIT(16) | BIT(0);                    //dCache Enable, iCache Enable
    CACHCON0 |= BIT(0);
    CACHCON0 |= BIT(16);
    CLKGAT1  |= BIT(17);
    asm("nop");asm("nop");asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");asm("nop");
    CACHCON0 |= BIT(22) | (0 << 26);
    ICINDEX  = 0;
    ICTAG   &= ~BIT(16);
    ICTAG   |= BIT(17);
    CACHCON1|= BIT(2);

    PICPR       = sp_sfr.picpr;
    PICPR1      = sp_sfr.picpr1;
    PICEN       = sp_sfr.picen;
    NMICON      = sp_sfr.nmicon;
    PICCON      = sp_sfr.piccon;
    NMICON = BIT(0);                                //Cache INT

    dcache_init_l4m();

    EFCON0 |= BIT(3) | BIT(2);                      //EF WP
    MEMCON |= BIT(16);                              //MROM Protect

    FUNCOUTMCON = sp_sfr.funcoutmcon;
    FUNCOUTCON  = sp_sfr.funcoutcon;
    FUNCINCON   = sp_sfr.funcincon;
    FUNCMCON0   = sp_sfr.funcomcon[0];
    FUNCMCON1   = sp_sfr.funcomcon[1];

    UART0BAUD    = sp_sfr.udetbaud;
    UART0CON     = sp_sfr.udetcon;
    UART0CPND = BIT(10) | BIT(15) | BIT(18);          //CLR Pending
    UART0CON = (UART0CON & ~(0xfff << 16)) | (0x5aa << 16);  //KEY MODE RESET

    GPIOGDE &= ~BIT(6);                             //PG6 digital disable(flash power)
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); //2us
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    GPIOGDRV |= BIT(7);                             //DI_MODE=1
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); //2us
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    GPIOGDRV |= BIT(8);                             //DI_SS_FPG=1

    GPIOGDE |= 0x3f;
    GPIOGSET = BIT(3) | BIT(0);                     //HOLD WP OUT HIGH
    GPIOGDIR &= ~(BIT(3) | BIT(0));

    SPI0BAUD    = sp_sfr.spi0baud;
    SPI0CON     = sp_sfr.spi0con;

    memcpy(&__comm_vma, &__comm_lma, (u32)&__comm_size);
    PICADR = (u32)&__comm_vma;

    #if SPIFLASH_DP_PDN4_EN
    spiflash0_release_dp();
    #endif // SPIFLASH_DP_PDN4_EN
}

//pdn快速唤醒后需要初始化一些中断和时钟
void pdn_back_init(void)
{
    timer0_init();
    PICCON |= BIT(0);                               //global interrupt enable
    WDT_EN();

    // clock init
    adpll_init(SYS_CLK_SEL);
    sys_clk_set(SYS_CLK_SEL);

    saradc_set_channel(sp_sfr.adc_ch);
    bsp_saradc_init();
    sys_set_tmr_enable(1, 1);

#if DAC_EN
    dac_init();
#endif
// #if WARNING_TONE_EN || MIDI_DEC_BK_EN
//     music_res_init();
// #endif
// #if SPI_AUDIO_TEST_EN
//     spi_test_init();
// #endif
// #if RGB_SERIAL_EN
//     spi1_led_init();
// #endif
// #if MIDI_EN
//     midi_init();
// #endif
// #if MIDI_REC_EN
//     midi_rec_init();
// #endif
#if EX_SPIFLASH_SUPPORT
    exspiflash_init();
#endif
}
#endif

#if ((PWR_L1M == POWER_DOWN_MODE) || (PWR_L2M == POWER_DOWN_MODE))
void pdn_port_state(void)
{
    //PB10配置为复位引脚，使能上拉电阻
    //关机下不需要用到的IO，都配置为模拟IO，避免漏电
    GPIOADE  = 0x0;
    GPIOBDE  = BIT(10);
    GPIOGDE  = 0x0;
    GPIOAFEN = 0x0;
    GPIOBFEN = 0x0;
    GPIOGFEN = 0x0;

    GPIOGPU  = 0;
    GPIOAPU  = 0;
    GPIOBPU  = BIT(10);
#if 0
    CLKGAT0 &= ~BIT(15);    //disable lp_top clock
    RTCCON0 &= ~BIT(26);    //IO interface disable, io控制寄存器无法写
#else
    CLKGAT0 |= BIT(15);      //enable lp_top clock
    RTCCON0 |= BIT(25);     // enable lpsw_rstn
    RTCCON0 |= BIT(26);     //IO interface enable, io控制寄存器可以写
#endif
}
#endif

#if(PWR_L1M == POWER_DOWN_MODE)
void pdn1(void)
{
#ifdef SNF_RC_EN
    RTCCON0 &= ~(0xff<<8);
    RTCCON0 |= BIT(0);     //enable SNIFF RC
#endif // SNF_RC_EN
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    // printf("pdn1 test, RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON12: %x\n", RTCCON0, RTCCON3, RTCCON4, RTCCON12);
    // printf("GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n", GPIOAPU, GPIOBPU, GPIOGPU);

    pwroff_pmu_voltage_config();
    pdn_port_state();

    RTCCON12 |= 0x3<<4;     //disable GPIO reset

    RTCCON &= ~BIT(0);      //关机选择RTCCON1 BIT8~6

    RTCCON3  = //BIT(19)    //pd_core3
              BIT(7)        //en vddio_aon
            | BIT(6)        //pd_core
            | BIT(4)        //pd_core2
            //| BIT(3)      //rl2vlc
            ;

    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    while(1);
}
#endif

#if(PWR_L2M == POWER_DOWN_MODE)
void pdn2(void)
{
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();
#ifdef SNF_RC_EN
    RTCCON0 &= ~(0xff<<8);
    RTCCON0 |= BIT(0);     //enable SNIFF RC
#endif // SNF_RC_EN

    RTCCON3 &= ~BIT(6);     // dis PD_CORE
    // RTCCON3 |= (1<<2);      // sw vddcore enable
    CLKGAT0 |= (1<<15);     // LP en clock

    // printf("pdn2 down\n");
    RTCCON9 = 0xffffffff;   //clear pending
    RTCCON10 = 0xffffffff;

    // printf("RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON12: %x\n", RTCCON0, RTCCON3, RTCCON4, RTCCON12);
    // printf("GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n", GPIOAPU, GPIOBPU, GPIOGPU);

    pwroff_pmu_voltage_config();

    //唤醒源选择 RTC1S RWDT ALM WKO PORT
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
    wk_cfg.source = WK_PWROFF_GPIO;
    if(POWER_ON_FALL_NUM != 0){
        u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
        memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
    }
    if(POWER_ON_RISE_NUM != 0){
        u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
        memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
    }
    pwroff_wk_source_config(&wk_cfg);
    RTCCON0 |=  BIT(31);        //power_flag
    CLKGAT0 &= ~BIT(15);        //disable lp_top clock
    RTCCON0 &= ~BIT(26);        //IO interface disable, io控制寄存器无法写
    if(MCLR_RESRT_MDOE == 2){
        RTCCON12 |= 0xf3;       //disable GPIO reset
        RTCCON12 &= ~0xc;       //PB10 reset
    }else{
        RTCCON12 |= 0xff;       //disable GPIO reset & PB10 reset
    }

    RTCCON &= ~BIT(0);      //关机选择RTCCON1 BIT8~6
    RTCCON3 = //BIT(19)     //pd_core3
              BIT(7)        //en vddio_aon
            //| BIT(6)      //pd_core
            | BIT(4)        //pd_core2
            | BIT(3)        //rl2vlc
            | BIT(17)       //port wakeup enable
            ;

    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    while(1);
}
#endif

#if(PWR_L3M == POWER_DOWN_MODE) || (PWR_L3M_ALM == POWER_DOWN_MODE)
AT(.pdncom_text.pwroff)
void pdn3_shutdown(void)
{
    CLKGAT0 &= ~BIT(2);      //disable ram0 clock
    RTCCON0 &= ~BIT(27);    //sram0 interface disable

    LPMCON |= BIT(0);
    LPMCON |= BIT(1);       //idle mode
    while(1);
}

AT(.pdncom_text.pwroff)
void pdn3_do(void)
{
    //唤醒源选择 RTC1S RWDT ALM WKO PORT
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
    wk_cfg.lp_type = LP_DEEP_SLEEP;
    wk_cfg.source  = WK_PWROFF_GPIO;
    #if (PWR_L3M == POWER_DOWN_MODE)
        wk_cfg.source  = WK_PWROFF_GPIO;
    #elif (PWR_L3M_ALM == POWER_DOWN_MODE)
        wk_cfg.source  = WK_PWROFF_GPIO | WK_PWROFF_ALARM;
    #endif
#if MIX_PWR_DOWN_EN
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = 1;
        wk_cfg.rise_io_group[0] = MIX_PWR_DOWN_UP_IO;
#else
    #if (WKU_IO_FALL_NUM != 0 && WKU_IO_PWRON_EN)
        u8 wku_fall_io_temp[WKU_IO_FALL_NUM] = WKU_IO_FALL_GROUP;
        wk_cfg.wk_io_fall_en = 1;
        wk_cfg.wk_io_fall_num = WKU_IO_FALL_NUM;
        memcpy(wk_cfg.wk_io_fall_group, wku_fall_io_temp, WKU_IO_FALL_NUM);
    #endif
    #if (WKU_IO_RISE_NUM != 0 && WKU_IO_PWRON_EN)
        u8 wku_rise_io_temp[WKU_IO_RISE_NUM] = WKU_IO_RISE_GROUP;
        wk_cfg.wk_io_rise_en = 1;
        wk_cfg.wk_io_rise_num = WKU_IO_RISE_NUM;
        memcpy(wk_cfg.wk_io_rise_group, wku_rise_io_temp, WKU_IO_RISE_NUM);
    #endif
    #if (POWER_ON_FALL_NUM != 0)
        u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
        memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
    #endif
    #if (POWER_ON_RISE_NUM != 0)
        u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
        memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
    #endif
#endif
    pwroff_wk_source_config(&wk_cfg);

    u32 rtccon3 =
            //BIT(20)           //vdd11(ram power)
              BIT(17)           //port wakeup enable
#if (PWR_L3M_ALM == POWER_DOWN_MODE)
            | BIT(8)            //RTC alarm
#endif
            | BIT(7)            //en vddio_aon
            | BIT(4)            //pd_core2
            | BIT(3)            //rl2vlc
            ;
    rtccon3 &= ~BIT(6);         //dis PD_CORE
    RTCCON3 = rtccon3;
    pwroff_rtc_core_config(&wk_cfg);
    pwroff_save_to_cache((u8 *)pdn3_shutdown, 100);
}

AT(.text.lowpwr.pdn)
void pdn3(void)
{
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();
    RTCCON12 = 0XFF;        //disable all reset

    // printf("RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON12: %x\n", RTCCON0, RTCCON3, RTCCON4, RTCCON12);
    // printf("GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n", GPIOAPU, GPIOBPU, GPIOGPU);

    pwroff_pmu_voltage_config();
    pdn3_do();
}
#endif

#if (POWER_DOWN_MODE == PWR_L4M) ||  (PWR_L4M_ALM == POWER_DOWN_MODE)
AT(.pdncom_text.pdn_shut)
void pdn4_shutdown(void)
{
    u32 rtccon3;
    CLKGAT0 &= ~BIT(2);     //disable ram0 clock
    RTCCON0 &= ~BIT(27);    //sram0 interface disable

    rtccon3 =
              BIT(20)       //vdd11(ram power)
            | BIT(17)       //port wakeup enable
#if (PWR_L4M_ALM == POWER_DOWN_MODE)
            | BIT(8)        //RTC alarm
#endif
            | BIT(7)        //en vddio_aon
            | BIT(4)        //pd_core2
            | BIT(3)        //rl2vlc
            ;
    rtccon3 &= ~BIT(6);     //dis PD_CORE
    RTCCON3 = rtccon3;

    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    while(1);
}

AT(.pdncom_text.pdn4)
void lp_sleep_pdn4_proc(void)
{
    //需要把RAM0的程序copy到ICACHE RAM上面跑
    u32 i;
    u8 *cp_dst_ptr, *cp_src_ptr;
    cp_src_ptr = (u8*) &pdn4_shutdown;
    cp_dst_ptr = (u8*) 0x72000;
    for(i = 0; i < 0x100; i++){
        *cp_dst_ptr++ = *cp_src_ptr++;
    }

    // 1. save some thing to stack
    RTCCON0 |= BIT(25);    //LPRST

    asm("j 0x72000");
    while(1);
}

AT(.pdncom_text.pdn4)
void lp_wakeup_pdn4_proc(void)
{
    //唤醒后的处理
    pdn_rtc_sta_restore();
}

AT(.pdncom_text.pdn4)
void pdn4_do(void)
{
    //唤醒源选择 RTC1S RWDT ALM WKO PORT
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
    wk_cfg.lp_type = LP_DEEP_SLEEP;
    #if (PWR_L4M == POWER_DOWN_MODE)
        wk_cfg.source  = WK_PWROFF_GPIO;
    #elif (PWR_L4M_ALM == POWER_DOWN_MODE)
        wk_cfg.source  = WK_PWROFF_GPIO | WK_PWROFF_ALARM;
    #endif

    #if (WKU_IO_FALL_NUM != 0 && WKU_IO_PWRON_EN)
        u8 wku_fall_io_temp[WKU_IO_FALL_NUM] = WKU_IO_FALL_GROUP;
        wk_cfg.wk_io_fall_en = 1;
        wk_cfg.wk_io_fall_num = WKU_IO_FALL_NUM;
        memcpy(wk_cfg.wk_io_fall_group, wku_fall_io_temp, WKU_IO_FALL_NUM);
    #endif
    #if (WKU_IO_RISE_NUM != 0 && WKU_IO_PWRON_EN)
        u8 wku_rise_io_temp[WKU_IO_RISE_NUM] = WKU_IO_RISE_GROUP;
        wk_cfg.wk_io_rise_en = 1;
        wk_cfg.wk_io_rise_num = WKU_IO_RISE_NUM;
        memcpy(wk_cfg.wk_io_rise_group, wku_rise_io_temp, WKU_IO_RISE_NUM);
    #endif
    #if (POWER_ON_FALL_NUM != 0)
        u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
        memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
    #endif
    #if (POWER_ON_RISE_NUM != 0)
        u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
        memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
    #endif

    pwroff_wk_source_config(&wk_cfg);
    pwroff_rtc_core_config(&wk_cfg);

    RTCCON0 = (RTCCON0 & ~(0x7<<22)) | (2<<22);     //sys reset delay
    RTCCON0 |= BIT(30);     //low power user flag
    lp_sleep(lp_sleep_pdn4_proc, lp_wakeup_pdn4_proc);
}

AT(.pdncom_text.pdn4)
void pdn4(void)
{
    pdn_rtc_sta_save();
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();
    RTCCON12 = 0XFF;        //disable all reset

    // printf("RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON12: %x\n", RTCCON0, RTCCON3, RTCCON4, RTCCON12);
    // printf("GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n", GPIOAPU, GPIOBPU, GPIOGPU);

    pwroff_pmu_voltage_config();
    pdn4_do();
    pdn_back_init();
}
#endif

#if(POWER_DOWN_MODE == PWR_RTC1S)
ALIGNED(512)
AT(.text.lowpwr.rtc1s)
void pdn_rtc1s(void)
{
    bool bo_loop = true;
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    pdn_rtc_sta_save();
    RTCCON12 |= 0xf3;       //disable GPIO reset
    RTCCON12 &= ~0xc;       //PA11 reset
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();

    pwroff_pmu_voltage_config();

    while(bo_loop){
        //唤醒源选择 RTC1S RWDT ALM WKO PORT
        memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
        wk_cfg.source = WK_PWROFF_RTC1S;
        if(POWER_ON_FALL_NUM != 0){
            u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
            wk_cfg.wk_fall_en = 1;
            wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
            memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
        }
        if(POWER_ON_RISE_NUM != 0){
            u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
            wk_cfg.wk_rise_en = 1;
            wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
            memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
        }
        pwroff_wk_source_config(&wk_cfg);
        RTCCON0 |=  BIT(31);        //power_flag
        CLKGAT0 &= ~BIT(15);        //disable lp_top clock
        RTCCON0 &= ~BIT(26);        //IO interface disable, io控制寄存器无法写
    #if PWR_RTC1S_WK_MODE
        RTCCON &= ~BIT(0);      //关机选择RTCCON1 BIT8~6
        CLKCON0 = (CLKCON0 & ~(0X3<<2)) | (0X0<<2);
        RTCCON3 = //BIT(19)     //pd_core3
                //   BIT(7)        //en vddio_aon
                // | BIT(4)        //pd_core2
                // | BIT(3)        //rl2vlc
                // | BIT(17)       //port wakeup enable
                  BIT(6)        //pd_core
                | BIT(9)
                ;
        LPMCON |= BIT(0);                           //sleep mode
        LPMCON |= BIT(1);                           //idle mode
        while(1);
    #else
        PWRCON1 &= ~0xF;    //disable flash PG
        CLKCON0 &= ~0xC;    //SYS clock switch to RC
        asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
        PLL0CON = 0;        //disable PLL
        PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x8<<0);    //DI_VCORES 0x13 = 1.275V

        LPMCON |= BIT(0);                           //sleep mode
        LPMCON |= BIT(1);                           //idle mode
        asm("nop");asm("nop");asm("nop");

    //唤醒后的处理
        pdn_rtc_sta_restore();

        GPIOADE  |= BIT(7);
        GPIOADIR |= BIT(7);
        GPIOAPU  |= BIT(7);
        u32 key_cnt = 0;
        u32 chk_cnt = 50;
        while(chk_cnt > 0) {
            chk_cnt--;
            if(GPIOA & BIT(7)) {                    //输入高电平
                key_cnt = 0;
            } else {                                //有IO按键按下
                key_cnt++;
            }
            if (key_cnt > 20) {
                bo_loop = false;
                break;
            }
        }
        // GPIOAFEN &= ~BIT(5);
        // GPIOADE  |=  BIT(5);
        // GPIOADIR &= ~BIT(5);
        // WDT_CLR();
        // GPIOASET = BIT(5);
        // asm("nop");asm("nop");asm("nop");
        // GPIOACLR = BIT(5);
    #endif
    }
}
#endif

#if(PWR_RUNRC == POWER_DOWN_MODE)
ALIGNED(512)
AT(.text.lowpwr.runrc)
void pdn_runrc(void)
{
    u16 gpioa_fen, gpiog_fen;
    u16 gpioa_dir, gpiog_dir;
    u16 gpioa_de, gpiog_de;
    u16 gpioa_drv, gpiog_drv;
    u16 gpioa_pu, gpiog_pu;
    u16 gpioa_pd, gpiog_pd;

    u16 adc_ch;
    u8 sys_clk;
    u32 cpu_ie;
    printf("%s poweroff\n", __func__);

    gui_display(DISP_OFF);
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
    dac_power_off();                    //dac power down

    led_off();
    rled_off();

    sys_clk = get_cur_sysclk();
    sys_clk_set(SYS_RC2M);
    PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x10<<0);           //DI_VCORES 0x10 = 1.2
    sys_set_tmr_enable(0, 0);
    adc_ch = bsp_saradc_exit();
    saradc_clr_channel(adc_ch);
    saradc_set_channel(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP));  //保留VBAT检测

    //close pll0
    PLL0CON &= ~BIT(18);                //pll10 sdm enable disable
    // PLL0CON &= ~BIT(6);                 //pll0 analog diable
    // PLL0CON &= ~BIT(12);                //pll0 ldo disable

    //io save
    gpioa_fen = GPIOAFEN;
    gpiog_fen = GPIOGFEN;
    gpioa_dir = GPIOADIR;
    gpiog_dir = GPIOGDIR;
    gpioa_de  = GPIOADE;
    gpiog_de  = GPIOGDE;
    gpioa_drv = GPIOADRV;
    gpiog_drv = GPIOGDRV;
    gpioa_pu  = GPIOAPU;
    gpiog_pu  = GPIOGPU;
    gpioa_pd  = GPIOAPD;
    gpiog_pd  = GPIOGPD;

    // my_printf("gpioapu 0: %X\n", gpioapu);
    GPIOAFEN = 0;
    GPIOADIR = 0;
    GPIOADE = 0;
    GPIOADRV = 0;
    GPIOAPU = 0;
    GPIOAPD = 0;

    // GPIOGFEN = 0;
    // GPIOGDIR = 0;
    GPIOGDE = 0x3F;     //MCP FLASH
    // GPIOGDRV = 0;
    // GPIOGPU = 0;
    // GPIOGPD = 0;

    // wakeup_disable();
    set_all_pin_off();
    cpu_ie = PICCON & BIT(0);
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();

    u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
    GPIOADE  |= BIT(fall_io_temp[0]-1);
    GPIOADIR |= BIT(fall_io_temp[0]-1);
    GPIOAPU  |= BIT(fall_io_temp[0]-1);
    u32 key_cnt = 0;
    u32 io_sta = GPIOA;
    while ((io_sta & BIT(fall_io_temp[0]-1)) == 0){         //等待IO松开
        io_sta = GPIOA;
        delay_us(5000);
    }
    while(1) {
        if(GPIOA & BIT(fall_io_temp[0]-1)) {                //输入高电平
            key_cnt = 0;
        } else {                                            //有IO按键按下
            key_cnt++;
            delay_us(5000);
        }
        if (key_cnt > 200) {
            break;
        }
    }

    PICCON |= cpu_ie;
    WDT_EN();

    GPIOAFEN = gpioa_fen;
    GPIOGFEN = gpiog_fen;
    GPIOADIR = gpioa_dir;
    GPIOGDIR = gpiog_dir;
    GPIOADE  = gpioa_de;
    GPIOGDE  = gpiog_de;
    GPIOADRV = gpioa_drv;
    GPIOGDRV = gpiog_drv;
    GPIOAPU  = gpioa_pu;
    GPIOGPU  = gpiog_pu;
    GPIOAPD  = gpioa_pd;
    GPIOGPD  = gpiog_pd;

    // wakeup_disable();

    //enable adpll
    // PLL0CON |= BIT(12);                //PLL1 LDO enable
    delay_us(100);
    PLL0CON |= BIT(18);                 //PLL1 SDM enable
    // PLL0CON |= BIT(6);                 //PLL1 Analog enable
    if(sys_clk == SYS_160M){
        PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x13<<0);       //DI_VCORES 0x13 = 1.275V
    }else if (sys_clk > SYS_128M) {
        PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x11<<0);       //DI_VCORES 0x10 = 1.225V
    }
    sys_clk_set(sys_clk);              //还原系统时钟
    saradc_set_channel(adc_ch);
    bsp_saradc_init();
    sys_set_tmr_enable(1, 1);

    dac_restart();
    bsp_change_volume(sys_cb.vol);
    // gui_init();
    // gui_display(DISP_POWERON);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif

    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}
#endif

#if(PWR_ALARM == POWER_DOWN_MODE)
AT(.pdncom_text.pdn_alm)
void pdn_alarm_do(void)
{
    //唤醒源选择 RTC1S RWDT ALM WKO PORT
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
    wk_cfg.lp_type = LP_DEEP_SLEEP;
    wk_cfg.source  = WK_PWROFF_ALARM;
    if(POWER_ON_FALL_NUM != 0){
        u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
        memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
    }
    if(POWER_ON_RISE_NUM != 0){
        u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
        memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
    }
    pwroff_wk_source_config(&wk_cfg);

    u32 rtccon3  =
              BIT(8)            //RTC alarm
            | BIT(7)            //en vddio_aon
            | BIT(4)            //pd_core2
            | BIT(3)            //rl2vlc
            ;
    rtccon3 &= ~BIT(6);         //dis PD_CORE
    RTCCON3 = rtccon3;
    pwroff_rtc_core_config(&wk_cfg);
    asm("nop");asm("nop");asm("nop");
    while(1);
}

AT(.pdncom_text.pdn_alm)
void pdn_alarm(void)
{
    u32 rtccon3;
    printf("%s poweroff\n", __func__);
    set_all_pin_off();
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();
    RTC_WDT_DIS();
    RTCCON12 = 0XFF;        //disable all reset

    // printf("RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON12: %x\n", RTCCON0, RTCCON3, RTCCON4, RTCCON12);
    // printf("GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n", GPIOAPU, GPIOBPU, GPIOGPU);

    pwroff_pmu_voltage_config();
    pdn_alarm_do();
}
#endif

AT(.pdncom_text.pwroff)
void pwron_fall_set_do(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR |=  (BIT(io_num-1-IO_PA15));
        GPIOBPU  |=  (BIT(io_num-1-IO_PA15));
        GPIOBPD  &= ~(BIT(io_num-1-IO_PA15));       //取消相应脚的下拉
    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR |=  (BIT(io_num-1));
        GPIOAPU  |=  (BIT(io_num-1));
        GPIOAPD  &= ~(BIT(io_num-1));
    }
}

void pwron_fall_set_do_flash(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR |=  (BIT(io_num-1-IO_PA15));
        GPIOBPU  |=  (BIT(io_num-1-IO_PA15));
        GPIOBPD  &= ~(BIT(io_num-1-IO_PA15));       //取消相应脚的下拉
    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR |=  (BIT(io_num-1));
        GPIOAPU  |=  (BIT(io_num-1));
        GPIOAPD  &= ~(BIT(io_num-1));
    }
}

AT(.pdncom_text.pwroff)
void pwron_rise_set_do(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR |=  (BIT(io_num-1-IO_PA15));
        GPIOBPD  |=  (BIT(io_num-1-IO_PA15));
        GPIOBPU  &= ~(BIT(io_num-1-IO_PA15));       //取消相应脚的上拉
    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR |=  (BIT(io_num-1));
        GPIOAPD  |=  (BIT(io_num-1));
        GPIOAPU  &= ~(BIT(io_num-1));
    }
}

void pwron_rise_set_do_flash(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR |=  (BIT(io_num-1-IO_PA15));
        GPIOBPD  |=  (BIT(io_num-1-IO_PA15));
        GPIOBPU  &= ~(BIT(io_num-1-IO_PA15));       //取消相应脚的上拉
    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR |=  (BIT(io_num-1));
        GPIOAPD  |=  (BIT(io_num-1));
        GPIOAPU  &= ~(BIT(io_num-1));
    }
}

AT(.pdncom_text.pwroff)
void wku_io_fall_set_do(u8 pin_num)
{
    if(pin_num == WKU_PA7){
        GPIOAFEN &= ~BIT(7);
        GPIOADE  |=  BIT(7);
        GPIOADIR |=  BIT(7);
        GPIOAPU  |=  BIT(7);
        GPIOAPD  &= ~BIT(7);
    }else if(pin_num == WKU_PB0){
        GPIOBFEN &= ~BIT(0);
        GPIOBDE  |=  BIT(0);
        GPIOBDIR |=  BIT(0);
        GPIOBPU  |=  BIT(0);
        GPIOBPD  &= ~BIT(0);
    }else if(pin_num == WKU_PB1){
        GPIOBFEN &= ~BIT(1);
        GPIOBDE  |=  BIT(1);
        GPIOBDIR |=  BIT(1);
        GPIOBPU  |=  BIT(1);
        GPIOBPD  &= ~BIT(1);
    }else if(pin_num == WKU_PB2){
        GPIOBFEN &= ~BIT(2);
        GPIOBDE  |=  BIT(2);
        GPIOBDIR |=  BIT(2);
        GPIOBPU  |=  BIT(2);
        GPIOBPD  &= ~BIT(2);
    }else if(pin_num == WKU_PB9){
        GPIOBFEN &= ~BIT(9);
        GPIOBDE  |=  BIT(9);
        GPIOBDIR |=  BIT(9);
        GPIOBPU  |=  BIT(9);
        GPIOBPD  &= ~BIT(9);
    }else if(pin_num == WKU_PA0){
        GPIOAFEN &= ~BIT(0);
        GPIOADE  |=  BIT(0);
        GPIOADIR |=  BIT(0);
        GPIOAPU  |=  BIT(0);
        GPIOAPD  &= ~BIT(0);
    }
}

AT(.pdncom_text.pwroff)
void wku_io_rise_set_do(u8 pin_num)
{
    if(pin_num == WKU_PA7){
        GPIOAFEN &= ~BIT(7);
        GPIOADE  |=  BIT(7);
        GPIOADIR |=  BIT(7);
        GPIOAPD  |=  BIT(7);
        GPIOAPU  &= ~BIT(7);
    }else if(pin_num == WKU_PB0){
        GPIOBFEN &= ~BIT(0);
        GPIOBDE  |=  BIT(0);
        GPIOBDIR |=  BIT(0);
        GPIOBPD  |=  BIT(0);
        GPIOBPU  &= ~BIT(0);
    }else if(pin_num == WKU_PB1){
        GPIOBFEN &= ~BIT(1);
        GPIOBDE  |=  BIT(1);
        GPIOBDIR |=  BIT(1);
        GPIOBPD  |=  BIT(1);
        GPIOBPU  &= ~BIT(1);
    }else if(pin_num == WKU_PB2){
        GPIOBFEN &= ~BIT(2);
        GPIOBDE  |=  BIT(2);
        GPIOBDIR |=  BIT(2);
        GPIOBPD  |=  BIT(2);
        GPIOBPU  &= ~BIT(2);
    }else if(pin_num == WKU_PB9){
        GPIOBFEN &= ~BIT(9);
        GPIOBDE  |=  BIT(9);
        GPIOBDIR |=  BIT(9);
        GPIOBPD  |=  BIT(9);
        GPIOBPU  &= ~BIT(9);
    }else if(pin_num == WKU_PA0){
        GPIOAFEN &= ~BIT(0);
        GPIOADE  |=  BIT(0);
        GPIOADIR |=  BIT(0);
        GPIOAPD  |=  BIT(0);
        GPIOAPU  &= ~BIT(0);
    }
}

#if MUL_PWRON_IO_EN
extern const u8 mulpwr_fall_group[];
extern const u8 mulpwr_rise_group[];

void mul_pwron_set(void)
{
#if (MUL_PWRON_FALL_NUM != 0)
    for (int i = 0; i < MUL_PWRON_FALL_NUM; i++) {
        pwron_fall_set_do(mulpwr_fall_group[i]);
    }
#endif
#if (MUL_PWRON_RISE_NUM != 0)
    for (int i = 0; i < MUL_PWRON_RISE_NUM; i++) {
        pwron_rise_set_do(mulpwr_rise_group[i]);
    }
#endif
}

void mul_pwron_fall_int_do(u8 io_num)
{
    PORTINTEDG |=  BIT(io_num-1);      //IO fall edge
    PORTINTEN  |=  BIT(io_num-1);      //enable IO interrupt
    WKUPEDG |= BIT(6);                 //fall edge
    WKUPCON |= BIT(6);                 //6 enable
    WKUPCPND = BIT(22);                //clear 6 pending
    WKUPCON |= 0x1<<20;                //fall filter select
    WKUPCON |= BIT(17);                //wakup sniff enable
}

void mul_pwron_rise_int_do(u8 io_num)
{
    PORTINTEDG &= ~BIT(io_num-1);      //IO rise edge
    PORTINTEN  |=  BIT(io_num-1);      //enable IO interrupt
    WKUPEDG &= ~BIT(7);                //rise edge
    WKUPCON |=  BIT(7);                //7 enable
	WKUPCPND =  BIT(23);			   //clear 7 pending
    WKUPCON |= 0x1<<22;                //rise filter select
    WKUPCON |= BIT(17);                //wakup sniff enable
}

void mul_pwron_int_set(void)
{
#if (MUL_PWRON_FALL_NUM != 0)
    for (int i = 0; i < MUL_PWRON_FALL_NUM; i++) {
        mul_pwron_fall_int_do(mulpwr_fall_group[i]);
    }
#endif
#if (MUL_PWRON_RISE_NUM != 0)
    for (int i = 0; i < MUL_PWRON_RISE_NUM; i++) {
        mul_pwron_rise_int_do(mulpwr_rise_group[i]);
    }
#endif
}
#endif

//设置所有IO,避免关机后漏电
AT(.pdncom_text.pwroff)
void set_all_pin_off(void)
{
    GPIOAFEN = 0;
    GPIOADE  = 0;
    GPIOADIR = 0;
    GPIOADRV = 0;
    GPIOAPU  = 0;
    GPIOAPD  = 0;
    GPIOAPU500K = 0x0;
    GPIOAPD500K = 0x0;

    GPIOBFEN = 0;
    GPIOBDE  = 0;
    GPIOBDIR = 0;
    GPIOBDRV = 0;
    GPIOBPU  = 0;
    GPIOBPD  = 0;
    GPIOBPU500K = 0x0;
    GPIOBPD500K = 0x0;

    GPIOGFEN = 0;
    GPIOGDE  = 0;
    GPIOGDIR = 0;
    GPIOGDRV = 0;       //close flash pg
    GPIOGPU  = 0;
    GPIOGPD  = 0;
    GPIOGPU500K = 0x0;
    GPIOGPD500K = 0x0;
}

AT(.pdncom_text.pwroff)
void wk_gpio_pin_init_do(pwroff_wk_t *wkcfg)
{
    if(wk_cfg.wk_io_fall_en){
        for (int i = 0; i < wk_cfg.wk_io_fall_num; i++) {
            wku_io_fall_set_do(wk_cfg.wk_io_fall_group[i]);
        }
    }
    if(wk_cfg.wk_fall_en){
        for (int i = 0; i < wk_cfg.fall_io_num; i++) {
            pwron_fall_set_do(wk_cfg.fall_io_group[i]);
        }
    }
    if((!wk_cfg.wk_io_fall_en) && (!wk_cfg.wk_fall_en)){
        GPIOBPU = 0;
        GPIOAPU = 0;
    }

    if(wk_cfg.wk_io_rise_en){
        for (int i = 0; i < wk_cfg.wk_io_rise_num; i++) {
            wku_io_rise_set_do(wk_cfg.wk_io_rise_group[i]);
        }
    }
    if(wk_cfg.wk_rise_en){
        for (int i = 0; i < wk_cfg.rise_io_num; i++) {
            pwron_rise_set_do(wk_cfg.rise_io_group[i]);
        }
    }
    if((!wk_cfg.wk_io_rise_en) && (!wk_cfg.wk_rise_en)){
        GPIOBPD = 0;
        GPIOAPD = 0;
    }
}

AT(.pdncom_text.pwroff)
void wk_gpio_wakeup_init_do(pwroff_wk_t *wkcfg)
{
    if(wk_cfg.wk_io_fall_en){
        for (int i = 0; i < wk_cfg.wk_io_fall_num; i++) {
            WKUPEDG  |= BIT(wk_cfg.wk_io_fall_group[i]);        //fall edge
            WKUPCON  |= BIT(wk_cfg.wk_io_fall_group[i]);        //wakeup enable
            WKUPCPND |= BIT(wk_cfg.wk_io_fall_group[i] + 16);   //clear wakeup pending
        }
        // WKUPCON |= 0x1<<20;                                  //fall filter select
    }
    if(wk_cfg.wk_io_rise_en){
        for (int i = 0; i < wk_cfg.wk_io_rise_num; i++) {
            WKUPEDG  &= ~BIT(wk_cfg.wk_io_rise_group[i]);       //rise edge
            WKUPCON  |=  BIT(wk_cfg.wk_io_rise_group[i]);       //wakeup enable
            WKUPCPND |=  BIT(wk_cfg.wk_io_rise_group[i] + 16);  //clear wakeup pending
        }
        // WKUPCON |= 0x1<<22;                                  //fall filter select
    }
    if(wk_cfg.wk_fall_en){
        for (int i = 0; i < wk_cfg.fall_io_num; i++) {
            PORTINTEDG |=  BIT(wk_cfg.fall_io_group[i]-1);      //IO fall edge
            PORTINTEN  |=  BIT(wk_cfg.fall_io_group[i]-1);      //enable IO interrupt
        }
        WKPINMAP = (WKPINMAP & ~(0x3<<12)) | (0x3<<12);         //all pin fall edge wakeup en
        WKUPEDG |= BIT(6);                                      //fall edge
        WKUPCON |= BIT(6);                                      //6 enable
        WKUPCPND = BIT(22);                                     //clear 6 pending
        WKUPIE = (WKUPIE & ~(0xf<<16)) | (1<<16);       		//fall filter select
    }
    if(wk_cfg.wk_rise_en){
        for (int i = 0; i < wk_cfg.rise_io_num; i++) {
            PORTINTEDG &= ~BIT(wk_cfg.rise_io_group[i]-1);      //IO rise edge
            PORTINTEN  |=  BIT(wk_cfg.rise_io_group[i]-1);      //enable IO interrupt
        }
        WKPINMAP = (WKPINMAP & ~(0x3<<14)) | (0x3<<14);         //all pin rise edge wakeup en
        WKUPEDG &= ~BIT(7);                                     //rise edge
        WKUPCON |=  BIT(7);                                     //7 enable
        WKUPCPND =  BIT(23);                                    //clear 7 pending
        WKUPIE = (WKUPIE & ~(0xf<<18)) | (1<<18);       		//rise filter select
    }

    WKUPIE |= BIT(31);                                          //wakup sniff enable
}

AT(.pdncom_text.wk_source)
void pwroff_wk_source_config(pwroff_wk_t *wkcfg)
{
    u32 rtccon;
    if(wkcfg){
        rtccon   = RTCCON;
        // my_printf("wkcfg->source: %X\n", wkcfg->source);
        if(wkcfg->source & WK_PWROFF_ALARM){
            //clk_clk32k_rtc_set
            u32 rtccon0 = RTCCON0;
            rtccon0 = (rtccon0 & ~(0x03 << 8)) | ((CLK32K_RTC_RC2M_RTC & 0x03) << 8);
            RTCCON0 = rtccon0;

            RTCALM = 0;
            u32 rc2m_clk = rc2m_clk_nhz_get_l4m(1);

            //rtc_prescale_set
            u32 rtccon2 = RTCCON2;
            rtccon2 = (rtccon2 & ~0x1ffff) | (((rc2m_clk/64) - 1) & 0x1ffff);
            RTCCON2 = rtccon2;

            RTCALM = (u32)(rtc_get_alarm() + PWR_ALARM_WK_TIME);

            RTCCON0 |= BIT(0) | BIT(2);                     //RC enable & RTC output clock to core enable
            RTCCNT   = 0;
            CLKCON0  = (CLKCON0 & ~(0x3<<2)) | (0x0<<2);    //sys_clk switch to RC
            asm("nop");
            asm("nop");
            asm("nop");
            rtccon  |= BIT(8);                              //RTC alarm wakeup sleep enable
        }
        if(wkcfg->source & WK_PWROFF_RTC1S){
            RTCCPND  = BIT(18);                             //RTC wakeup pending
            #if !PWR_RTC1S_WK_MODE
            rtccon  |= BIT(7);                              //RTC1s wakeup enable
            #endif
            RTCCON0 |= BIT(0);                              //RC enable
            RTCCON0  = (RTCCON0 & ~(0x3<<8)) | (0x1<<8);    //clk2m_rtc sel
            RTCCON2  = PWR_RTC1S_WK_TIME;                   //timer PR
            RTCCNT   = 0;
        }
        if(wkcfg->source & WK_PWROFF_GPIO){
            // my_printf("WK_PWROFF_GPIO\n");
            wk_gpio_pin_init_do(wkcfg);                     //开机IO初始化上下拉配置

            #if MUL_PWRON_IO_EN
                mul_pwron_set();
            #endif

            if(MCLR_RESRT_MDOE == 2){
                GPIOADE |= BIT(7);
                GPIOAPU |= BIT(7);
            }

            #if SPIFLASH_DP_PDN4_EN
            spiflash0_enter_dp();
            #endif // SPIFLASH_DP_PDN4_EN

            wk_gpio_wakeup_init_do(wkcfg);                  //开机IO初始化wakeup唤醒配置

            #if MUL_PWRON_IO_EN
                mul_pwron_int_set();
            #endif
        }
        if(wkcfg->lp_type == LP_DEEP_SLEEP){
            rtccon |= BIT(5);       //when wakeup ,will reset system
        }else{
            rtccon &= ~BIT(5);
        }

        PWRCON1  = (PWRCON1 & ~(0XF<<0));   //close flash pg
        AUANGCON0 &= ~BIT(0);               //close micadc bg

        CLKGAT0 &= ~BIT(15);        //disable lp_top clock
        RTCCON0 &= ~BIT(26);        //IO interface disable, io控制寄存器无法写

        rtccon  &= ~BIT(0);         //关机选择RTCCON1 BIT8~6
        RTCCON  = rtccon;
    }
}

AT(.text.lowpwr.pwroff)
void pwroff_do(void)
{
    #if(PWR_L1M == POWER_DOWN_MODE)
        pdn1();
    #elif(PWR_L2M == POWER_DOWN_MODE)
        pdn2();
    #elif(PWR_L3M == POWER_DOWN_MODE) || (PWR_L3M_ALM == POWER_DOWN_MODE)
        pdn3();
    #elif(PWR_L4M == POWER_DOWN_MODE) || (PWR_L4M_ALM == POWER_DOWN_MODE)
        pdn4();
    #elif(PWR_RTC1S == POWER_DOWN_MODE)
        pdn_rtc1s();
    #elif(PWR_RUNRC == POWER_DOWN_MODE)
        pdn_runrc();
    #elif(PWR_ALARM == POWER_DOWN_MODE)
        pdn_alarm();
    #endif

#if WARNING_POWER_ON
    lsbc_res_play(RES_BUF_SBC_POWERON_LSBC, RES_LEN_SBC_POWERON_LSBC);
#endif
}

#if (!SOFT_POWER_ON_OFF) || MIX_PWR_DOWN_EN
ALIGNED(512)
AT(.text.lowpwr.lowbat)
void sfunc_lowbat_do(void)
{
    sp_sfr.piccon   = PICCON;
    sp_sfr.picen    = PICEN;
    sp_sfr.picpr    = PICPR;
    sp_sfr.nmicon   = NMICON;
    sp_sfr.clkgat0  = CLKGAT0;
    sp_sfr.clkgat1  = CLKGAT1;

    sp_sfr.pwrcon0  = PWRCON0;
    sp_sfr.pwrcon1  = PWRCON1;

    sp_sfr.pll0con  = PLL0CON;
    sp_sfr.pll0div  = PLL0DIV;

    sp_sfr.clkcon0  = CLKCON0;
    sp_sfr.clkcon1  = CLKCON1;
    sp_sfr.clkdivcon0 = CLKDIVCON0;

    sp_sfr.funcoutmcon  = FUNCOUTMCON;
    sp_sfr.funcoutcon   = FUNCOUTCON;
    sp_sfr.funcincon    = FUNCINCON;
    sp_sfr.funcomcon[0] = FUNCMCON0;
    sp_sfr.funcomcon[1] = FUNCMCON1;

    sp_sfr.udetbaud = UART0BAUD;
    sp_sfr.udetcon  = UART0CON;
    sp_sfr.adc_ch   = bsp_saradc_exit();

    sp_sfr.gpioa_fen = GPIOAFEN;
    sp_sfr.gpiog_fen = GPIOGFEN;
    sp_sfr.gpioa_dir = GPIOADIR;
    sp_sfr.gpiog_dir = GPIOGDIR;
    sp_sfr.gpioa_de  = GPIOADE;
    sp_sfr.gpiog_de  = GPIOGDE;
    sp_sfr.gpioa_drv = GPIOADRV;
    sp_sfr.gpiog_drv = GPIOGDRV;
    sp_sfr.gpioa_pu  = GPIOAPU;
    sp_sfr.gpiog_pu  = GPIOGPU;
    sp_sfr.gpioa_pd  = GPIOAPD;
    sp_sfr.gpiog_pd  = GPIOGPD;
#if 0   //使能会导致mic bias抖动一下
    dac_power_off();                                //dac power down
    pwroff_mic_bias_on();
#endif

    RTCCON12 |= 3<<4;    //gpio reset disable
    PICCONCLR = BIT(0); //关总中断
    WDT_DIS();
    RTC_WDT_DIS();

    //唤醒源选择 RTC1S RWDT ALM WKO PORT
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
    wk_cfg.source = WK_PWROFF_GPIO;
    #if MIX_PWR_DOWN_EN
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = 1;
        wk_cfg.fall_io_group[0] = MIX_PWR_SLEEP_UP_IO;
    #else
    if(POWER_ON_FALL_NUM != 0){
        u8 fall_io_temp[POWER_ON_FALL_NUM] = POWER_ON_FALL_IO;
        wk_cfg.wk_fall_en = 1;
        wk_cfg.fall_io_num = POWER_ON_FALL_NUM;
        memcpy(wk_cfg.fall_io_group, fall_io_temp, POWER_ON_FALL_NUM);
    }
    if(POWER_ON_RISE_NUM != 0){
        u8 rise_io_temp[POWER_ON_RISE_NUM] = POWER_ON_RISE_IO;
        wk_cfg.wk_rise_en = 1;
        wk_cfg.rise_io_num = POWER_ON_RISE_NUM;
        memcpy(wk_cfg.rise_io_group, rise_io_temp, POWER_ON_RISE_NUM);
    }
    #endif
    pwroff_wk_source_config(&wk_cfg);

    PWRCON1 &= ~0xF;    //disable flash PG
    CLKCON0 &= ~0xC;    //SYS clock switch to RC
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    PLL0CON = 0;        //disable PLL
    PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x8<<0);    //DI_VCORES 0x13 = 1.275V

    LPMCON |= BIT(0);                           //sleep mode
    //LPMCON |= BIT(1);                           //idle mode
    asm("nop");asm("nop");asm("nop");

//唤醒后的处理
    PWRCON0     = sp_sfr.pwrcon0;

    PWRCON1 &= ~(0xf << 0);                         //软开机时，关掉flash power gate
    PWRCON1    |= BIT(4);                           //VDDF pulldown
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); //至少4us
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    PWRCON1 &= ~BIT(4);                             //flash power gate pull down disable
    PWRCON1 |= BIT(0) | BIT(1) | BIT(2);
    asm("nop"); asm("nop"); asm("nop"); asm("nop");
    PWRCON1 |= BIT(3);

    //第一时间拉高MCP Flash的CS脚，以免CLK翻转影响SPIFlash
    SPI_CS1_DIS();
    SPI_CS1_PDIR &= ~BIT(SPI_CS1);

    CLKGAT0     = sp_sfr.clkgat0;
    CLKGAT1     = sp_sfr.clkgat1;

    PLL0DIV     = sp_sfr.pll0div;
    PLL0CON     = sp_sfr.pll0con;
    PLL0CON    |= BIT(20);                          //update pll0div to pll_clk
    PLL0CON    |= BIT(18);                          //pll sdm enable

    GPIOAFEN    = sp_sfr.gpioa_fen;
    GPIOADIR    = sp_sfr.gpioa_dir;
    GPIOADE     = sp_sfr.gpioa_de;
    GPIOADRV    = sp_sfr.gpioa_drv;
    GPIOAPU     = sp_sfr.gpioa_pu;
    GPIOAPD     = sp_sfr.gpioa_pd;

    GPIOGFEN    = sp_sfr.gpiog_fen;
    GPIOGDIR    = sp_sfr.gpiog_dir;
    GPIOGDE     = sp_sfr.gpiog_de;
    GPIOGDRV    = sp_sfr.gpiog_drv;
    GPIOGPU     = sp_sfr.gpiog_pu;
    GPIOGPD     = sp_sfr.gpiog_pd;

    CLKDIVCON0  = sp_sfr.clkdivcon0;
    CLKCON1     = sp_sfr.clkcon1;
    asm("nop");
    asm("nop");
    asm("nop");
    CLKCON0     = (sp_sfr.clkcon0 & (~0x0c));
    asm("nop");
    asm("nop");
    asm("nop");
    CLKCON0 = sp_sfr.clkcon0;

    LVDCON = (LVDCON & ~0x07) | 1;                  //LVD=2.2V,  LVDCON[2:0]
    PICADR = (u32)&__comm_vma;
    CACHCON0 = BIT(16) | BIT(0);                    //dCache Enable, iCache Enable

    EFCON0 |= BIT(3) | BIT(2);                      //EF WP
    MEMCON |= BIT(16);                              //MROM Protect

    SPI0BAUD = 1;                                   //RC CLK / (1 + 1) = 1M
    SPI0CON = BIT(0);                               //SPI EN

    FUNCOUTMCON = sp_sfr.funcoutmcon;
    FUNCOUTCON  = sp_sfr.funcoutcon;
    FUNCINCON   = sp_sfr.funcincon;
    FUNCMCON0   = sp_sfr.funcomcon[0];
    FUNCMCON1   = sp_sfr.funcomcon[1];

    UART0BAUD    = sp_sfr.udetbaud;
    UART0CON     = sp_sfr.udetcon;
    UART0CPND = BIT(10) | BIT(15) | BIT(18);          //CLR Pending
    UART0CON = (UART0CON & ~(0xfff << 16)) | (0x5aa << 16);  //KEY MODE RESET

    PICCON      = sp_sfr.piccon;
    PICPR       = sp_sfr.picpr;
    PICEN       = sp_sfr.picen;
    NMICON      = sp_sfr.nmicon;

    PORTINTEDG  = 0;
    PORTINTEN   = 0;
    WKUPCON &= ~(BIT(6) | BIT(7));
    WKUPCPND = BIT(22);                             //clear 6 pending
    WKUPCPND =  BIT(23);			                //clear 7 pending
    WKUPCON &= ~BIT(17);                            //wakup sniff enable

    spiflash_init(0x0b, 1, false);

    timer0_init();
    PICCON |= BIT(0);                               //global interrupt enable
    WDT_EN();

    saradc_set_channel(sp_sfr.adc_ch);
    bsp_saradc_init();
    sys_set_tmr_enable(1, 1);
}
#endif

#if MIX_PWR_DOWN_EN
AT(.text.lowpwr.mixpwr)
void mix_pwr_down(u8 pwr_mode)
{
    printf("%s\n", __func__);
    u32 io_sta;
    io_sta = GPIOA;
    if(pwr_mode){
        while ((io_sta & BIT(MIX_PWR_SLEEP_IO-1)) == 0){     //等待按键松开
            io_sta = GPIOA;
            delay_5ms(1);
            WDT_CLR();
        }
        sfunc_lowbat();
    }else{
#if MIX_PWR_DOWN_EN
        while ((io_sta & BIT(MIX_PWR_DOWN_IO-1)) == 0){     //等待按键松开
            io_sta = GPIOA;
            delay_5ms(1);
            WDT_CLR();
        }
#endif // MIX_PWR_DOWN_EN
        memcpy(&__pdn_comm_vma, &__pdn_comm_lma, (u32)&__pdn_comm_size);//复用aram
        sfunc_pwroff();
    }
    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}
#endif

AT(.text.lowpwr.dac_off)
void sfunc_dac_off(void)
{
    music_control(MUSIC_MSG_STOP);
    dac_fade_out();
    dac_dc_exp_off();
    dac_classd_analog_off();
}

//硬开关方案，低电时，进入省电状态
AT(.text.lowpwr.lowbat)
void sfunc_lowbat(void)
{
    sfunc_dac_off();
    memcpy(&__pdn_comm_vma, &__pdn_comm_lma, (u32)&__pdn_comm_size);//复用aram
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
#if (POWER_DOWN_MODE == PWR_L4M) || (POWER_DOWN_MODE == PWR_L4M_ALM) || (!SOFT_POWER_ON_OFF) || (POWER_DOWN_MODE == PWR_RTC1S) || MIX_PWR_DOWN_EN
    memset(&sp_sfr, 0, sizeof(sleep_sfr_t));
#endif
#if (!SOFT_POWER_ON_OFF) || MIX_PWR_DOWN_EN
    printf("%s\n", __func__);
    sys_clk_free_all();
    sys_clk_set(SYS_24M);
    PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x10<<0);       //DI_VCORES 0x10 = 1.2V
    sfunc_lowbat_do();
#endif
}

AT(.text.lowpwr.pwroff)
void sfunc_pwroff(void)
{
    sfunc_dac_off();
    memcpy(&__pdn_comm_vma, &__pdn_comm_lma, (u32)&__pdn_comm_size);//复用aram
    memset(&wk_cfg, 0, sizeof(pwroff_wk_t));
#if (POWER_DOWN_MODE == PWR_L4M) || (POWER_DOWN_MODE == PWR_L4M_ALM) || (!SOFT_POWER_ON_OFF) || (POWER_DOWN_MODE == PWR_RTC1S) || MIX_PWR_DOWN_EN
    memset(&sp_sfr, 0, sizeof(sleep_sfr_t));
#endif
#if EX_SPIFLASH_PWROFF
    // spiflash1_power_down();//进入flash power_down模式, 有些flash不支持可以注释掉
#endif
    sys_clk_free_all();
    sys_clk_set(SYS_24M);
    PWRCON0 = (PWRCON0 & ~(0x1f<<0)) | (0x10<<0);       //DI_VCORES 0x10 = 1.2V
    pwroff_do();
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s\n", __func__);
#if (!MUL_PWRON_IO_EN) && (POWER_OFF_IO != 0)
    u32 io_sta;
    u8 wait = 1;
    /*-------------------------------------------------------------*/
   //此段判断,仅为下降沿关机IO使用,作用是避免按键未松开就进行关机流程
   //如要使用上升沿唤醒IO进行关机,要修改此部分判断,避免上升沿未松开就进入关机
#if (POWER_OFF_IO != 0)
    u16 gpio_fen, gpio_de, gpio_dir, gpio_pu;
    u8  pwr_off_io = POWER_OFF_IO;
    if(POWER_OFF_IO > IO_PA15){
        gpio_fen = GPIOBFEN;
        gpio_de  = GPIOBDE;
        gpio_dir = GPIOBDIR;
        gpio_pu  = GPIOBPU;
        GPIOBFEN &= ~BIT(pwr_off_io-IO_PA15-1);       //设置IO状态
        GPIOBDE  |=  BIT(pwr_off_io-IO_PA15-1);
        GPIOBDIR |=  BIT(pwr_off_io-IO_PA15-1);
        GPIOBPU  |=  BIT(pwr_off_io-IO_PA15-1);
//        if (PB7_SHORT_PB8_EN && (POWER_OFF_IO == IO_PB7 || POWER_OFF_IO == IO_PB8)) {
//            wait = 0;
//        }
        io_sta = (GPIOB << 16);
    }else{
        gpio_fen = GPIOAFEN;
        gpio_de  = GPIOADE;
        gpio_dir = GPIOADIR;
        gpio_pu  = GPIOAPU;
        GPIOAFEN &= ~BIT(pwr_off_io-1);               //设置IO状态
        GPIOADE  |=  BIT(pwr_off_io-1);
        GPIOADIR |=  BIT(pwr_off_io-1);
        GPIOAPU  |=  BIT(pwr_off_io-1);
        io_sta = GPIOA;
    }
    if (wait) {
        while ((io_sta & BIT(pwr_off_io-1)) == 0){    //等待按键松开
            if(pwr_off_io > IO_PA15){
                io_sta = (GPIOB << 16);
            }else{
                io_sta = GPIOA;
            }
            delay_5ms(1);
            WDT_CLR();
        }
    }
    if(pwr_off_io > IO_PA15){
        GPIOBFEN = gpio_fen;                            //还原IO状态
        GPIOBDE  = gpio_de;
        GPIOBDIR = gpio_dir;
        GPIOBPU  = gpio_pu;
    }else{
        GPIOAFEN = gpio_fen;                            //还原IO状态
        GPIOADE  = gpio_de;
        GPIOADIR = gpio_dir;
        GPIOAPU  = gpio_pu;
    }
#endif
    /*-------------------------------------------------------------*/
#endif
    if (SOFT_POWER_ON_OFF) {
        sfunc_pwroff();
    } else {
        sfunc_lowbat();                                 //低电关机进入Sleep Mode
    }
    //sleep->wakeup
    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}


