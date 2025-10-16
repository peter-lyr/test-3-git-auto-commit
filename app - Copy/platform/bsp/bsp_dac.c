#include "include.h"
#include "bsp_dac.h"

#define BSP_DAC_DEBUG_EN    0

#define DAC_OBUF_SIZE       384

const u16 *dac_dvol_table;
u16 dac_obuf[DAC_OBUF_SIZE * (1 + DAC_24BITS_EN)] AT(.dac_obuf.src0);

uint16_t cfg_pcm_out_format = DAC_24BITS_EN * PCM_OUT_24BITS;

u8 low_power_vbat_en = LPWR_VBAT_EN;

//启用dac_msc_vol_by_src0时,为SRC0VOLCON音量表格
AT(.rodata.dac)
const u16 dac_dvol_tbl_db[61] = {
    DIG_N0DB,  DIG_N1DB,  DIG_N2DB,  DIG_N3DB,  DIG_N4DB,  DIG_N5DB,  DIG_N6DB,  DIG_N7DB,
    DIG_N8DB,  DIG_N9DB,  DIG_N10DB, DIG_N11DB, DIG_N12DB, DIG_N13DB, DIG_N14DB, DIG_N15DB,
    DIG_N16DB, DIG_N17DB, DIG_N18DB, DIG_N19DB, DIG_N20DB, DIG_N21DB, DIG_N22DB, DIG_N23DB,
    DIG_N24DB, DIG_N25DB, DIG_N26DB, DIG_N27DB, DIG_N28DB, DIG_N29DB, DIG_N30DB, DIG_N31DB,
    DIG_N32DB, DIG_N33DB, DIG_N34DB, DIG_N35DB, DIG_N36DB, DIG_N37DB, DIG_N38DB, DIG_N39DB,
    DIG_N40DB, DIG_N41DB, DIG_N42DB, DIG_N43DB, DIG_N44DB, DIG_N45DB, DIG_N46DB, DIG_N47DB,
    DIG_N48DB, DIG_N49DB, DIG_N50DB, DIG_N51DB, DIG_N52DB, DIG_N53DB, DIG_N54DB, DIG_N55DB,
    DIG_N56DB, DIG_N57DB, DIG_N58DB, DIG_N59DB, DIG_N60DB,
};

//启用dac_msc_vol_by_src0时,为DACVOLCON音量表格
AT(.rodata.dac)
const u16 dac_max_gain_tbl_db[61] = {
    MAX_DIG_P_VAL, DIG_P5DB, DIG_P4DB, DIG_P3DB, DIG_P2DB, DIG_P1DB,
    DIG_N0DB,  DIG_N1DB,  DIG_N2DB,  DIG_N3DB,  DIG_N4DB,  DIG_N5DB,  DIG_N6DB,  DIG_N7DB,
    DIG_N8DB,  DIG_N9DB,  DIG_N10DB, DIG_N11DB, DIG_N12DB, DIG_N13DB, DIG_N14DB, DIG_N15DB,
    DIG_N16DB, DIG_N17DB, DIG_N18DB, DIG_N19DB, DIG_N20DB, DIG_N21DB, DIG_N22DB, DIG_N23DB,
    DIG_N24DB, DIG_N25DB, DIG_N26DB, DIG_N27DB, DIG_N28DB, DIG_N29DB, DIG_N30DB, DIG_N31DB,
    DIG_N32DB, DIG_N33DB, DIG_N34DB, DIG_N35DB, DIG_N36DB, DIG_N37DB, DIG_N38DB, DIG_N39DB,
    DIG_N40DB, DIG_N41DB, DIG_N42DB, DIG_N43DB, DIG_N44DB, DIG_N45DB, DIG_N46DB, DIG_N47DB,
    DIG_N48DB, DIG_N49DB, DIG_N50DB, DIG_N51DB, DIG_N52DB, DIG_N53DB, DIG_N54DB,
};

