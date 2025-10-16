#include "include.h"
#include "bsp_pwm.h"

#if (PWM_HW_EN || PWM_TMR2_EN)
AT(.text.bsp.pwm)
void pwm_gpio_config(u8 io_num)
{
    if(io_num == IO_NONE) {
        return;
    }

    gpio_t gpio;
    gpio_cfg_init(&gpio, io_num);
    gpio.sfr[GPIOxDE] |= BIT(gpio.num);
    gpio.sfr[GPIOxFEN] |= BIT(gpio.num);
    gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
}
#endif

/*****************************************************************************
 * Module    : HW_PWM
 *****************************************************************************/
#if PWM_HW_EN
const u8 pwm_hw_map_sel[] = PWM_HW_MAP_SEL;


//寄存器配置
void bsp_pwm_io_init(u8 io_num_0, u8 io_num_1, u8 io_num_2, u8 io_num_3)
{
    FUNCMCON1 = (FUNCMCON1 & ~(0xf << 8)) | (0x1 << 8);    //PWM map to G1
    if(io_num_0) {
        pwm_gpio_config(IO_PA4);
        PWMCON = (PWMCON & ~(0x3 << 16)) | (0x0 << 16);
    }

    if(io_num_1) {
        pwm_gpio_config(IO_PA5);
        PWMCON = (PWMCON & ~(0x3 << 18)) | (0x1 << 18);
    }

    if(io_num_2) {
        pwm_gpio_config(IO_PA8);
        PWMCON = (PWMCON & ~(0x3 << 20)) | (0x2 << 20);
    }

    if(io_num_3) {
        pwm_gpio_config(IO_PA9);
        PWMCON = (PWMCON & ~(0x3 << 22)) | (0x3 << 22);
    }
}

void bsp_pwm_clk_source(void)
{
    PTMR0CON = (PTMR0CON & ~(0x7 << 1)) | (0 << 1);
    PTMR1CON = (PTMR0CON & ~(0x7 << 1)) | (0 << 1);
    PTMR2CON = (PTMR0CON & ~(0x7 << 1)) | (0 << 1);
    PTMR2CON = (PTMR0CON & ~(0x7 << 1)) | (0 << 1);
}

void pwm_map_all_io_select(u8 pwm0_io, u8 pwm1_io, u8 pwm2_io, u8 pwm3_io)
{
    if(pwm0_io) {
        pwm_gpio_config(pwm0_io);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 0)) | (0 << 0);                                        //PWM0 map
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 0)) | (pwm0_io << 0);
        PWMCON = (PWMCON & ~(0x3 << 16)) | (0x0 << 16);                                           //PWM0 select PWM_TMR0
    }

    if(pwm1_io) {
        pwm_gpio_config(pwm1_io);                                                                 //PWM1 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 8)) | (1 << 8);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 8)) | (pwm1_io << 8);
        PWMCON = (PWMCON & ~(0x3 << 18)) | (0x1 << 18);                                           //PWM1 select PWM_TMR1
    }

    if(pwm2_io) {
        pwm_gpio_config(pwm2_io);                                                                 //PWM2 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 16)) | (2 << 16);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 16)) | (pwm2_io << 16);
        PWMCON = (PWMCON & ~(0x3 << 20)) | (0x2 << 20);                                           //PWM2 select PWM_TMR2
    }

    if(pwm3_io) {
        pwm_gpio_config(pwm3_io);                                                                 //PWM3 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 24)) | (3 << 24);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 24)) | (pwm3_io << 24);
        PWMCON = (PWMCON & ~(0x3 << 22)) | (0x3 << 22);                                           //PWM3 select PWM_TMR3
    }

}

void bsp_pwm_clk_init(void)
{
    CLKGAT1 |= BIT(8);     //PWM clken

    //CLKGAT0 |= BIT(25);    //TMR2 clken
}

void bsp_pwm_inv_cfg(u8 en0,u8 en1,u8 en2,u8 en3)
{
    ///PWM0~3 invert
    if(en0) {
        PWMCON |= BIT(8);
    } else {
        PWMCON &= ~BIT(8);
    }

    if(en1) {
        PWMCON |= BIT(9);
    } else {
        PWMCON &= ~BIT(9);
    }

    if(en2) {
        PWMCON |= BIT(10);
    } else {
        PWMCON &= ~BIT(10);
    }

    if(en3) {
        PWMCON |= BIT(11);
    } else {
        PWMCON &= ~BIT(11);
    }
}

