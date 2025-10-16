#include "include.h"

void bsp_sdadc_dc_test(void)
{
    //开启全部通路测试，实际根据需要开启对应通路即可。每条通路5us转换时间。
    sdadc_dc_set_channel(BIT(SDADCCH_PA1));
    sdadc_dc_set_channel(BIT(SDADCCH_PA3));
    sdadc_dc_set_channel(BIT(SDADCCH_PA4));
    sdadc_dc_set_channel(BIT(SDADCCH_PA10));
    sdadc_dc_set_channel(BIT(SDADCCH_VBG));
    sdadc_dc_set_channel(BIT(SDADCCH_VBAT));

    //HOT_DET控制, 硬件外部上拉, 正常使用时输出低电平。sdadc dc compare满足条件后变成输入状态硬件拉高。
    GPIOADE |= BIT(7);
    GPIOADIR &= ~BIT(7);
    GPIOAFEN |= BIT(7);
    GPIOACLR = BIT(7);
    GPIOADRV |= BIT(19);                //PA7 sdadc dc compare自动关闭output

    sdadc_dc_init();

    sdadc_dc_set_compare(35600, BIT(SDADCCH_PA10), 1);       //SDADCCH_PA10通道ADC值大于35600产生中断

    while (1) {
        for (u32 i = 0; i < 5; i++) {
            sdadc_dc_kick_start();

            printf("ch0: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_PA1), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_PA1)));
            printf("ch1: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_PA3), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_PA3)));
            printf("ch2: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_PA4), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_PA4)));
            printf("ch3: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_PA10), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_PA10)));
            printf("ch4: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_VBG), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_VBG)));
            printf("ch5: %d, %dmv\n", sdadc_dc_get_value(SDADCCH_VBAT), sdadc_dc_get_voltage(sdadc_dc_get_value(SDADCCH_VBAT)));
        }
        delay_5ms(200);
        WDT_CLR();
    }
}
