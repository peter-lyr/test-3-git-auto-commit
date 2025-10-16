/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/*****************************************************************************
 * Module    : 显示相关配置选择列表
 *****************************************************************************/
//显示驱动屏选择
#define DISPLAY_NO                      0                                       //无显示模块
#define DISPLAY_LCD                     0x100                                   //选用LCD点阵屏做为显示驱动
#define DISPLAY_LCDSEG                  0x200                                   //选用断码屏做为显示驱动
#define DISPLAY_LEDSEG                  0x400                                   //选用数码管做为显示驱动

#define GUI_NO                          DISPLAY_NO                              //无主题，无显示
#define GUI_LCD                         (DISPLAY_LCD | 0x00)                    //LCD点阵屏(128*64)
#define GUI_LCDSEG                      (DISPLAY_LCDSEG | 0x00)                 //断码屏默认主题
#define GUI_LEDSEG_5C7S                 (DISPLAY_LEDSEG | 0x00)                 //5C7S 数码管
#define GUI_LEDSEG_7P7S                 (DISPLAY_LEDSEG | 0x01)                 //7PIN 数码管 按COM方式扫描
#define GUI_LEDSEG_3P7S                 (DISPLAY_LEDSEG | 0x02)                 //3PIN 7段数码管
#define GUI_LEDSEG_6C6S                 (DISPLAY_LEDSEG | 0x04)                 //6C6S 6段数码管

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
#define FSIZE_16M                       0x1000000   //MAX flash size
#define FSIZE_8M                        0x8000000
#define FSIZE_4M                        0x400000
#define FSIZE_2M                        0x200000
#define FSIZE_1M                        0x100000
#define FSIZE_512K                      0x80000
#define FSIZE_256K                      0x40000
#define FSIZE_128K                      0x20000

/*****************************************************************************
 * Module    : 提示音语言配置选择列表
 *****************************************************************************/
#define LANG_EN         0               //英文提示音
#define LANG_ZH         1               //中文提示音
#define LANG_EN_ZH      2               //英文、中文双语提示音

/*****************************************************************************
 * Module    : ADC通路选择列表
 *****************************************************************************/
#define ADCCH_PA0       0               //SARADC channel 0
#define ADCCH_PA1       1               //SARADC channel 1
#define ADCCH_PA2       2               //SARADC channel 2
#define ADCCH_PA3       3               //SARADC channel 3
#define ADCCH_PA4       4               //SARADC channel 4
#define ADCCH_PA5       5               //SARADC channel 5
#define ADCCH_PA10      6               //SARADC channel 6
#define ADCCH_PA11      7               //SARADC channel 7
#define ADCCH_PA13      8               //SARADC channel 8
#define ADCCH_PA14      9               //SARADC channel 9
#define ADCCH_PA15      10              //SARADC channel 10
#define ADCCH_PB0       11              //SARADC channel 11
#define ADCCH_PB3       12              //SARADC channel 12
#define ADCCH_PB4       13              //SARADC channel 13
#define ADCCH_VBAT      14              //SARADC channel 14     vbatdiv2
#define ADCCH_BGOP      15              //SARADC channel 15
#define ADCCH_TSENSOR   15              //SARADC channel 15
#define ADCCH_VRTC      15              //SARADC channel 15     vddcor/vlcore
#define ADCCH_MIC       15              //SARADC channel 15

/*****************************************************************************
 * Module    : Timer Capture Mapping选择列表
 *****************************************************************************/
#define TMR0MAP_PA14    (1 << 0)        //TMR0CAP G1  capture mapping: PA14
#define TMR1MAP_PB9     (1 << 0)        //TMR1CAP G1  capture mapping: PB9
#define TMR2MAP_PA1     (1 << 0)        //TMR2CAP G1  capture mapping: PA1


/*****************************************************************************
 * Module    : IRRX Mapping选择列表
 *****************************************************************************/
#define IRMAP_PA1       (1 << 20)       //G1  capture mapping: PA1
#define IRMAP_PA2       (2 << 20)       //G2  capture mapping: PA2
#define IRMAP_PA7       (3 << 20)       //G3  capture mapping: PA7
#define IRMAP_PB2       (4 << 20)       //G4  capture mapping: PB2
#define IRMAP_PB5       (5 << 20)       //G5  capture mapping: PB5
#define IRMAP_PB6       (6 << 20)       //G6  capture mapping: PB6
#define IRMAP_PE0       (7 << 20)       //G7  capture mapping: PE0
#define IRMAP_PF0       (8 << 20)       //G8  capture mapping: PF0


/*****************************************************************************
 * Module    : Clock output Mapping选择列表
 *****************************************************************************/