void bsp_pwm_dump_sfr(void)
{
        my_printf("PWMCON 0x%x\n",PWMCON);
        my_printf("PTMRCON 0x%x, 0x%x, 0x%x, 0x%x\n",PTMR0CON,PTMR1CON,PTMR2CON,PTMR3CON);
        my_printf("PTMRPR 0x%x, 0x%x, 0x%x, 0x%x\n",PTMR0PR,PTMR1PR,PTMR2PR,PTMR3PR);
        my_printf("PTMRDUTY 0x%x, 0x%x, 0x%x, 0x%x\n",PTMR0DUTY,PTMR1DUTY,PTMR2DUTY,PTMR3DUTY);
        my_printf("FUNCMCON1 0x%x\n",FUNCMCON1);
}

void bsp_pwm_pre_en(u8 pre0,u8 pre1,u8 pre2,u8 pre3)
{
    ///PWM0~3 Pre_sel selection  000:clock divide 1  001:clock divide 2  010:clock divide 4  011:clock divide 8  100:clock divide 16  101:clock divide 32  110:clock divide 48  111:clock divide 64
    PTMR0CON = (PTMR0CON & ~(0x7 << 4)) | (pre0 << 4);
    PTMR1CON = (PTMR1CON & ~(0x7 << 4)) | (pre1 << 4);
    PTMR2CON = (PTMR2CON & ~(0x7 << 4)) | (pre2 << 4);
    PTMR3CON = (PTMR3CON & ~(0x7 << 4)) | (pre3 << 4);
}

void bsp_pwm_tmr_mode(u8 mode0,u8 mode1,u8 mode2,u8 mode3)
{
    ///PWM0~3 TMR_MODE
    PTMR0CON = (PTMR0CON & ~(0x7 << 14)) | (mode0 << 14);
    PTMR1CON = (PTMR1CON & ~(0x7 << 14)) | (mode1 << 14);
    PTMR2CON = (PTMR2CON & ~(0x7 << 14)) | (mode2 << 14);
    PTMR3CON = (PTMR3CON & ~(0x7 << 14)) | (mode3 << 14);
}

void bsp_pwm_duty_cfg(u16 duty0, u16 duty1, u16 duty2, u16 duty3)
{
    ///PWM0~3 PTMRDUTY
    PTMR0DUTY = BIT(0) * duty0;
    PTMR1DUTY = BIT(0) * duty1;
    PTMR2DUTY = BIT(0) * duty2;
    PTMR3DUTY = BIT(0) * duty3;
}

void bsp_pwm_pr_cfg(u16 pr0, u16 pr1, u16 pr2, u16 pr3)
{
    ///PWM0~3 PTMRDUTY
    PTMR0PR = BIT(0) * pr0;
    PTMR1PR = BIT(0) * pr1;
    PTMR2PR = BIT(0) * pr2;
    PTMR3PR = BIT(0) * pr3;
}

void bsp_pwm_en(u8 pwm0_en, u8 pwm1_en, u8 pwm2_en, u8 pwm3_en)
{
    if(pwm0_en) {
        PWMCON |= BIT(0);                                           //TMR0 en
        PTMR0CON |= BIT(0);
        PWMCON |= BIT(4);

    } else {
        PWMCON &= ~BIT(0);                                          //TMR0 dis
        PTMR0CON &= ~BIT(0);
        PWMCON &= ~BIT(4);
    }

    if(pwm1_en) {
        PWMCON |= BIT(1);                                           //TMR1 en
        PTMR1CON |= BIT(0);
        PWMCON |= BIT(5);
    } else {
        PWMCON &= ~BIT(1);                                          //TMR1 dis
        PTMR1CON &= ~BIT(0);
        PWMCON &= ~BIT(5);
    }

    if(pwm2_en) {
        PWMCON |= BIT(2);                                           //TMR2 en
        PTMR2CON |= BIT(0);
        PWMCON |= BIT(6);
    } else {
        PWMCON &= ~BIT(2);                                          //TMR2 dis
        PTMR2CON &= ~BIT(0);
        PWMCON &= ~BIT(6);
    }

    if(pwm3_en) {
        PWMCON |= BIT(3);                                           //TMR3 en
        PTMR3CON |= BIT(0);
        PWMCON |= BIT(7);
    } else {
        PWMCON &= ~BIT(3);                                          //TMR3 dis
        PTMR3CON &= ~BIT(0);
        PWMCON &= ~BIT(7);
    }
}

