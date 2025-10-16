#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if EX_SPIFLASH_SUPPORT && !(EX_SPIFLASH_SUPPORT == 0x1)  //进外置录音播放会复位
    FUNC_EXSPIFLASH_MUSIC,
#endif

#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN

#if FUNC_SPEAKER_EN
    FUNC_SPEAKER,
#endif // FUNC_SPEAKER_EN

#if FUNC_AUX_EN
    FUNC_AUX,
#endif // FUNC_AUX_EN

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif // FUNC_IDLE_EN
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

u32 getcfg_mic_bias_method(void)
{
    return xcfg_cb.mic_bias_method;
}

u32 getcfg_mic_bias_en(void)
{
    return xcfg_cb.mic_bias_en;
}

u32 getcfg_mic_bias_rias(void)
{
    return xcfg_cb.mic_rias_sel;
}

AT(.com_text.vddio)
u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

///获取当前vddio电压，单位mV，与实际输出vddio偏差0.01v左右
u32 get_vddio_voltage(void)
{
    return (1500 + xcfg_cb.vddio_sel * 75);
}

AT(.com_text.vddio)
u32 getcfg_reset_sel(void)
{
    return xcfg_cb.reset_sel;
}

u16 get_wav_fix_spr(void)
{
#if WAV_FIX_SPR_EN
    return WAV_FIX_SPR_VAL;
#endif
    return 0;
}