#define CLKOMAP_PB6     (1 << 0)        //G1 Clock output mapping: PB6
#define CLKOMAP_PA3     (2 << 0)        //G2 Clock output mapping: PA3
#define CLKOMAP_PB1     (3 << 0)        //G3 Clock output mapping: PB1
#define CLKOMAP_PB4     (4 << 0)        //G4 Clock output mapping: PB4
#define CLKOMAP_PB3     (5 << 0)        //G5 Clock output mapping: PB3
#define CLKOMAP_PA9     (6 << 0)        //G6 Clock output mapping: PB9
#define CLKOMAP_PA12    (7 << 0)        //G7 Clock output mapping: PB12

/*****************************************************************************
 * Module    : sd0 Mapping选择列表
 *****************************************************************************/
#define SD0MAP_G1       (1 << 0)       //G1  SDCLK(PA1), SDCMD(PA2), SDDAT0(PA3)
#define SD0MAP_G2       (2 << 0)       //G2  SDCLK(PA11), SDCMD(PA12), SDDAT0(PA10)
#define SD0MAP_G3       (3 << 0)       //G3  SDCLK(PA4), SDCMD(PA5), SDDAT0(PB4)
#define SD0MAP_G4       (4 << 0)       //G4  SDCLK(PA4), SDCMD(PA5), SDDAT0(PA6)


/*****************************************************************************
 * Module    : spi1 Mapping选择列表 (FUNCMCON1)
 *****************************************************************************/
#define SPI1MAP_G1     (1 << 12)        //G1 SPI1CLK(PA3),  SPI1DI(PB4),  SPI1DO(PB3)
#define SPI1MAP_G2     (2 << 12)        //G2 SPI1CLK(PA11), SPI1DI(PA10), SPI1DO(PA12)
#define SPI1MAP_G3     (3 << 12)        //G3 SPI1CLK(PA4),  SPI1DI(PA6),  SPI1DO(PA5)
#define SPI1MAP_G4     (4 << 12)        //G4 SPI1CLK(PA14),  SPI1DI(PA13),  SPI1DO(PA15)


/*****************************************************************************
 * Module    : uart0 Mapping选择列表
 *****************************************************************************/
#define UTX0MAP_PA4     (1 << 8)        //G1 uart0 tx: PA4
#define UTX0MAP_PA0     (2 << 8)        //G2 uart0 tx: PA0
#define UTX0MAP_PB3     (3 << 8)        //G3 uart0 tx: PB3
#define UTX0MAP_PA9     (4 << 8)        //G4 uart0 tx: PA9

#define URX0MAP_PA1     (1 << 12)       //G1 uart0 rx: PA1
#define URX0MAP_PA5     (2 << 12)       //G2 uart0 rx: PA5
#define URX0MAP_PB4     (3 << 12)       //G3 uart0 rx: PB4
#define URX0MAP_PA10    (4 << 12)       //G4 uart0 rx: PA10
#define URX0MAP_TX      (7 << 12)       //G7 uart0 map to TX pin by UT0TXMAP select(1线模式)

/*****************************************************************************
* Module    : hsuart select table
*****************************************************************************/
#define HSTX_PA9        (1 << 16)       //HSTX G1: PA9
#define HSTX_PB3        (2 << 16)       //HSTX G2: PB3
#define HSTX_PB4        (3 << 16)       //HSTX G3: PB4
#define HSTX_PA12       (4 << 16)       //HSTX G4: PA12
#define HSTX_PA0        (5 << 16)       //HSTX G5: PA0
#define HSTX_PA7        (6 << 16)       //HSTX G6: PA7

#define HSRX_PA9        (1 << 20)       //HSRX G1: PA9
#define HSRX_PB3        (2 << 20)       //HSRX G2: PB3
#define HSRX_PB4        (3 << 20)       //HSRX G3: PB4
#define HSRX_PA12       (4 << 20)       //HSRX G4: PA12
#define HSRX_PA0        (5 << 20)       //HSRX G5: PA0
#define HSRX_PA7        (6 << 20)       //HSRX G6: PA7

/*****************************************************************************
 * Module    : 录音文件类型列表
 *****************************************************************************/
#define REC_NO          0
#define REC_WAV         1              //PCM WAV
#define REC_ADPCM       2              //ADPCM WAV
#define REC_MP3         3
#define REC_ESBC        4

/*****************************************************************************
* Module    : TouchKey通路
*****************************************************************************/
#define TK_PA2_CH0          0
#define TK_PA7_CH1          1
#define TK_PB2_CH2          2
#define TK_PB6_CH3          3

