/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "config_define.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_MUSIC_EN                   1                       //是否打开SD/UDISK音乐播放功能
#define FUNC_CLOCK_EN                   0                       //是否打开时钟功能
#define FUNC_SPEAKER_EN                 0                       //是否打开SPEAKER功能
#define FUNC_AUX_EN                     0                       //是否打开AUX功能
#define FUNC_USBDEV_EN                  0                       //是否打开USB DEVICE功能
#define FUNC_IDLE_EN                    0                       //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_48M                 //选择系统时钟
#define EXT_32K_EN                      0                       //是否使用外部32.768K晶振,否则内部32K OSC
#define MCLR_RESRT_MDOE                 0                       //0-disable, 1-长按复位(PA0), 2-短按复位(PA7)
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级

#define SOFT_POWER_ON_OFF               1                       //是否使用软开关机功能
#define SYS_SLEEP_TIME                  0                       //自动休眠时间(time * 1秒)
#define SYS_OFF_TIME                    0                       //自动关机时间(time * 30秒)
#define POWER_DOWN_MODE                 PWR_L4M                 //PWR_DOWN模式

#define POWER_ON_FALL_NUM               0                       //低电平唤醒IO个数(0-即不使用低电平唤醒)
#define POWER_ON_FALL_IO                {IO_PA0}                //低电平唤醒IO(支持任意IO唤醒)
#define POWER_ON_RISE_NUM               0                       //高电平唤醒IO个数(0-即不使用高电平唤醒)
#define POWER_ON_RISE_IO                {IO_PA2}                //高电平唤醒IO(支持任意IO唤醒)
#define POWER_OFF_IO                    IO_NONE                 //关机IO设置(用于避免按键未松开就进行关机流程,实际关机接口func_pwroff,如不使用IO关机,则设置为IO_NONE)
#define PB7_SHORT_PB8_EN                0                       //PB7和PB8是否合封，解决它俩无法IO唤醒的问题

#define GUI_SELECT                      GUI_NO                  //GUI Display Select
#define UART0_PRINTF_SEL                PRINTF_PA9              //选择UART打印信息输出IO，或关闭打印信息输出

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_512K              //芯片内置Flash 512KB,实际导出prd文件要小于496K,param占用16k)
#define FLASH_CODE_SIZE                 496K                    //程序使用空间大小
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 1                       //是否支持2线模式(2bit data)
#define FLASH_QUAD_READ                 0                       //是否支持4线模式(4bit data) (会占用到PA6, PA5两个IO)
#define FLASH_2WIRE_EN                  0                       //SPI FLASH 2-wire模式，1bit data in/out
#define SPIFLASH_SPEED_UP_EN            1                       //SPI FLASH提速


#define UFLASH_BIN_EN                   0                       //用户资源bin使能
#define UFLASH_BIN_ADDR                 0x20000                 //用户资源bin存放地址
#define UFLASH_BIN_LEN                  0x10000                 //用户资源bin长度(4k对齐)
#define UFLASH_BIN_DATA_LEN             0x10000                 //用户资源bin数据实际长度

/*****************************************************************************
 * Module    : SPEAKER功能
 *****************************************************************************/
#define SPK_MIC_APF_CHG_EN              0                       //是否开启随机相位(防啸叫)
#define SPK_MIC_FREQ_SHIFT2_EN          0                       //是否开启移频（防啸叫）
#define SPK_MIC_ANGLE_CHANGE_EN         0                       //是否开启angle_change（防啸叫）
#define SPK_MIC_ECHO_EN                 0                       //是否使能echo功能
#define SPK_MIC_REVERB_EN               0                       //是否使能reverb功能
#define SPK_MIC_VOICE_CHANGER_EN        0                       //是否使能变音功能(实时变音)

#define SPK_MIC_WSOLA_REC_EN            0                       //是否使能录变速功能(录音变速)
#define SPK_MIC_WSOLA_INPUT_LEN         112                     //变速倍数：input_len / HS_LEN，速度(64-128)(adpcm变速的input_len只能是8的倍数)

#define VOICE_CHANGER_ROBOT_EN          0                       //是否使能变音功能-机械音变音(会复用aram))
#define VOICE_CHANGER_FS_EN             0                       //是否使能变音共振峰修正

#define SPK_MIC_ECHO_LEVEL              55                      //MIC回音衰减百分比(建议30-80之间)
#define SPK_MIC_ECHO_DELAY              376                     //MIC回音延时(1~500ms)

#define SPK_MIC_MAXPOW_EN               0                       //Speaker模式MIC能量检测使能
#define MIC_SOFT_GAIN                   xcfg_cb.mic_soft_gain   //配置MIC软件数字增益0~31dB(0~63)

