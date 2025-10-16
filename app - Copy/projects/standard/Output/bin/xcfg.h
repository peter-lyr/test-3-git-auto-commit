//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u32 lang_id                          : 2;   //提示音语言选择: 英文: 0, 中文: 1, 中英文(出厂默认英文): 2, 中英文(出厂默认中文): 3
    u32 lowpower_warning_en              : 1;   //低电提示
    u8 lpwr_warning_vbat;                       //低电语音提示电压: 0:1.9V, 1:2.0V, 2:2.1V, 3:2.2V, 4:2.3V, 5:2.4V, 6:2.5V, 7:2.6V, 8:2.7V, 9:2.8V, 10:2.9V, 11:3.0V, 12:3.1V, 13:3.2V, 14:3.3V, 15:3.4V, 16:3.5V, 17:3.6V, 18:3.7V
    u8 lpwr_off_vbat;                           //低电关机电压: 0:不关机, 1:1.9V, 2:2.0V, 3:2.1V, 4:2.2V, 5:2.3V, 6:2.4V, 7:2.5V, 8:2.6V, 9:2.7V, 10:2.8V, 11:2.9V, 12:3.0V, 13:3.1V, 14:3.2V, 15:3.3V, 16:3.4V, 17:3.5V, 18:3.6V, 19:3.7V
    u8 lpwr_warning_period;                     //低电语音播报周期(秒)
    u32 vol_max                          : 2;   //音量级数: 16级音量: 0, 32级音量: 1, 50级音量: 2
    u8 sys_init_vol;                            //开机默认音量
    u8 warning_volume;                          //提示音播放音量
    u32 vddio_sel                        : 5;   //VDDIO电压: 1.5V: 0, 1.6V: 2, 1.7V: 3, 1.8V: 4, 1.9V: 6, 2.0V: 7, 2.1V: 8, 2.2V: 10, 2.3V: 11, 2.4V: 12, 2.5V: 14, 2.6V: 15, 2.7V: 16, 2.8V: 18, 2.9V: 19, 3.0V: 20, 3.1V: 22, 3.2V: 23, 3.3V: 24, 3.4V: 26, 3.5V: 27, 3.6V: 28, 3.7V: 30, 3.8V: 31
    u32 reset_sel                        : 3;   //复位电压: 1.8V: 0, 2.0V: 1, 2.2V: 2, 2.4V: 3, 2.6V: 4, 2.8V: 5, 3.0V: 6, 1.7V: 7
    u32 reset_sel2                       : 3;   //中断电压: 1.8V: 0, 2.0V: 1, 2.2V: 2, 2.4V: 3, 2.6V: 4, 2.8V: 5, 3.0V: 6, 1.7V: 7
    u32 dac_max_gain                     : 4;   //DAC最大音量: +6DB: 0, +5DB: 1, +4DB: 2, +3DB: 3, +2DB: 4, +1DB: 5, 0DB: 6, -1DB: 7, -2DB: 8, -3DB: 9, -4DB: 10, -5DB: 11, -6DB: 12, -7DB: 13, -8DB: 14, -9DB: 15
    u32 dac_soft_gain                    : 6;   //DAC软件增益(0~31DB)
    u32 mic_bias_method                  : 3;   //MIC偏置电路配置: 省电阻单端输入: 0, 省电阻差分输入: 1, 省电阻电容: 2, 外部电阻电容单端输入: 3, 外部电阻电容差分输入: 4
    u32 mic_rias_sel                     : 4;   //MIC内部电阻选择: 16.36K: 1, 8.18K: 2, 5.45K: 3, 4.09K: 4, 3.27K: 5, 2.73K: 6, 2.34K: 7, 2.05K: 8, 1.82K: 9, 1.64K: 10, 1.49K: 11, 1.36K: 12, 1.26K: 13, 1.17K: 14, 1.09K: 15
    u32 mic_bias_en                      : 1;   //MIC_BIAS使能
    u32 mic_anl_gain                     : 2;   //MIC模拟增益(6~12DB): 6DB: 0, 12DB: 1
    u32 mic_dig_gain                     : 6;   //MIC数字增益(0~36DB)
    u32 mic_soft_gain                    : 6;   //MIC软件增益(0~31DB)
    u32 user_adkey1_en                   : 1;   //ADKEY1功能
    u32 user_adkey2_en                   : 1;   //ADKEY2功能
    u32 user_iokey_en                    : 1;   //IOKEY功能
    u32 adkey_ch                         : 4;   //ADKEY1通路选择: ADCCH_PA0: 0, ADCCH_PA1: 1, ADCCH_PA2: 2, ADCCH_PA3: 3, ADCCH_PA4: 4, ADCCH_PA5: 5, ADCCH_PA10: 6, ADCCH_PA11: 7, ADCCH_PA13: 8, ADCCH_PA14: 9, ADCCH_PA15: 10, ADCCH_PB0: 11, ADCCH_PB3: 12, ADCCH_PB4: 13
    u32 adkey_ch2                        : 4;   //ADKEY2通路选择: ADCCH_PA0: 0, ADCCH_PA1: 1, ADCCH_PA2: 2, ADCCH_PA3: 3, ADCCH_PA4: 4, ADCCH_PA5: 5, ADCCH_PA10: 6, ADCCH_PA11: 7, ADCCH_PA13: 8, ADCCH_PA14: 9, ADCCH_PA15: 10, ADCCH_PB0: 11, ADCCH_PB3: 12, ADCCH_PB4: 13
    u32 user_key_multi_press_en          : 1;   //按键多击功能
    u32 double_key_time                  : 3;   //双击响应时间选择: 200ms: 0, 300ms: 1, 400ms: 2, 500ms: 3, 600ms: 4, 700ms: 5, 800ms: 6, 900ms: 7
    u32 pwron_press_time                 : 3;   //软开机长按时间选择: 0.1秒: 0, 0.5秒: 1, 1秒: 2, 1.5秒: 3, 2秒: 4, 2.5秒: 5, 3秒: 6, 3.5秒: 7
    u32 pwroff_press_time                : 3;   //软关机长按时间选择: 1.5秒: 0, 2秒: 1, 2.5秒: 2, 3秒: 3, 3.5秒: 4, 4秒: 5, 4.5秒: 6, 5秒: 7
    u32 key_multi_config_en              : 1;   //支持多击的按键定制
    u32 dblkey_num0                      : 5;   //多击按键1选择: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 dblkey_num1                      : 5;   //多击按键2选择: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 dblkey_num2                      : 5;   //多击按键3选择: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 dblkey_num3                      : 5;   //多击按键4选择: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 dblkey_num4                      : 5;   //多击按键5选择: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_io0                        : 5;   //IOKEY按键1的IO: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 iokey_io1                        : 5;   //IOKEY按键2的IO: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 iokey_io2                        : 5;   //IOKEY按键3的IO: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 iokey_io3                        : 5;   //IOKEY按键4的IO: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 iokey_io4                        : 5;   //IOKEY按键5的IO: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 iokey_num0                       : 5;   //IOKEY按键1功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_num1                       : 5;   //IOKEY按键2功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_num2                       : 5;   //IOKEY按键3功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_num3                       : 5;   //IOKEY按键4功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_num4                       : 5;   //IOKEY按键5功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 iokey_midkey_num                 : 5;   //两IO中间的按键功能: None: 0, PP/POWER: 1, P/P: 2, POWER: 3, NEXT/VOL+: 4, VOL+/NEXT: 5, PREV/VOL-: 6, VOL-/PREV: 7, VOL+: 8, VOL-: 9, NEXT: 10, PREV: 11, MODE: 12, PP/MODE: 13, HSF: 14, EQ: 15, REPEAT: 16, MUTE: 17, MODE/POWER: 18, REC: 19, PLAY/HSF: 20, A-B: 21
    u32 spi1_slave_master                : 2;   //SPI1主从模式: SPI1从机: 0, SPI1主机: 1
    u32 led_disp_en                      : 1;   //系统指示灯(蓝灯)
    u32 led_pwr_en                       : 1;   //电源状态灯(红灯)
    u32 rled_lowbat_en                   : 1;   //电池低电闪灯
    u32 port_2led_resless_en             : 1;   //省电阻1个IO推两个灯
    u32 bled_io_sel                      : 5;   //蓝灯IO选择: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
    u32 rled_io_sel                      : 5;   //红灯IO选择: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PA8: 9, PA9: 10, PA10: 11, PA11: 12, PA12: 13, PA13: 14, PA14: 15, PA15: 16, PB0: 17, PB1: 18, PB2: 19, PB3: 20, PB4: 21, PB5: 22, PB6: 23, PB7: 24, PB8: 25, PB9: 26, PB10: 27, PB11: 28
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