/*****************************************************************************
* Module    : DAC OUT Sample Rate
*****************************************************************************/
#define DAC_OUT_44K1        0       //dac out sample rate 44.1K
#define DAC_OUT_48K         1       //dac out sample rate 48K

/*****************************************************************************
* Module    : DAC LDOH Select
*****************************************************************************/
#define AU_LDOH_2V4         0       //VDDAUD LDO voltage 2.4V
#define AU_LDOH_2V5         1       //VDDAUD LDO voltage 2.5V
#define AU_LDOH_2V7         2       //VDDAUD LDO voltage 2.7V
#define AU_LDOH_2V9         3       //VDDAUD LDO voltage 2.9V
#define AU_LDOH_3V1         4       //VDDAUD LDO voltage 3.1V
#define AU_LDOH_3V2         5       //VDDAUD LDO voltage 3.2V

/*****************************************************************************
* Module    : 电池低电电压列表
*****************************************************************************/
#define VBAT_2V8            0       //2.8v
#define VBAT_2V9            1       //2.9v
#define VBAT_3V0            2       //3.0v
#define VBAT_3V1            3       //3.1v
#define VBAT_3V2            4       //3.2v
#define VBAT_3V3            5       //3.3v
#define VBAT_3V4            6       //3.4v
#define VBAT_3V5            7       //3.5v
#define VBAT_3V6            8       //3.6v
#define VBAT_3V7            9       //3.7v
#define VBAT_3V8            10      //3.8v

/*****************************************************************************
* Module    : uart0 printf IO列表
*****************************************************************************/
#define PRINTF_NONE         0           //关闭UART0打印信息
#define PRINTF_PA0          1
#define PRINTF_PA4          2
#define PRINTF_PB3          3
#define PRINTF_PA9          4

/*****************************************************************************
* Module    : GPIO list
*****************************************************************************/
#define IO_NONE             0
#define IO_PA0              1
#define IO_PA1              2
#define IO_PA2              3
#define IO_PA3              4
#define IO_PA4              5
#define IO_PA5              6
#define IO_PA6              7
#define IO_PA7              8
#define IO_PA8              9
#define IO_PA9              10
#define IO_PA10             11
#define IO_PA11             12
#define IO_PA12             13
#define IO_PA13             14
#define IO_PA14             15
#define IO_PA15             16
#define IO_PB0              17
#define IO_PB1              18
#define IO_PB2              19
#define IO_PB3              20
#define IO_PB4              21
#define IO_PB5              22
#define IO_PB6              23
#define IO_MAX_NUM          23

#define IO_MUX_SDCLK        26
//#define IO_MUX_SDCMD        27      //IO内部没300R上拉，不支持复用检测

/*****************************************************************************
* Module    : wakeup circuit port
*****************************************************************************/
#define WKU_PA7             0
#define WKU_PB0             1
#define WKU_PB1             2
#define WKU_PB2             3
#define WKU_PB9             4
#define WKU_PA0             5

/*****************************************************************************
* Module    : Function output select table
*****************************************************************************/
#define FUNCO_PWM0          0
#define FUNCO_PWM1          1
#define FUNCO_PWM2          2
#define FUNCO_PWM3          3
#define FUNCO_TMR2PWM0      4
#define FUNCO_TMR2PWM1      5
#define FUNCO_UART0TX       6
#define FUNCO_HSUTTX        7
#define FUNCO_IIC_SDA       8
#define FUNCO_IIC_SCL       9
#define FUNCO_SPI1_DO       10
#define FUNCO_SPI1_DI       11
#define FUNCO_SPI1_CLK      12
#define FUNCO_RES0          13
#define FUNCO_RES1          14
#define FUNCO_CLKOUT        15

/*****************************************************************************
* Module    : EX_SPIFLASH_SUPPORT
*****************************************************************************/
#define EXSPI_NOT_SUPPORT   0
#define EXSPI_REC           (1<<0)    //外接SPIFLASH支持录音
#define EXSPI_MUSIC         (1<<1)    //外接SPIFLASH支持MP3音乐镜像文件播放

/*****************************************************************************
 * Module    : I2C Mapping选择列表 FUNCMCON2[27:24]
 *****************************************************************************/
#define I2CMAP_PB1PB0       (1 << 24)        //G1 i2c clk: PB1  dat: PB0
#define I2CMAP_PA1PA2       (2 << 24)        //G2 i2c clk: PA1  dat: PA2
#define I2CMAP_PA6PA7       (3 << 24)        //G3 i2c clk: PA6  dat: PA7
#define I2CMAP_PB8PB9       (4 << 24)        //G4 i2c clk: PB8  dat: PB9



#endif //CONFIG_DEFINE_H