#define SPK_MIC_DNR_FRE_EN              0                       //是否使能dnr传统降噪(16k)
#define SPK_MIC_DNR_FRE_MODE            1                       //dnr降噪(模式1：支持录音降噪,不支持同时开变音；模式2：支持开变音降噪，不支持同时录音)
#define SPK_MIC_DNR_FRE_LEVEL           16384                   //降噪量, 越大降噪越弱，最大32767
/*****************************************************************************
 * Module    : MIDI功能配置
 *****************************************************************************/
#define MIDI_EN                         0                       //midi模块使能
#define VOICE_BIN_ADDR                  0x40000                 //voice.bin存放地址
#define VOICE_BIN_LEN                   0x2E000                 //voice.bin加载长度(4k对齐)
#define VOICE_DATA_LEN                  0x2D3EC                 //voice.bin数据实际长度
#define MIDI_VOICE_NUM                  64                      //MIDI发音数(1~64)
#define MIDI_TRACK_NUM                  16                      //MIDI轨道数(1~16)
#define MIDI_CHANNEL_NUM                16                      //MIDI通道数(1~16)
#define MIDI_DEC_BK_EN                  1                       //MIDI后台解码模式

#define MIDI_SONG_EN                    1                       //MIDI歌曲使能
#define MIDI_TEST_EN                    1                       //MIDI歌曲测试代码
#define MIDI_OKON_EN                    0                       //MIDI one key one note功能使能(需打开MIDI_SONG_EN)

#define MIDI_KEYBOARD_EN                1                       //MIDI键盘使能
#define MIDI_KEYS_NUM                   MKEY_25                 //MIDI键盘按键数
#define MIDI_KEYS_TEST                  0                       //MIDI键盘测试代码
#define MIDI_UART0_EN                   0                       //是否开启MIDI uart0(开启后需要设UART0_PRINTF_SEL为PRINTF_NONE)

#define MIDI_REC_EN                     0                       //MIDI录音使能
#define MIDI_REC_TEST_EN                0                       //MIDI录音测试代码
#define MIDI_SPIFLASH_REC_BEGIN_ADDR    0xE8000                 //MIDI录音起始地址
#define MIDI_SPIFLASH_REC_END_ADDR      0xFFFFF                 //MIDI录音结束地址
#define MIDI_SPIFLASH_REC_LIMIT         0x400                   //单个midi录音文件限制大小

#define MIDI_METRO_EN                   0                       //MIDI节拍器使能
#define MIDI_METRO_CH                   9                       //MIDI节拍器使用的通道(默认使用鼓组通道9,并且voice.bin需要打包鼓组的音色)
#define MIDI_METRO_BPM                  120                     //MIDI节拍器默认每分钟节拍数(BPM:30~300)

/*****************************************************************************
* Module    : ASR配置控制
******************************************************************************/
#define ASR_RECOG_EN                    0                       //是否使能语音识别功能
#define ASR_LIBS_SEL                    ASR_ZH                  //选择中英文识别库
#define TDNN_HW_EN                      1                       //硬件TDNNACC

#define WEIGHT_BIN_ADDR                 0x28000                 //weight.bin存放地址
#define WEIGHT_BIN_LEN                  0x54000                 //weight.bin对齐长度
#define WEIGHT_DATA_LEN                 0x53400                 //实际长度

#define FFT_PROC_EN                     0                       //FFT处理接口使能

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#define MUSIC_UDISK_EN                  0   //是否支持播放UDISK
#define MUSIC_SDCARD_EN                 1   //是否支持播放SDCARD
#define MUSIC_SDCARD1_EN                0   //是否支持播放双卡

#define MUSIC_WAV_SUPPORT               1   //是否支持WAV格式解码
#define MUSIC_ESBC_SUPPORT              0   //是否支持ESBC格式解码
#define MUSIC_LSBC_SUPPORT              0   //是否支持ESBC格式解码
#define MUSIC_SBC_SUPPORT               0   //是否支持SBC格式解码

#define MUSIC_FOLDER_SELECT_EN          0   //文件夹选择功能
#define MUSIC_AUTO_SWITCH_DEVICE        0   //双设备循环播放
#define MUSIC_BREAKPOINT_EN             0   //音乐断点记忆播放
#define MUSIC_QSKIP_EN                  0   //快进快退功能
#define MUSIC_PLAYMODE_NUM              1   //音乐播放模式总数
#define MUSIC_MODE_RETURN               0   //退出音乐模式之后是否返回原来的模式
#define MUSIC_PLAYDEV_BOX_EN            0   //是否显示“USB”, "SD"界面
#define MUSIC_ID3_TAG_EN                0   //是否获取MP3 ID3信息
#define MUSIC_REC_FILE_FILTER           0   //是否区分录音文件与非录音文件分别播放
#define MUSIC_LRC_EN                    0   //是否支持歌词显示
#define MUSIC_NAVIGATION_EN             0   //音乐文件导航功能(LCD点阵屏功能)
#define MUSIC_ENCRYPT_EN                0   //是否支持加密MP3文件播放(使用MusicEncrypt.exe工具进行MP3加密)

