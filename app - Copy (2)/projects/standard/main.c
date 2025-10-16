#include "include.h"

void mclr_l_init(void)
{
    CLKGAT0 |=  BIT(15);                        //lp clken
    RTCCON0 |=  BIT(0);                         //RC enable
    // RTCCON3 |=  BIT(3);                      //切换到vlcore供电
    // RTCCON3 &= ~BIT(5);
    RTCCON12 &= ~(3 << 4);                      //GPIO 10s reset enable
}

void mclr_10s_rst_io_init(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBDE  |= BIT(io_num-1-IO_PA15);
        GPIOBPU  |= BIT(io_num-1-IO_PA15);
        GPIOBDIR |= BIT(io_num-1-IO_PA15);
    } else {
        GPIOADE  |= BIT(io_num-1);
        GPIOAPU  |= BIT(io_num-1);
        GPIOADIR |= BIT(io_num-1);
    }
}

//当efuse MCLR标志位为1时,MCLR复位引脚内部会有上拉,不用外部上拉
void mclr_reset_init(void)
{
    RTCCON12 |= ((3 << 2) | (3 << 4) | (3 << 6));
    if(MCLR_RESRT_MDOE == 1){               //长按复位(任意IO)
        mclr_10s_rst_io_init(IO_PA0);
        if(RTCCON10 & BIT(13)) {
            RTCCON12 |= (3 << 4);           //GPIO 10s reset disable
            RTCCON10 |= BIT(13);            //clear pnd
            CRSTPND  |= BIT(19);
            mclr_l_init();                  //10s reset init
        } else {
            mclr_l_init();                  //10s reset init
        }
        RTCCON12 |=  ((3 << 2) | (3 << 6));
    }else if(MCLR_RESRT_MDOE == 2){         //短按复位(PA7)
        GPIOADE  |= BIT(7);
        GPIOAPU  |= BIT(7);
        GPIOADIR |= BIT(7);
        if(RTCCON10 & BIT(9)) {
            RTCCON10 |= BIT(9);             //clear pnd
        } else {
            // RTCCON3  |=  BIT(3);         //切换到vlcore供电
            // RTCCON3  &= ~BIT(5);
            RTCCON12 &= ~(3 << 2);          //PA7 reset enable
        }
        RTCCON12 |=  ((3 << 4) | (3 << 6));
    }else{
        GPIOAPU &= ~BIT(0);                 //取消硬件默认上拉
        GPIOAPU &= ~BIT(7);
    }
}

//系统启动最初的位置
void platform_main_insert(void)
{
    mclr_reset_init();
}

//正常启动Main函数
int main(void)
{
    u32 rst_reason;
    rst_reason = sys_rst_init();
    printf("Hello AB18x standard: %08x\n", rst_reason);
    sys_rst_dump(rst_reason);
    sys_cb.wakeup_reason = lowpwr_get_wakeup_source();

    sys_ram_info_dump();
    bsp_sys_init();
    //tdnn_cal_test();
    func_run();

    return 0;
}

//升级完成
void update_complete(int mode)
{
    bsp_update_init();
    printf("update complete: %d\n", mode);
    if (mode == 0) {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}

/*
                    PA06                    PA08        KEY1-
             水流   PA05                    PA09
                    PA04                    PA10        KEY2-
                    PB04                    PA11
                    PB03                    PA12
                    PA03                    VTN
                    PA02                    VTP
                    PA01                    GND
                    PA00                    VBAT
                    PA14                    VDDIO
                    PA13                    PB0
                    PB01                    AGND

*/