//每档0.5DB
AT(.rodata.sdadc)
const int dac_digital_gain_tbl[64] = {
    DIG_P0DB, DIG_P0_5DB, DIG_P1DB, DIG_P1_5DB, DIG_P2DB, DIG_P2_5DB, DIG_P3DB, DIG_P3_5DB,
    DIG_P4DB, DIG_P4_5DB, DIG_P5DB, DIG_P5_5DB, DIG_P6DB, DIG_P6_5DB, DIG_P7DB, DIG_P7_5DB,
    DIG_P8DB, DIG_P8_5DB, DIG_P9DB, DIG_P9_5DB, DIG_P10DB, DIG_P10_5DB, DIG_P11DB, DIG_P11_5DB,
    DIG_P12DB, DIG_P12_5DB, DIG_P13DB, DIG_P13_5DB, DIG_P14DB, DIG_P14_5DB, DIG_P15DB, DIG_P15_5DB,
    DIG_P16DB, DIG_P16_5DB, DIG_P17DB, DIG_P17_5DB, DIG_P18DB, DIG_P18_5DB, DIG_P19DB, DIG_P19_5DB,
    DIG_P20DB, DIG_P20_5DB, DIG_P21DB, DIG_P21_5DB, DIG_P22DB, DIG_P22_5DB, DIG_P23DB, DIG_P23_5DB,
    DIG_P24DB, DIG_P24_5DB, DIG_P25DB, DIG_P25_5DB, DIG_P26DB, DIG_P26_5DB, DIG_P27DB, DIG_P27_5DB,
    DIG_P28DB, DIG_P28_5DB, DIG_P29DB, DIG_P29_5DB, DIG_P30DB, DIG_P30_5DB, DIG_P31DB, DIG_P31_5DB,
};

AT(.rodata.dac)
const u16 dac_dvol_tbl_16[16 + 1] = {
    60,  43,  32,  26,  24,  22,  20,  18, 16,
    14,  12,  10,  8,   6,   4,   2,   0,
};

AT(.rodata.dac)
const u16 dac_dvol_tbl_32[32 + 1] = {
    60,  50,  43,  38,  35,  30,  28,  26,
    24,  23,  22,  21,  20,  19,  18,  17,
    16,  15,  14,  13,  12,  11,  10,  9,
    8,   7,   6,   5,   4,   3,   2,   1,   0,
};

AT(.rodata.dac)
const u16 dac_dvol_tbl_50[50 + 1] = {
    60,  56,  54,  52,  50,  48,  46,  44,
    42,  39,  38,  37,  36,  35,  34,  33,
    32,  31,  30,  29,  28,  27,  26,  25,
    24,  23,  22,  21,  20,  19,  18,  17,
    16,  15,  14,  13,  12,  11,  10,  9,
    8,   7,   6,   5,   4,   3,   4,   3,
    2,   1,   0,
};

AT(.text.bsp.dac)
void bsp_change_volume(u8 vol)
{
    u8 level = 0;

    if (vol <= VOL_MAX) {
        level = dac_dvol_table[vol];
        if (level > 60) {
            level = 60;
        }
        dac_set_dvol(dac_dvol_tbl_db[level]);
    }
}

AT(.text.dac)
bool bsp_set_volume(u8 vol)
{
    bsp_change_volume(vol);
    if (vol == sys_cb.vol) {
        return false;
    }

    if (vol <= VOL_MAX) {
        sys_cb.vol = vol;
        param_sys_vol_write();
        sys_cb.cm_times = 0;
        sys_cb.cm_vol_change = 1;
    }
    printf("vol: %d\n", sys_cb.vol);
    return true;
}


AT(.text.dac.vtbl)
u8 bsp_volume_inc(u8 vol, u8 step)
{
    vol += step;
    if(vol > VOL_MAX)
        vol = VOL_MAX;
    return vol;
}

AT(.text.dac.vtbl)
u8 bsp_volume_dec(u8 vol, u8 step)
{
    if(vol >= step) {
        vol -= step;
    } else {
        vol = 0;
    }
    return vol;
}

#if BSP_DAC_DEBUG_EN