#define MUSIC_FILE_SORT_CHAR_EN         0   //是否将文件按字符排序
#define MUSIC_FILE_SORT_NUM             10  //参与排序文件数上限

#define MUSIC_ENCRYPT_KEY               12345   //MusicEncrypt.exe工具上填的加密KEY
#define SPI_MP3_ENCRYPT_KEY             12345   //MusicEncrypt.exe工具上填的加密KEY

#define MUSIC_DECODE_BK_EN              0       //使能后音频在大循环上进行解码，可以layer0+layer1叠加播放(一般使用lsbc/mp3/midi叠加wav播放)
#define MUISC_WAV_LAYER_EN              MSC_WAV_SEL//wav叠加层数控制(配置叠加层数越多,越占用buf空间)

#define MUSIC_MODE_TABLE_EN             0       //使能后自动切换自定义表格里的歌曲（见bsp_music.c）

#define MP3_STEREO_SUPPORT_EN           1       //是否使能播双声道MP3（增加ram占用，platform和codecs库需使能）

#define VMP3_BREAK_POINT_EN             0       //是否开启(v)mp3播放断点记忆
#define VMP3_BREAK_POINT_SAVE_PORIOD    2       //每当经过多长时间(单位:100ms)保存一次断点
#define VMP3_BREAK_POINT_BACK_SECOND    1       //多往回播多长时间(单位:秒)


/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#define FUNC_REC_EN                     0   //录音功能总开关
#define REC_SPEAKER_EN                  1   //是否SPEAKER模式录音
#define REC_AUX_EN                      1   //是否AUX模式录音
#define SDCARD_UDICK_REC                0   //是否录入SD卡
#define REC_INTERNAL_FLASH              1   //1-录入SPI0flash, 0-录入SPI1flash,并要设置EX_SPIFLASH_SUPPORT为 EXSPI_REC
#define REC_FAST_PLAY_INTERNAL          1   //快速播放最新的录音文件
#define MIC_DET_REC                     0   //是否开启能量检测自动录音，需要打开SPK_MIC_MAXPOW_EN宏
#define REC_MIC_WAV_OVERLAY_EN          0   //是否使能mic+wav播放叠加录音

#define REC_MP3_SUPPORT                 1   //是否支持MP3音频格式录音
#define REC_WAV_SUPPORT                 0   //是否支持WAV音频格式录音
#define REC_ADPCM_SUPPORT               0   //是否支持ADPCM音频格式录音

#define REC_PLAY_MODE_SEL               1   //0-使用res_play独占播放, 1-使用song_play后台播放(可播放录音时叠加wav播放)

//#define REC_FAST_PLAY                   0   //播卡播U下快速播放最新的录音文件(双击REC)

/*****************************************************************************
* Module    : AUX功能配置
******************************************************************************/
#define AUX_CHANNEL_CFG                 0///CH_AUX_PA14     //选择LINEIN通路
#define AUX_ANL_GAIN                    0               //0: 0DB, 1:6DB
#define AUX_DIG_GAIN                    0               //AUX SDADC数字增益0~36DB

#define LINEIN_DETECT_EN                0               //是否打开LINEIN检测
#define AUX_DET_GPIO_SEL                IO_PA15         //复用检测时可以选IO_MUX_SDCLK

#define LINEIN_DETECT_INIT()        linein_detect_init()
#define LINEIN_IS_ONLINE()          linein_is_online()
#define IS_DET_LINEIN_BUSY()        is_detect_linein_busy()


/*****************************************************************************
* Module    : DAC配置控制
******************************************************************************/
#define DAC_EN                          1               //DAC使能(PB0输出DAC信号)
#define DAC_CLASSD_EN                   1               //DAC classd输出(使能后,会关闭PB0 DAC信号输出,通过voutp/voutn引脚输出ClassD信号)
#define DAC_CLASSD_MODE                 0               //0-BD mode0, 1-BD mode1, 2-BD mode2, 3-AD mode
#define DAC_DRV_SEL                     7               //0-默认功率驱动,1~7增加输出功率驱动(设7功率最大)
#define DAC_CLASSD_SPEED                6               //0~3:slow1~slow4, 4~7:fast1~fast8
#define DAC_CLASSD_VDET                 1               //ClassD过压检测(需要同时开启电池电量检测功能)
#define DAC_CLASSD_SW_EN                0               //0-classd功放输出, 1-VOUTN/P用做普通IO, 默认输出高(通过dac_voutp_set/dac_voutn_set控制)
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain    //配置DAC最大输出幅度
#define DAC_SOFT_GAIN                   xcfg_cb.dac_soft_gain   //配置DAC软件数字增益0~31dB(0~63)
#define DAC_OUT_SPR                     DAC_OUT_48K     //dac out sample rate
#define DAC_DRC_EN                      0               //DAC DRC V3.0使能位
#define DAC_24BITS_EN                   0               //aubuf缓存buf会多一倍
#define SPI_DUMP_EN                     0               //SPI dump en

