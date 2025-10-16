#ifndef _BSP_DAC_H
#define _BSP_DAC_H

//数字音量
#define MAX_DIG_VAL         32767
#define MAX_DIG_P_VAL       0xFFFF

// #define DIG_P6DB            (MAX_DIG_VAL / 0.501187)
// #define DIG_P5DB            (MAX_DIG_VAL / 0.562341)
// #define DIG_P4DB            (MAX_DIG_VAL / 0.630957)
// #define DIG_P3DB            (MAX_DIG_VAL / 0.707946)
// #define DIG_P2DB            (MAX_DIG_VAL / 0.794328)
// #define DIG_P1DB            (MAX_DIG_VAL / 0.891251)
#define DIG_N0DB            (MAX_DIG_VAL / 1.000000)
#define DIG_N1DB            (MAX_DIG_VAL / 1.122018)
#define DIG_N2DB            (MAX_DIG_VAL / 1.258925)
#define DIG_N3DB            (MAX_DIG_VAL / 1.412538)
#define DIG_N4DB            (MAX_DIG_VAL / 1.584893)
#define DIG_N5DB            (MAX_DIG_VAL / 1.778279)
#define DIG_N6DB            (MAX_DIG_VAL / 1.995262)
#define DIG_N7DB            (MAX_DIG_VAL / 2.238721)
#define DIG_N8DB            (MAX_DIG_VAL / 2.511886)
#define DIG_N9DB            (MAX_DIG_VAL / 2.818383)
#define DIG_N10DB           (MAX_DIG_VAL / 3.162278)
#define DIG_N11DB           (MAX_DIG_VAL / 3.548134)
#define DIG_N12DB           (MAX_DIG_VAL / 3.981072)
#define DIG_N13DB           (MAX_DIG_VAL / 4.466836)
#define DIG_N14DB           (MAX_DIG_VAL / 5.011872)
#define DIG_N15DB           (MAX_DIG_VAL / 5.623413)
#define DIG_N16DB           (MAX_DIG_VAL / 6.309573)
#define DIG_N17DB           (MAX_DIG_VAL / 7.079458)
#define DIG_N18DB           (MAX_DIG_VAL / 7.943282)
#define DIG_N19DB           (MAX_DIG_VAL / 8.912509)
#define DIG_N20DB           (MAX_DIG_VAL / 10.000000)
#define DIG_N21DB           (MAX_DIG_VAL / 11.220185)
#define DIG_N22DB           (MAX_DIG_VAL / 12.589254)
#define DIG_N23DB           (MAX_DIG_VAL / 14.125375)
#define DIG_N24DB           (MAX_DIG_VAL / 15.848932)
#define DIG_N25DB           (MAX_DIG_VAL / 17.782794)
#define DIG_N26DB           (MAX_DIG_VAL / 19.952623)
#define DIG_N27DB           (MAX_DIG_VAL / 22.387211)
#define DIG_N28DB           (MAX_DIG_VAL / 25.118864)
#define DIG_N29DB           (MAX_DIG_VAL / 28.183829)
#define DIG_N30DB           (MAX_DIG_VAL / 31.622777)
#define DIG_N31DB           (MAX_DIG_VAL / 35.481339)
#define DIG_N32DB           (MAX_DIG_VAL / 39.810717)
#define DIG_N33DB           (MAX_DIG_VAL / 44.668359)
#define DIG_N34DB           (MAX_DIG_VAL / 50.118723)
#define DIG_N35DB           (MAX_DIG_VAL / 56.234133)
#define DIG_N36DB           (MAX_DIG_VAL / 63.095734)
#define DIG_N37DB           (MAX_DIG_VAL / 70.794578)
#define DIG_N38DB           (MAX_DIG_VAL / 79.432823)
#define DIG_N39DB           (MAX_DIG_VAL / 89.125094)
#define DIG_N40DB           (MAX_DIG_VAL / 100.000000)
#define DIG_N41DB           (MAX_DIG_VAL / 112.201845)
#define DIG_N42DB           (MAX_DIG_VAL / 125.892541)
#define DIG_N43DB           (MAX_DIG_VAL / 141.253754)
#define DIG_N44DB           (MAX_DIG_VAL / 158.489319)
#define DIG_N45DB           (MAX_DIG_VAL / 177.827941)
#define DIG_N46DB           (MAX_DIG_VAL / 199.526231)
#define DIG_N47DB           (MAX_DIG_VAL / 223.872114)
#define DIG_N48DB           (MAX_DIG_VAL / 251.188643)
#define DIG_N49DB           (MAX_DIG_VAL / 281.838293)
#define DIG_N50DB           (MAX_DIG_VAL / 316.227766)
#define DIG_N51DB           (MAX_DIG_VAL / 354.813389)
#define DIG_N52DB           (MAX_DIG_VAL / 398.107171)
#define DIG_N53DB           (MAX_DIG_VAL / 446.683592)
#define DIG_N54DB           (MAX_DIG_VAL / 501.187234)
#define DIG_N55DB           (MAX_DIG_VAL / 562.341325)
#define DIG_N56DB           (MAX_DIG_VAL / 630.957344)
#define DIG_N57DB           (MAX_DIG_VAL / 707.945784)
#define DIG_N58DB           (MAX_DIG_VAL / 794.328235)
#define DIG_N59DB           (MAX_DIG_VAL / 891.250938)
#define DIG_N60DB           0