//1k 0db
AT(.rodata.sina)
const u8 sina_48k_0db_mono_tbl[] = {
    0x00, 0x00, 0xB6, 0x10, 0x21, 0x21, 0xFC, 0x30, 0x00, 0x40, 0xEB, 0x4D, 0x82, 0x5A, 0x8B, 0x65,
    0xDA, 0x6E, 0x40, 0x76, 0xA3, 0x7B, 0xE7, 0x7E, 0xFE, 0x7F, 0xE7, 0x7E, 0xA2, 0x7B, 0x41, 0x76,
    0xD9, 0x6E, 0x8C, 0x65, 0x82, 0x5A, 0xEA, 0x4D, 0x00, 0x40, 0xFB, 0x30, 0x21, 0x21, 0xB5, 0x10,
    0x00, 0x00, 0x4B, 0xEF, 0xDF, 0xDE, 0x04, 0xCF, 0x02, 0xC0, 0x15, 0xB2, 0x7F, 0xA5, 0x74, 0x9A,
    0x28, 0x91, 0xBF, 0x89, 0x5D, 0x84, 0x19, 0x81, 0x01, 0x80, 0x19, 0x81, 0x5D, 0x84, 0xC0, 0x89,
    0x27, 0x91, 0x75, 0x9A, 0x7F, 0xA5, 0x15, 0xB2, 0x00, 0xC0, 0x03, 0xCF, 0xDF, 0xDE, 0x4B, 0xEF,
};

AT(.rodata.sina)
const s16 sina_16k_tbl[16] = {
    0x0000, 0x188D, 0x2D5C, 0x3B44, 0x4026, 0x3B45, 0x2D5C, 0x188D,
    0x0000, 0xE773, 0xD2A3, 0xC4BC, 0xBFDA, 0xC4BC, 0xD2A4, 0xE774,
};

static u16 aubuf_in[48];

void dac_play_sin1k_test(void)
{
    u32 samples;
    static u32 ticks = 0;
    printf("`BSP_DAC_DEBUG_EN` is true, set false to disable debuging:\n");
    printf("sin1k playing...\n");

    dac_fade_in();
    bsp_change_volume(16);
#if 1
    dac_spr_set(SPR_48000);
    memcpy(aubuf_in, sina_48k_0db_mono_tbl, sizeof(sina_48k_0db_mono_tbl));
    samples = 48;
#else
    dac_spr_set(SPR_16000);
    memcpy(aubuf_in, sina_16k_tbl, sizeof(sina_16k_tbl));
    samples = 16;
#endif
    aubuf0_dma_init(1);
    while (1) {
        aubuf0_dma_w4_done();
        aubuf0_dma_kick(aubuf_in, samples, 1, false);
        WDT_CLR();

        if (tick_check_expire(ticks, 1000)) {
            ticks = tick_get();
            printf("DACVOLCON: %x\n", DACVOLCON);
        }
    }
}
#endif // BSP_DAC_DEBUG_EN

AT(.text.bsp.dac)
void dac_set_vol_table(u8 vol_max)
{
    if (vol_max == 16) {
        dac_dvol_table = dac_dvol_tbl_16;
    } else if (vol_max <= 32) {
        dac_dvol_table = dac_dvol_tbl_32;
    } else {
        dac_dvol_table = dac_dvol_tbl_50;
    }
}

AT(.text.bsp.dac)
u8 dac_max_gain_limit(u8 dac_max_gain)
{
    u8 tmp_gain;
    if(DAC_CLASSD_EN){              //classD
        tmp_gain = dac_max_gain;
    }else{                          //DAC
        tmp_gain = dac_max_gain;
        if(dac_max_gain < 9){       //使用DAC输出时,需要限制DAC幅度在-3dB以下,避免DAC失真
            tmp_gain = 9;
        }
    }
    return tmp_gain;
}

AT(.text.bsp.dac)
void dac_init(void)
{
    u8 dac_max_gain = 0;
    dac_set_vol_table(sys_cb.vol_max);
    printf("[%s] vol_max:%d\n", __func__, sys_cb.vol_max);

    dac_aubuf_init(dac_obuf, sizeof(dac_obuf), DAC_OBUF_SIZE);
    dac_power_on();
    dac_msc_vol_by_src0();
    dac_max_gain = dac_max_gain_limit(DAC_MAX_GAIN);
    dac_set_max_vol(dac_max_gain_tbl_db[dac_max_gain]);
    plugin_music_eq();          //dac eq/drc配置
#if MUSIC_DECODE_BK_EN || WARNING_WAV_PLAY
    dac_src1_init();
#endif
#if BSP_DAC_DEBUG_EN
    dac_play_sin1k_test();
#endif

}


