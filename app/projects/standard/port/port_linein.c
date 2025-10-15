#include "include.h"

#if LINEIN_DETECT_EN
static gpio_t linein_det_gpio;

void linein_detect_init(void)
{
    gpio_t *g = &linein_det_gpio;
    u8 io_num = AUX_DET_GPIO_SEL;
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

AT(.com_text.linein.detect)
bool linein_is_online(void)
{
    gpio_t *g = &linein_det_gpio;
    u8 io_num = AUX_DET_GPIO_SEL;

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

AT(.com_text.linein.detect)
bool is_detect_linein_busy(void)
{
#if SDCMD_MUX_DETECT_AUX
    u8 io_num = AUX_DET_GPIO_SEL;

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

#endif // LINEIN_DETECT_EN