//mic软件增益
//10^(n/20), n为DB数                                                  n
#define MAX_DIG_VAL          32767
#define DIG_P0DB            (int)(MAX_DIG_VAL * 1.000000)           //0db
#define DIG_P0_5DB          (int)(MAX_DIG_VAL * 1.059254)           //0.5db
#define DIG_P1DB            (int)(MAX_DIG_VAL * 1.122018)           //1db
#define DIG_P1_5DB          (int)(MAX_DIG_VAL * 1.188502)           //1.5db
#define DIG_P2DB            (int)(MAX_DIG_VAL * 1.258925)           //2db
#define DIG_P2_5DB          (int)(MAX_DIG_VAL * 1.333521)           //2.5db
#define DIG_P3DB            (int)(MAX_DIG_VAL * 1.412538)           //3db
#define DIG_P3_5DB          (int)(MAX_DIG_VAL * 1.496236)           //3.5db
#define DIG_P4DB            (int)(MAX_DIG_VAL * 1.584893)           //4db
#define DIG_P4_5DB          (int)(MAX_DIG_VAL * 1.678804)           //4.5db
#define DIG_P5DB            (int)(MAX_DIG_VAL * 1.778279)           //5db
#define DIG_P5_5DB          (int)(MAX_DIG_VAL * 1.883649)           //5.5db
#define DIG_P6DB            (int)(MAX_DIG_VAL * 1.995262)           //6db
#define DIG_P6_5DB          (int)(MAX_DIG_VAL * 2.113489)           //6.5db
#define DIG_P7DB            (int)(MAX_DIG_VAL * 2.238721)           //7db
#define DIG_P7_5DB          (int)(MAX_DIG_VAL * 2.371374)           //7.5db
#define DIG_P8DB            (int)(MAX_DIG_VAL * 2.511886)           //8db
#define DIG_P8_5DB          (int)(MAX_DIG_VAL * 2.660725)           //8.5db
#define DIG_P9DB            (int)(MAX_DIG_VAL * 2.818383)           //9db
#define DIG_P9_5DB          (int)(MAX_DIG_VAL * 2.985383)           //9.5db
#define DIG_P10DB           (int)(MAX_DIG_VAL * 3.162278)           //10db
#define DIG_P10_5DB         (int)(MAX_DIG_VAL * 3.349654)           //10.5db
#define DIG_P11DB           (int)(MAX_DIG_VAL * 3.548134)           //11db
#define DIG_P11_5DB         (int)(MAX_DIG_VAL * 3.758374)           //11.5db
#define DIG_P12DB           (int)(MAX_DIG_VAL * 3.981072)           //12db
#define DIG_P12_5DB         (int)(MAX_DIG_VAL * 4.216965)           //12.5db
#define DIG_P13DB           (int)(MAX_DIG_VAL * 4.466836)           //13db
#define DIG_P13_5DB         (int)(MAX_DIG_VAL * 4.731513)           //13.5db
#define DIG_P14DB           (int)(MAX_DIG_VAL * 5.011872)           //14db
#define DIG_P14_5DB         (int)(MAX_DIG_VAL * 5.308844)           //14.5db
#define DIG_P15DB           (int)(MAX_DIG_VAL * 5.623413)           //15db
#define DIG_P15_5DB         (int)(MAX_DIG_VAL * 5.956621)           //15.5db
#define DIG_P16DB           (int)(MAX_DIG_VAL * 6.309573)           //16db
#define DIG_P16_5DB         (int)(MAX_DIG_VAL * 6.683439)           //16.5db
#define DIG_P17DB           (int)(MAX_DIG_VAL * 7.079458)           //17db
#define DIG_P17_5DB         (int)(MAX_DIG_VAL * 7.498942)           //17.5db
#define DIG_P18DB           (int)(MAX_DIG_VAL * 7.943282)           //18db
#define DIG_P18_5DB         (int)(MAX_DIG_VAL * 8.413951)           //18.5db
#define DIG_P19DB           (int)(MAX_DIG_VAL * 8.912509)           //19db
#define DIG_P19_5DB         (int)(MAX_DIG_VAL * 9.440609)           //19.5db
#define DIG_P20DB           (int)(MAX_DIG_VAL * 10.000000)          //20db
#define DIG_P20_5DB         (int)(MAX_DIG_VAL * 10.592537)          //20.5db
#define DIG_P21DB           (int)(MAX_DIG_VAL * 11.220185)          //21db
#define DIG_P21_5DB         (int)(MAX_DIG_VAL * 11.885022)          //21.5db
#define DIG_P22DB           (int)(MAX_DIG_VAL * 12.589254)          //22db
#define DIG_P22_5DB         (int)(MAX_DIG_VAL * 13.335214)          //22.5db
#define DIG_P23DB           (int)(MAX_DIG_VAL * 14.125375)          //23db
#define DIG_P23_5DB         (int)(MAX_DIG_VAL * 14.962357)          //23.5db
#define DIG_P24DB           (int)(MAX_DIG_VAL * 15.848932)          //24db
#define DIG_P24_5DB         (int)(MAX_DIG_VAL * 16.788040)          //24.5db
#define DIG_P25DB           (int)(MAX_DIG_VAL * 17.782794)          //25db
#define DIG_P25_5DB         (int)(MAX_DIG_VAL * 18.836491)          //25.5db
#define DIG_P26DB           (int)(MAX_DIG_VAL * 19.952623)          //26db
#define DIG_P26_5DB         (int)(MAX_DIG_VAL * 21.134890)          //26.5db
#define DIG_P27DB           (int)(MAX_DIG_VAL * 22.387211)          //27db
#define DIG_P27_5DB         (int)(MAX_DIG_VAL * 23.713737)          //27.5db
#define DIG_P28DB           (int)(MAX_DIG_VAL * 25.118864)          //28db
#define DIG_P28_5DB         (int)(MAX_DIG_VAL * 26.607251)          //28.5db
#define DIG_P29DB           (int)(MAX_DIG_VAL * 28.183829)          //29db
#define DIG_P29_5DB         (int)(MAX_DIG_VAL * 29.853826)          //29.5db
#define DIG_P30DB           (int)(MAX_DIG_VAL * 31.622777)          //30db
#define DIG_P30_5DB         (int)(MAX_DIG_VAL * 33.496544)          //30.5db
#define DIG_P31DB           (int)(MAX_DIG_VAL * 35.481339)          //31db
#define DIG_P31_5DB         (int)(MAX_DIG_VAL * 37.583740)          //31.5db


#define PCM_OUT_24BITS              BIT(0)
#define PCM_OUT_MONO                BIT(1)
extern u16 cfg_pcm_out_format;

#define PCM_IS_24BITS()             (bool)(cfg_pcm_out_format & PCM_OUT_24BITS)

extern const u16 *dac_dvol_table;
extern const u16 dac_dvol_tbl_16[16 + 1];
extern const u16 dac_dvol_tbl_db[61];
extern const u16 dac_max_gain_tbl_db[61];

u8 bsp_volume_inc(u8 vol, u8 step);
u8 bsp_volume_dec(u8 vol, u8 step);
void bsp_change_volume(u8 vol);
bool bsp_set_volume(u8 vol);
void dac_init(void);

#endif // _BSP_DAC_H