#if MIDI_EN
#define WAV_FIX_SPR_EN                  1               //是否固定WAV以固定采样率WAV_FIX_SPR_VAL去推DAC
#define WAV_FIX_SPR_VAL                 48000           //当WAV实际采样率为8K时，该宏应为8K的倍数，插点到此采样率再推dac
#else
#define WAV_FIX_SPR_EN                  0
#define WAV_FIX_SPR_VAL                 16000
#endif

/*****************************************************************************
 * Module    :外接SPIFLASH配置, 外接SPIFLASH可以播放MP3音乐文件, 及录音
 *****************************************************************************/
#define EX_SPIFLASH_SUPPORT             0               //可以配置为 EXSPI_NOT_SUPPORT(0) 或 EXSPI_MUSIC 或 EXSPI_REC 或 (EXSPI_MUSIC | EXSPI_REC)

#define SPIFLASH_BAUD                   (5000000)       //SPI波特率5M
#define SPIFLASH_MAP                    SPI1MAP_G3      //SPI CLK/DI/DO IO 选用G2防止与SD UDISK冲突
#define SPIFLASH_CS_IO                  IO_PA6          //SPI CS IO
#define SPI_4LINE_EN                    0               //是否使用4线SPI. CLK/DI/DO/CS

#if (EX_SPIFLASH_SUPPORT & EXSPI_MUSIC)
//FLASH_MUSIC.BIN 镜像文件占用区域(BYTE)
#define SPIFLASH_MUSIC_BEGIN_ADDR       0               //FLASH_MUSIC.BIN镜像文件默认从0地址开始存放,此宏暂不支持修改.
#define SPIFLASH_MUSIC_END_ADDR         (1024*36)       //FLASH_MUSIC.BIN镜像文件结束地址. 测试DEMO的镜像文件是36K大小.
#define SPIFLASH_MUSIC_BIN_WRITE_TEST   0               //默认的FLASH_MUSIC.BIN写入SPIFLASH, 可以在func_exspiflash_music中测试外接SPIFLASH播放MP3.

#define EXSPI_WAV_EN                    1               //是否支持exspiflash  wav音频播放
#define EXSPI_MP3_EN                    1               //是否支持exspiflash  mp3音频播放
#define EXSPI_LSBC_EN                   1               //是否支持exspiflash lsbc音频播放
#define EXSPI_ESBC_EN                   0               //是否支持exspiflash esbc音频播放
#define EXSPI_MIDI_EN                   1               //是否支持exspiflash midi音频播放
#endif

#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
//录音占用区域(BYTE)  //注意SPIFLASH的录音区域不要覆盖 FLASH_MUSIC.BIN区域
#if REC_INTERNAL_FLASH
#define SPIFLASH_REC_BEGIN_ADDR         0x6F000         //录音起始地址
#define SPIFLASH_REC_END_ADDR           0x80000         //录音结束地址
#else
#define SPIFLASH_REC_BEGIN_ADDR         (1024*37)       //录音起始地址
#define SPIFLASH_REC_END_ADDR           (1024*1024)     //录音结束地址
#endif
#endif

#define SPIFLASH_MUSIC_PLAY_REC         0               // 1 FUNC_EXSPIFLASH_MUSIC 模式下播放录音示例： 支持录音上下曲，及删除当前播放的录音  //0 播放镜像音乐示例
#define SPIFLASH_MUSIC_BREAK_MEMORY     0               //断点记忆，切出flash模式时记忆断点，再次切入flash播放时从上次记忆的断点开始播放，加入100ms断电记忆一次功能
#define SPIFLASH_MUSIC_BREAK_LOOP       1               //0-打开断点记忆时,对当前音乐循环播放 1-打开断点记忆时,对Flash中的所有歌曲循环播放
#define SPIFLASH_MUSIC_SEAMLESS_LOOP    0               //1-对当前歌曲开启无缝循环播放
#define EX_SPIFLASH_PWROFF              0               //是否在休眠时开启exspiflash低功耗模式(flash支持低功耗模式才开)，默认关闭

//#if EX_SPIFLASH_SUPPORT                 //TEST CONFIG
//#undef GUI_SELECT
//#undef FUNC_REC_EN
//#undef MIC_REC_EN
//#undef REC_AUTO_PLAY
//
//#define GUI_SELECT                      GUI_NO
//#define FUNC_REC_EN                     1
//#define MIC_REC_EN                      1
//#define REC_AUTO_PLAY                   1
//#endif


/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
#define USER_ADKEY                      0           //ADKEY的使用， 0为不使用
#define USER_ADKEY2                     0           //ADKEY2的使用，0为不使用
#define USER_IOKEY                      1           //IOKEY的使用， 0为不使用
#define IOKEY_HIGH_TRIGGER              0           //IOKEY高电平触发，0：低电平触发， 1：高电平触发
#define USER_ADKEY_MUX_LED              0           //是否使用ADKEY与LED复用, 共用USER_ADKEY的流程(ADKEY与BLED配置同一IO)
#define ADKEY_PU10K_EN                  0           //ADKEY是否使用内部10K上拉, 按键数量及阻值见port_key.c

