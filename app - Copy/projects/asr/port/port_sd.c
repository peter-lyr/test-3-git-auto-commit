#include "include.h"

#if MUSIC_SDCARD_EN
static gpio_t sddet_gpio;

void sdcard_detect_init(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = SD0_DET_GPIO_SEL;
    bsp_gpio_cfg_init(g, io_num);

    if (!io_num) {
        return;
    } else if (io_num == IO_MUX_SDCLK) {
        SD_MUX_DETECT_INIT();
    } else if (io_num <= IO_MAX_NUM) {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.sdio)
bool sdcard_is_online(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = SD0_DET_GPIO_SEL;

    if (!io_num) {
        return false;
    } else if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_ONLINE();
    }
//    else if (io_num == IO_MUX_SDCMD) {
//        return SD_CMD_MUX_IS_ONLINE();
//    }
    else {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
    return true;
}

AT(.com_text.sdio)
bool is_det_sdcard_busy(void)
{
#if SDCLK_MUX_DETECT_SD
    u8 io_num = SD0_DET_GPIO_SEL;

    //无SD检测
    if (!io_num) {
        return true;
    }

    //复用SDCMD或SDCLK检测
    if (io_num == IO_MUX_SDCLK) {
        return SD_MUX_IS_BUSY();
    }
//    else if(io_num == IO_MUX_SDCMD){
//        return SD_MUX_CMD_IS_BUSY();
//    }
#endif
    return false;
}

//只支持复用SDCLK检测, SDCMD没有300R上拉不能复用
AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
#if SDCMD_MUX_DETECT_AUX || SDCLK_MUX_DETECT_SD
    if (type == 0) {
        SD_MUX_IO_INIT();
        //SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    } else if (type == 1) {
        SD_CLK_DIR_OUT();
        //SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
        FUNCMCON0 = SD0_MAPPING;
    } else{
        SD_CLK_DIR_IN();
        //SD_CMD_MUX_PU10K();                     //IDLE状态，SDCMD上拉10K检测外设
    }
#else
    if (type != 0) {
        return;
    }
    SD_IO_INIT();
#endif
}

#else
AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
}
bool is_det_sdcard_busy(void){ return false;};
#endif  // MUSIC_SDCARD_EN