void bsp_pwm_ie(u8 pwm0_ie, u8 pwm1_ie, u8 pwm2_ie, u8 pwm3_ie)
{
    if(pwm0_ie) {
        PTMR0CON |= BIT(7);
    } else {
        PTMR0CON &= ~BIT(7);
    }

    if(pwm1_ie) {
        PTMR1CON |= BIT(7);
    } else {
        PTMR1CON &= ~BIT(7);
    }

    if(pwm2_ie) {
        PTMR2CON |= BIT(7);
    } else {
        PTMR2CON &= ~BIT(7);
    }

    if(pwm3_ie) {
        PTMR3CON |= BIT(7);
    } else {
        PTMR2CON &= ~BIT(7);
    }
}

void bsp_pwm_hw_init(pwm_init_t *pwm_cfg)
{
    pwm_init_t *ptr = pwm_cfg;

    if(ptr) {
        bsp_pwm_clk_init();

#if (!PWM_HW_MAP_IO_EN)  //不映射，则使用默认IO
        bsp_pwm_io_init(ptr->io_state.pwm0_io_state, ptr->io_state.pwm1_io_state, ptr->io_state.pwm2_io_state, ptr->io_state.pwm3_io_state);
#endif
        //只能选择system clock时钟源
        bsp_pwm_clk_source();
        //分频
        bsp_pwm_pre_en(ptr->clock_pre_sel.pwm0pre_sel, ptr->clock_pre_sel.pwm1pre_sel, ptr->clock_pre_sel.pwm2pre_sel, ptr->clock_pre_sel.pwm3pre_sel);
        //mode
        bsp_pwm_tmr_mode(ptr->tmr_mode.pwm0_tmr_mode_state, ptr->tmr_mode.pwm1_tmr_mode_state, ptr->tmr_mode.pwm2_tmr_mode_state, ptr->tmr_mode.pwm3_tmr_mode_state);
        //duty
        bsp_pwm_duty_cfg(ptr->tmrduty.tmr0duty, ptr->tmrduty.tmr1duty, ptr->tmrduty.tmr2duty, ptr->tmrduty.tmr3duty);
        //周期
        bsp_pwm_pr_cfg(ptr->tmrpr.tmr0pr, ptr->tmrpr.tmr1pr, ptr->tmrpr.tmr2pr, ptr->tmrpr.tmr3pr);
        //是否反相
        bsp_pwm_inv_cfg(ptr->inv_state.pwm0_inv, ptr->inv_state.pwm1_inv, ptr->inv_state.pwm2_inv, ptr->inv_state.pwm3_inv);
        //是否开中断
        bsp_pwm_ie(ptr->ie_state.pwm0_ie, ptr->ie_state.pwm1_ie, ptr->ie_state.pwm2_ie, ptr->ie_state.pwm3_ie);
        //是否使能
        bsp_pwm_en(ptr->en_state.pwm0_en, ptr->en_state.pwm1_en, ptr->en_state.pwm2_en, ptr->en_state.pwm3_en);



        delay_us(100);
    }
    pwm_irq_init();
}