#define USER_KEY_KNOB_EN                0           //旋钮的使用，0为不使用
#define USER_KEY_KNOB_LEVEL             16          //旋钮的级数

#define USER_MULTI_PRESS_EN             0           //按键N击检测使能
#define USER_KEY_DOUBLE                 {KU_REC, KU_PLAY_POWER, KU_PLAY}        //支持双击/三击/四击/五击的按键
#define ADKEY_CH                        xcfg_cb.adkey_ch    //ADCCH_PA0
#define ADKEY2_CH                       xcfg_cb.adkey2_ch   //ADCCH_PA1

#define IS_PWRKEY_PRESS()			    0
#define DOUBLE_KEY_TIME                 (1)                       //按键双击响应时间（单位50ms）
#define PWRON_PRESS_TIME                (500*xcfg_cb.pwron_press_time)                  //长按PWRKEY多长时间开机？
#define PWROFF_PRESS_TIME               (3+3*xcfg_cb.pwroff_press_time)                 //长按PWRKEY多长时间关机？

#define WKU_IO_PWRON_EN                 0                               //使能固定的IO唤醒(此唤醒IO之间互不影响,仅支持PA7/PB0/PB1/PB2/PB9/PA0)
#define WKU_IO_FALL_NUM                 3                               //固定唤醒的下降沿IO个数(0-即不使用)
#define WKU_IO_FALL_GROUP               {WKU_PA7, WKU_PB0, WKU_PB1}     //固定唤醒的下降沿IO
#define WKU_IO_RISE_NUM                 2                               //固定唤醒的上升沿IO个数(0-即不使用)
#define WKU_IO_RISE_GROUP               {WKU_PB2, WKU_PA0}              //固定唤醒的上升沿IO

#define MUL_PWRON_IO_EN                 0           //demo功能:使能多个IO唤醒,IO能唤醒和触发KEY_NUM,仅支持触发短按
#define MUL_PWRON_FALL_NUM              9           //下降沿唤醒IO个数,可设置为0,即不使用
#define MUL_PWRON_FALL_GROUP            {IO_PA0, IO_PA1, IO_PA2, IO_PA3, IO_PA4, IO_PA5, IO_PA6, IO_PA7, IO_PA8}
#define MUL_PWRON_RISE_NUM              9           //上升沿唤醒IO个数,可设置为0,即不使用
#define MUL_PWRON_RISE_GROUP            {IO_PB0, IO_PB1, IO_PB2, IO_PB3, IO_PB4, IO_PB5, IO_PB6, IO_PB7, IO_PB8}

#define MIX_PWR_DOWN_EN                 0           //demo功能:sfunc_lowbat->sfunc_pwroff,同时使用两模式
#define MIX_PWR_SLEEP_IO                IO_PA4      //休眠IO
#define MIX_PWR_SLEEP_UP_IO             IO_PA12     //休眠唤醒IO
#define MIX_PWR_DOWN_IO                 IO_PA6      //关机IO
#define MIX_PWR_DOWN_UP_IO              IO_PA12     //关机唤醒IO

/*****************************************************************************
 * Module    : TouchKey按键配置
 *****************************************************************************/
#define TKEY_MUL_SCAN_EN                0                                     //启用Touchkey按键 (需要开按键N击检测使能Tkey才有多击功能)
#define TKEY_ARRAY                      {IO_PA4,IO_PA5,IO_PA7}                                //例如开启两个通道则数组写成{IO_PA4, IO_PA5}, 最多支持22通道
#define TKEY_TKCD_PR                    50                                    //触控采集的周期，一般不需要修改

#define TKEY_PRESS_UPDATE               0                                     //触控按压值动态更新，默认时间为3000ms
#define TKEY_PRESS_UPDATE_TIME          3000								  //更新时间长度

#define TKEY_TKCNT_ADJ_EN               1                                     //是否启用自动调整Touchkey按键键值
#define TKEY_TKCNT_ADJ_TIMES            200                                   //TKEY_TKCNT采集次数
#define TKEY_TKCNT_ADJ_LEVEL            20                                    //1 ~ 100 touchkey按键灵敏度，越低越灵敏，注意误触发

#define TKEY_FILTER_EN                  1                                     //触控软件算术平均值滤波
#define TKEY_FILTER_TIMES               10                                    //连续取N个采样值进行算术平均

/*****************************************************************************
 * Module    : 矩阵按键配置
 *****************************************************************************/