//初始化和中断
AT(.com_text.isr.pwm)
void pwm_isr(void)
{
    if(PTMR0CON & BIT(9)) {
        PTMR0CPND |= BIT(9);
    }
    if(PTMR1CON & BIT(9)) {
        PTMR1CPND |= BIT(9);
    }
    if(PTMR2CON & BIT(9)) {
        PTMR2CPND |= BIT(9);
    }
    if(PTMR3CON & BIT(9)) {
        PTMR3CPND |= BIT(9);
    }
}
///硬件PWM初始化函数
void pwm_hw_cfg_init(void)
{
    pwm_init_t  pwm_cfg_init;
    memset(&pwm_cfg_init, 0, sizeof(pwm_cfg_init));
#if PWM_HW_MAP_IO_EN
    pwm_map_all_io_select(pwm_hw_map_sel[0],pwm_hw_map_sel[1],pwm_hw_map_sel[2],pwm_hw_map_sel[3]);        //PWM IO映射
#else
    pwm_cfg_init.io_state.pwm0_io_state    =    PWM_IO_EN;       //只有PWM OUTPUT 对应的IO使能，才会操操作相关的寄存器，才能输出PWM波
    pwm_cfg_init.io_state.pwm1_io_state    =    PWM_IO_EN;
    pwm_cfg_init.io_state.pwm2_io_state    =    PWM_IO_EN;
    pwm_cfg_init.io_state.pwm3_io_state    =    PWM_IO_EN;
#endif // PWM_HW_MAP_IO_EN

	//参数配置
    //占空比duty (范围是 0~0xffff, duty)
    pwm_cfg_init.tmrduty.tmr0duty              =    0x7fff;
    pwm_cfg_init.tmrduty.tmr1duty              =    0x7000;
    pwm_cfg_init.tmrduty.tmr2duty              =    0x7fff;
    pwm_cfg_init.tmrduty.tmr3duty              =    0x7000;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    pwm_cfg_init.tmrpr.tmr0pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr1pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr2pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr3pr                  =    0xffff;
    //是否反相invert
    pwm_cfg_init.inv_state.pwm0_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm1_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm2_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm3_inv            =    PWM_INV_EN;
    //分频选择pre_sel
    pwm_cfg_init.clock_pre_sel.pwm0pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm1pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm2pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm3pre_sel     =    CLOCK_DEVIDE_1;
    //timer or PWM mode 默认PWM模式不需要修改
    pwm_cfg_init.tmr_mode.pwm0_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm1_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm2_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm3_tmr_mode_state  =    TIMER_PWM_MODE;
    //是否开启中断
    pwm_cfg_init.ie_state.pwm0_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm1_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm2_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm3_ie              =    PWM_IE_EN;

#if PWM_HW_1KHZ_TEST_EN
    //系统时钟48M，PWM频率 = 系统时钟(48M) / （PWM周期(tmrpr)+1） / PWM分频(clock_pre_sel)
    // 48000000 / (47999 + 1) = 1000HZ
    pwm_cfg_init.tmrduty.tmr0duty              =    0x5dc0;
    pwm_cfg_init.tmrduty.tmr1duty              =    0x5dc0;
    pwm_cfg_init.tmrduty.tmr2duty              =    0x5dc0;
    pwm_cfg_init.tmrduty.tmr3duty              =    0x5dc0;

    pwm_cfg_init.tmrpr.tmr0pr                  =    0xbb7f;
    pwm_cfg_init.tmrpr.tmr1pr                  =    0xbb7f;
    pwm_cfg_init.tmrpr.tmr2pr                  =    0xbb7f;
    pwm_cfg_init.tmrpr.tmr3pr                  =    0xbb7f;

    pwm_cfg_init.clock_pre_sel.pwm0pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm1pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm2pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm3pre_sel     =    CLOCK_DEVIDE_1;
#endif // PWM_HW_1KHZ_TEST_EN
    //是否开启PWM
    pwm_cfg_init.en_state.pwm0_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm1_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm2_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm3_en              =    PWM_EN;

    bsp_pwm_hw_init(&pwm_cfg_init);
}
#endif //PWM_HW_EN



/*****************************************************************************
 * Module    : TMR2PWM
 *****************************************************************************/
#if PWM_TMR2_EN
#define PWM_TMR2_CLK_SEL            PWM_XOSC24M_CLK     //选择TMR2PWM时钟选择晶振

const u8 pwm_tmr2_map_sel[] = PWM_TMR2_MAP_SEL;

//寄存器配置
void bsp_tmr2pwm_clk_init(void)
{
    CLKGAT0 |= BIT(25);
}

void bsp_tmr2pwm_en(u8 pwm0_sta, u8 pwm1_sta)
{
    if(pwm0_sta) {
        TMR2CON |= BIT(16);    //pwm en
    } else {
        TMR2CON &= ~BIT(16);
    }

    if(pwm1_sta) {
        TMR2CON |= BIT(18);    //pwm en
    } else {
        TMR2CON &= ~BIT(18);
    }
}

void bsp_tmr2pwm_mode(u8 mode)
{
    TMR2CON = (TMR2CON & ~(0x3 << 14)) | (mode << 14);
}

void bsp_tmr2pwm_inv_en(u8 pwm0_sta, u8 pwm1_sta)
{
    if(pwm0_sta) {
        TMR2CON |= BIT(17);
    } else {
        TMR2CON &= ~BIT(17);
    }

    if(pwm1_sta) {
        TMR2CON |= BIT(19);
    } else {
        TMR2CON &= ~BIT(19);
    }
}

void bsp_tmr2pwm_pr_config(u32 pwmpr)
{
    TMR2PR = pwmpr;
}

void bsp_tmr2pwm_duty_config(u16 pwm0duty, u16 pwm1duty)
{
    TMR2DUTY0 = pwm0duty;
    TMR2DUTY1 = pwm1duty;
}