#define MATRIX_XY_KEY_EN                0           //X*Y矩阵按键使能
#define MATRIX_X_PORT_GROUP             {ADCCH_PA0, ADCCH_PA1, ADCCH_PA2, ADCCH_PA3, ADCCH_PA4, ADCCH_PA5}  //X轴需要是ADC检测脚
#define MATRIX_Y_PORT_GROUP             {IO_PB0, IO_PB1, IO_PB2, IO_PB3, IO_PB4, IO_PB5}                    //Y轴普通IO口
#define MATRIX_XY_KEY_SCAN_SEL          0           //0-中断5ms扫描按键,1-中断1ms扫描按键(当按键多的时候可提高灵敏度,但占用算力)

#define MAXTRIX_TRIGLE_KEYBOARD_EN      0           //44按键矩阵键盘使能
#define MAX_KEY_0                       IO_PB0
#define MAX_KEY_1                       IO_PB1
#define MAX_KEY_2                       IO_PB2
#define MAX_KEY_3                       IO_PB3
#define MAX_KEY_4                       IO_PB4
#define MAX_KEY_5                       IO_PB5
#define MAX_KEY_6                       IO_PB6
#define MAX_KEY_7                       IO_PB7

/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SD_SOFT_DETECT_EN               1           //是否使用软件检测（SD发命令检测）
#define SD0_MAPPING                     SD0MAP_G1   //选择SD0 mapping
#define SD0_DET_GPIO_SEL                IO_MUX_SDCLK//复用检测时可以选IO_MUX_SDCLK
#define SD_PlayMusic_Path_EN            0           //播放SD卡中指定的音频文件

///通过配置工具选择检测GPIO
#define SD_DETECT_INIT()                sdcard_detect_init()
#define SD_IS_ONLINE()                  sdcard_is_online()
#define IS_DET_SD_BUSY()                is_det_sdcard_busy()

#define SD1_DETECT_INIT()
#define SD1_IS_ONLINE()                 (0)
#define IS_DET_SD1_BUSY()               (0)

/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                 1
#define UDE_SPEAKER_EN                 1
#define UDE_HID_EN                     1
#define UDE_MIC_EN                     1
#define UDE_MIDI_EN                    0

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能
#define I2C_HW_EN_TEST                  0           //测试硬件I2C
#define I2C_MAPPING                     I2CMAP_PA1PA2 //选择I2C mapping

#define I2C_SW_EN                       0           //是否使能软件I2C功能
#define I2C_SW_EN_TEST                  0           //测试软件I2C
#define I2C_MUX_SD_EN                   0           //是否I2C复用SD卡的IO

#if I2C_MUX_SD_EN
#define I2C_SCL_IN()                    SD_CMD_DIR_IN()
#define I2C_SCL_OUT()                   SD_CMD_DIR_OUT()
#define I2C_SCL_H()                     SD_CMD_OUT_H()
#define I2C_SCL_L()                     SD_CMD_OUT_L()

#define I2C_SDA_IN()                    SD_DAT_DIR_IN()
#define I2C_SDA_OUT()                   SD_DAT_DIR_OUT()
#define I2C_SDA_H()                     SD_DAT_OUT_H()
#define I2C_SDA_L()                     SD_DAT_OUT_L()
#define I2C_SDA_IS_H()                  SD_DAT_STA()
#else
#define I2C_SCL_IN()                    {GPIOADIR |= BIT(1); GPIOAPU   |= BIT(1);}
#define I2C_SCL_OUT()                   {GPIOADE  |= BIT(1); GPIOADIR &= ~BIT(1);}
#define I2C_SCL_H()                     {GPIOASET  = BIT(1);}
#define I2C_SCL_L()                     {GPIOACLR  = BIT(1);}

#define I2C_SDA_IN()                    {GPIOADIR |= BIT(2); GPIOAPU   |= BIT(2);}
#define I2C_SDA_OUT()                   {GPIOADE  |= BIT(2); GPIOADIR &= ~BIT(2);}
#define I2C_SDA_H()                     {GPIOASET  = BIT(2);}
#define I2C_SDA_L()                     {GPIOACLR  = BIT(2);}
#define I2C_SDA_IS_H()                  (GPIOA & BIT(2))
#endif //I2C_MUX_SD_EN

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}
#define I2C_IO_INIT()                   {I2C_SDA_SCL_OUT(); I2C_SDA_H();}

/*****************************************************************************
 * Module    :SPI1配置
 *****************************************************************************/
#define SPI1_AUDIO_EN                   0           //SPI1音频发送接收总使能位
#define SPI_SLAVE_OR_MASTER_EN          0           //0-SPI从机, 1-SPI主机

#if SPI1_AUDIO_EN
#define SPI_AUDIO_TEST_EN               1           //SPI1提示音收发测试
#define     SPI_TXBUF_SIZE              576
#define     SPI_RX_MAX                  2
#define     SPI_RXBUF_SIZE              576
#endif

/*****************************************************************************
 * Module    : ST7735LCD配置
 *****************************************************************************/
#define LCD_DISPLAY_EN                  0           //LCD屏幕总使能
#define LCD_TEST_EN                     0           //LCD测试使能