void tmr2pwm_map_io_select(u8 pwm0_io, u8 pwm1_io)
{
    if(pwm0_io) {
        pwm_gpio_config(pwm0_io);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 0)) | (pwm0_io << 0);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 0)) | (4 << 0);
    }

    if(pwm1_io) {
        pwm_gpio_config(pwm1_io);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 8)) | (pwm1_io << 8);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 8)) | (5 << 8);
    }
}

//初始化
void bsp_tmr2pwm_init(tmr2pwm_t *pwm_cfg)
{
    tmr2pwm_t *ptr = pwm_cfg;

    if(ptr) {
        my_printf("%s\n",__func__);
        TMR2CON = (TMR2CON & ~(0x7<<1)) | (PWM_TMR2_CLK_SEL << 1);
        //时钟初始化
        bsp_tmr2pwm_clk_init();
        //IO使能
        bsp_tmr2pwm_en(ptr->io_state.pwm0_io_state, ptr->io_state.pwm1_io_state);
        //mode
        bsp_tmr2pwm_mode(ptr->tmr_mode.pwm_tmr_mode_state);
        //分频
        TMR2CON = (TMR2CON & ~(0x7 << 4)) | (ptr->clock_pre_sel.pwmpre_sel << 4);
        //周期
        bsp_tmr2pwm_pr_config(ptr->tmrpr.tmr2pr);
        //duty
        bsp_tmr2pwm_duty_config(ptr->tmrduty.tmr0duty, ptr->tmrduty.tmr1duty);
        //是否反相
        bsp_tmr2pwm_inv_en(ptr->inv_state.pwm0_inv,ptr->inv_state.pwm1_inv);

        TMR2CNT = -1;
        TMR2CON |= BIT(7);
        TMR2CON |= BIT(0);
    }
    //my_printf("TMR2CON 0x%x\n",TMR2CON);
}

///TMR2PWM初始化函数
void tmr2pwm_cfg_init(void)
{
    tmr2pwm_t tmr2_pwm_init;
    memset(&tmr2_pwm_init, 0, sizeof(tmr2_pwm_init));

#if PWM_TMR2_MAP_IO_EN
    tmr2pwm_map_io_select(pwm_tmr2_map_sel[0],pwm_tmr2_map_sel[1]);
#else
    GPIOADE |= BIT(2)| BIT(3);
    GPIOAFEN |= BIT(2)|BIT(3);
    GPIOADIR &= ~(BIT(2)|BIT(3));
    FUNCMCON1 = (FUNCMCON1 & ~(0xf << 4)) | (1 << 4);
#endif // PWM_TMR2_MAP_IO_EN

    //参数配置
    //是否开启PWM
    tmr2_pwm_init.io_state.pwm0_io_state      = TMR2PWM_IO_EN;
    tmr2_pwm_init.io_state.pwm1_io_state      = TMR2PWM_IO_EN;
    //是否反相invert
    tmr2_pwm_init.inv_state.pwm0_inv          = TMR2PWM_INV_EN;
    tmr2_pwm_init.inv_state.pwm1_inv          = TMR2PWM_INV_EN;
    //占空比duty (范围是 0~0xffff, duty)
    tmr2_pwm_init.tmrduty.tmr0duty            = 0x7fff;
    tmr2_pwm_init.tmrduty.tmr1duty            = 0x7fff;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    tmr2_pwm_init.tmrpr.tmr2pr                = 0xffff;
    //分频选择pre_sel
    tmr2_pwm_init.clock_pre_sel.pwmpre_sel    = TMR2CLOCK_DEVIDE_1;
    //timer or PWM mode 默认PWM模式不需要修改
    tmr2_pwm_init.tmr_mode.pwm_tmr_mode_state = TMR2_TIMER_PWM_MODE;
#if PWM_TMR2_1KHZ_TEST_EN
    //PWM时钟，PWM频率 = PWM时钟 / （PWM周期(tmrpr)+1） / PWM分频(clock_pre_sel)
    //此处选择XOSC24M_CLK，即24M时钟
    tmr2_pwm_init.tmrduty.tmr0duty            = 0x2ee0;
    tmr2_pwm_init.tmrduty.tmr1duty            = 0x2ee0;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    tmr2_pwm_init.tmrpr.tmr2pr                = 0x5dbf;
    //分频选择pre_sel
    tmr2_pwm_init.clock_pre_sel.pwmpre_sel    = TMR2CLOCK_DEVIDE_1;
#endif // PWM_TMR2_1KHZ_TEST_EN

    bsp_tmr2pwm_init(&tmr2_pwm_init);
}
#endif // PWM_TMR2_EN