#if LCD_DISPLAY_EN
#undef  SPIFLASH_BAUD
#undef  SPIFLASH_MAP
#define SPIFLASH_MAP                    SPI1MAP_G3  //SPI CLK/DI/DO IO
#define LCD_RST_IO                      IO_PA11     //LCD RST IO
#define LCD_CS_IO                       IO_PA8      //LCD CS IO
#define LCD_RS_IO                       IO_PA12     //LCD RS IO
#define SPIFLASH_BAUD                   (20000000)  //SPI波特率
#endif

/*****************************************************************************
 * Module    : IRRX配置
 *****************************************************************************/
#define IRRX_SW_EN                      0           //是否打开timer capture ir
#define IR_NUMKEY_EN                    0           //是否打开数字键输入
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#define IR_FILTER_EN                    1           //是否使能红外滤波

//可以打开一个或多个
#define IR_ADDR_FF00_EN                 0
#define IR_ADDR_BF00_EN                 0
#define IR_ADDR_FD02_EN                 0
#define IR_ADDR_FE01_EN                 0
#define IR_ADDR_7F80_EN                 0

#define IR_CAPTURE_PORT()               {GPIOADE |= BIT(1); GPIOAPU  |= BIT(1); GPIOADIR |= BIT(1);}
#define IRRX_MAPPING                    IO_PA1


/*****************************************************************************
 * Module    :huart配置
 *****************************************************************************/
#define HUART_EN                        0                   //是否使能huart
#define HUART_TX_MAP                    HUART_TR_PA12       //Huart tx map
#define HUART_RX_MAP                    HUART_TR_PA12       //Huart rx map
#define HUART_CROSSBAR_IO               IO_PA15             //crossbar的huart仅支持单工模式(G8, G9)

#define HUART_EQ_DBG_EN                 1                   //huart在线调EQ
#define HUART_DUMP_EN                   0                   //huart dump数据
#define HUART_DUMP_SPK_EN               1                   //huart speaker模式dump mic adc数据
#define HUART_DUMP_AUX_EN               1                   //huart AUX模式dump aux adc数据

#define HUART_EXT_DUMP_EN               0                   //Huart ext dump en
/*****************************************************************************
 * Module    : UART0/1配置
 *****************************************************************************/
#define USER_UART0_EN                   0                   //是否开启uart0(开启后需要设UART0_PRINTF_SEL为PRINTF_NONE)
#define UART0_TX_PORT                   UTX0MAP_PA6
#define UART0_RX_PORT                   URX0MAP_PA7
#define UART0_DUPLEX_SEL                0                   //0-半双工模式，1-全双工模式
#define UART0_TX_TEST_EN                0                   //uart0 tx发送测试
#define UART0_RX_TEST_EN                0                   //uart0 rx接收打印
#define UART_CROSSBAR                   0                   //crossbar的uart仅支持单工模式

#if UART_CROSSBAR
#undef  UART0_TX_PORT
#undef  UART0_RX_PORT

#define UART_TX_OR_RX                   0                   //0-TX 1-RX
#define UART_CROSSBAR_IO                IO_PB6              //uart crossbar only support oneline mode
#endif


/*****************************************************************************
 * Module    : PWM配置
 *****************************************************************************/
#define PWM_HW_EN                       0                                   //是否启用硬件PWM输出功能，详细配置需要在pwm_hw_cfg_init内配置
#define PWM_HW_MAP_IO_EN                0                                   //是否映射到其他IO，1：映射  0：不映射，使用默认IO（PA4,PA5,PA8,PA9）; 若映射需要占用crossbar通道
#define PWM_HW_MAP_SEL                  {IO_PA7,IO_PA14,IO_PA15,IO_PB4}     //映射IO，若不映射（不使能通道）则选择IO_NONE
#define PWM_HW_1KHZ_TEST_EN             0                                   //硬件PWM输出1K hz测试

#define PWM_TMR2_EN                     0                                   //是否启用TIMER2 PWM输出功能，详细配置需要在tmr2pwm_cfg_init内配置
#define PWM_TMR2_MAP_IO_EN              0                                   //是否映射到其他IO，1：映射  0：不映射，使用默认IO（PA2,PA3）; 若映射需要占用crossbar通道
#define PWM_TMR2_MAP_SEL                {IO_PA14,IO_PA15}                   //映射IO，若不映射（不使能通道）则选择IO_NONE
#define PWM_TMR2_1KHZ_TEST_EN           1                                   //TMR2PWM输出1K hz测试


/*****************************************************************************
 * Module    : 系统细节配置
 *****************************************************************************/
#define RGB_SERIAL_EN                   0           //串行RGB推灯功能
#define RGB_SPI_PORT                    SPI1MAP_G1  //RGB数据TX口,默认只使能SPIDO口
#define RGB_SPI_CROSSBAR                0           //crossbar的SPI1使能
#define RGB_CROSSBAR_IO                 IO_PB2      //crossbar的SPI1 DO IO任意mapping

#define ENERGY_LED_EN                   0           //能量灯软件PWM显示,声音越大,点亮的灯越多.
#define TMR2_US_EN                      0           //timer2做10us定时器
#define SYS_PARAM_RTCRAM                0           //是否系统参数保存到RTCRAM
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VBAT2_ADCCH                     ADCCH_VBAT  //ADCCH_VBAT为内部1/2电压通路，带升压应用需要外部ADC通路检测1/2电池电压
#define VBAT_FILTER_USE_PEAK            0           //电池检测滤波选则://0 取平均值.//1 取峰值(适用于播放音乐时,电池波动比较大的音箱方案).
#define EQ_MODE_EN                      0           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define MIC_EQ_EN                       1           //是否调节MIC_EQ (6段MIC EQ)
#define EQ_DBG_IN_UART                  0           //是否使能UART在线调节EQ
#define SYS_INIT_VOLUME                 xcfg_cb.sys_init_vol        //系统默认音量
#define LPWR_WARNING_VBAT               xcfg_cb.lpwr_warning_vbat   //低电提醒电压
#define LPWR_OFF_VBAT                   xcfg_cb.lpwr_off_vbat       //低电关机电压
#define LOWPWR_REDUCE_VOL_EN            0                           //低电是否降低音量
#define LPWR_WARING_TIMES               0xff                        //报低电次数
#define LPWR_WARNING_PERIOD             30          //低电播报周期(单位：秒)
#define FREQ_DET_EN                     0           //是否开启频率检测
#define VDDIO_FOLLOW_VBAT_EN            0           //是否开启vddio跟随vbat,[reset_sel, vddio_sel]
#define LVD_IRQ_EN                      0           //是否开启LVD低电压复位或中断
#define LPWR_VBAT_EN                    0           //是否开启低电压电池供电

#define TS_MODE_EN                      0

/*****************************************************************************
 * Module    : LED指示灯配置
 *****************************************************************************/
#define LED_DISP_EN                     0           //是否使用LED指示灯(蓝灯)
#define LED_PWR_EN                      0           //充电及电源指示灯(红灯)
#define LED_LOWBAT_EN                   0           //电池低电是否闪红灯

#define LED_INIT()                      bled_func.port_init(&bled_gpio)
#define LED_SET_ON()                    bled_func.set_on(&bled_gpio)
#define LED_SET_OFF()                   bled_func.set_off(&bled_gpio)

#define LED_PWR_INIT()                  rled_func.port_init(&rled_gpio)
#define LED_PWR_SET_ON()                rled_func.set_on(&rled_gpio)
#define LED_PWR_SET_OFF()               rled_func.set_off(&rled_gpio)


/*****************************************************************************
 * Module    : Loudspeaker mute及耳机检测配置
 *****************************************************************************/
#define EARPHONE_DETECT_EN              0           //是否打开耳机检测
#define SDCMD_MUX_DETECT_EARPHONE       0           //是否复用SDCMD检测耳机插入

///通过配置工具选择检测GPIO
#define EARPHONE_DETECT_INIT()          earphone_detect_init()
#define EARPHONE_IS_ONLINE()            earphone_is_online()
#define IS_DET_EAR_BUSY()               is_detect_earphone_busy()

#define LOUDSPEAKER_MUTE_EN             0           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_UNMUTE_DELAY        6           //UNMUTE延时配置，单位为5ms

#define AMPLIFIER_SEL_INIT()            amp_sel_cfg_init(0)
#define AMPLIFIER_SEL_D()               amp_sel_cfg_d()
#define AMPLIFIER_SEL_AB()              amp_sel_cfg_ab()


/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 0            //是否打开提示音功能, 总开关
#define WARING_MAXVOL_MP3               0            //最大音量提示音WAV或MP3选择， 播放WAV可以与MUSIC叠加播放。
#define WARNING_MP3_PLAY                1            //是否支持MP3提示音播放
#define WARNING_VMP3_PLAY               1            //是否支持VMP3提示音播放
#define WARNING_WAV_PLAY                1            //是否支持WAV提示音播放
#define WARNING_ESBC_PLAY               1            //是否支持esbc提示音播放
#define WARNING_LSBC_PLAY               1            //是否支持esbc提示音播放
#define WARNING_VOLUME                  xcfg_cb.warning_volume   //播放提示音的音量级数
#define LANG_SELECT                     LANG_EN      //提示音语言选择
#define WARNING_PAUSE_PLAY              0            //是否支持提示音暂停播放

#define WARNING_VOICE_CHANGER_PLAY_EN   0            //是否使能变音功能(播放时变音)
#define WARNING_WSOLA_WAV_PLAY_EN       0            //是否使能播放变速功能(wav播放变速)

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               0
#define WARNING_FUNC_MUSIC              0

#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             0
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              0
#define WARNING_MIN_VOLUME              0
#define WARNING_UPDATE_DONE             0

#include "config_extra.h"

#endif // USER_CONFIG_H
