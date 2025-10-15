#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if !AUX_2_SDADC_EN
#undef AUX_REC_EN
#define AUX_REC_EN                  0
#endif

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN      0
#endif // SYS_MODE_BREAKPOINT_EN

#ifndef ENERGY_LED_EN
#define ENERGY_LED_EN               0
#endif

#ifndef WARNING_WAV_PLAY
#define WARNING_WAV_PLAY            0
#endif

#if FLASH_2WIRE_EN
#undef  FLASH_DUAL_READ
#undef  FLASH_QUAD_READ
#undef  SPIFLASH_SPEED_UP_EN
#define FLASH_DUAL_READ             0
#define FLASH_QUAD_READ             0
#define SPIFLASH_SPEED_UP_EN        0
#endif // FLASH_2WIRE_EN

#ifndef DAC_DRC_EN
#define DAC_DRC_EN                      0
#endif

/*****************************************************************************
 * Module    : MIDI功能配置
 *****************************************************************************/
#if !MIDI_EN
#undef MIDI_SONG_EN
#define MIDI_SONG_EN        0

#undef MIDI_TEST_EN
#define MIDI_TEST_EN        0

#undef MIDI_OKON_EN
#define MIDI_OKON_EN        0

#undef MIDI_KEYBOARD_EN
#define MIDI_KEYBOARD_EN    0

#undef MIDI_REC_EN
#define MIDI_REC_EN         0

#undef MIDI_REC_TEST_EN
#define MIDI_REC_TEST_EN    0

#undef MIDI_METRO_EN
#define MIDI_METRO_EN       0
#endif

#ifndef MIDI_VOICE_NUM
#define MIDI_VOICE_NUM                  64
#endif

#ifndef MIDI_CHANNEL_NUM
#define MIDI_CHANNEL_NUM                16
#endif

#if (UFLASH_BIN_EN && MIDI_EN)
#error "UFLASH_BIN_EN & MIDI_EN can not open at the same time"
#endif

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN
#undef  MUSIC_SDCARD1_EN
#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_ESBC_SUPPORT
#undef  MUSIC_ID3_TAG_EN
#undef  MUSIC_PLAYMODE_NUM
#undef  MUSIC_BREAKPOINT_EN
#undef  MUSIC_QSKIP_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_SDCARD1_EN            0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_ESBC_SUPPORT          0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_PLAYMODE_NUM          0
#define MUSIC_BREAKPOINT_EN         0
#define MUSIC_QSKIP_EN              0
#endif // FUNC_MUSIC_EN


#if !EARPHONE_DETECT_EN
#undef SDCMD_MUX_DETECT_EARPHONE
#undef EARPHONE_DETECT_INIT
#undef EARPHONE_IS_ONLINE
#undef IS_DET_EAR_BUSY
#define EARPHONE_DETECT_INIT()
#define SDCMD_MUX_DETECT_EARPHONE   0
#define EARPHONE_IS_ONLINE()        0
#define IS_DET_EAR_BUSY()           0
#endif

#if !FUNC_AUX_EN
#undef  LINEIN_DETECT_EN
#undef  REC_AUX_EN
#define LINEIN_DETECT_EN            0
#define REC_AUX_EN                  0
#endif

#if !LINEIN_DETECT_EN
#undef SDCMD_MUX_DETECT_AUX
#undef LINEIN_DETECT_INIT
#undef LINEIN_IS_ONLINE
#undef IS_DET_LINEIN_BUSY

#define LINEIN_DETECT_INIT()
#define SDCMD_MUX_DETECT_AUX     0
#define LINEIN_IS_ONLINE()          0
#define IS_DET_LINEIN_BUSY()        0
#else
    #if (AUX_DET_GPIO_SEL == IO_MUX_SDCLK || AUX_DET_GPIO_SEL == IO_MUX_SDCMD)
    #define SDCMD_MUX_DETECT_AUX    1
    #else
    #define SDCMD_MUX_DETECT_AUX    0
    #endif
#endif

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            1500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef DOUBLE_KEY_TIME
#define DOUBLE_KEY_TIME             1
#endif // DOUBLE_KEY_TIME

#ifndef SYS_LIMIT_VOLUME
#define SYS_LIMIT_VOLUME            SYS_INIT_VOLUME
#endif // SYS_LIMIT_VOLUME

#ifndef LINEIN_2_PWRDOWN_TONE_EN
#define LINEIN_2_PWRDOWN_TONE_EN    0   //插入Linein直接关机，是否播放关机提示音。
#endif // LINEIN_2_PWRDOWN_TONE_EN

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef LED_LOWBAT_EN
#define LED_LOWBAT_EN              0
#endif // RLED_LOWBAT_EN

#ifndef RLED_LOWBAT_FOLLOW_EN
#define RLED_LOWBAT_FOLLOW_EN       0
#endif // RLED_LOWBAT_FOLLOW_EN

#ifndef SLEEP_DAC_OFF_EN
#define SLEEP_DAC_OFF_EN            1
#endif // SLEEP_DAC_OFF_EN

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LED_DISP_EN
#undef LED_LOWBAT_EN
#undef RLED_LOWBAT_FOLLOW_EN
#undef BLED_FFT_EN
#undef BLED_LOW2ON_EN
#undef BLED_CHARGE_FULL_EN

#define LED_LOWBAT_EN              0
#define RLED_LOWBAT_FOLLOW_EN       0
#define BLED_FFT_EN                 0
#define BLED_LOW2ON_EN              0
#define BLED_CHARGE_FULL_EN         0

#endif

#if !LED_DISP_EN
#undef LED_INIT
#define LED_INIT()
#undef LED_SET_ON
#define LED_SET_ON()
#undef LED_SET_OFF
#define LED_SET_OFF()
#endif

#if !LED_PWR_EN
#undef LED_PWR_INIT
#define LED_PWR_INIT()
#undef LED_PWR_SET_ON
#define LED_PWR_SET_ON()
#undef LED_PWR_SET_OFF
#define LED_PWR_SET_OFF()
#endif

#if ((GUI_SELECT & DISPLAY_LCD) == DISPLAY_LCD)
#define GUI_LCD_EN                      1
#else
#define GUI_LCD_EN                      0
#undef  MUSIC_NAVIGATION_EN
#define MUSIC_NAVIGATION_EN             0
#endif

#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#endif

#ifndef IR_INPUT_NUM_MAX
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#endif // IR_INPUT_NUM_MAX

#ifndef FMRX_THRESHOLD_VAL
#define FMRX_THRESHOLD_VAL              128
#endif // FMRX_THRESHOLD_VAL

#ifndef FMRX_SEEK_DISP_CH_EN
#define FMRX_SEEK_DISP_CH_EN            0
#endif // FMRX_SEEK_DISP_CH_EN

#ifndef SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN               0
#undef SD_IS_SOFT_DETECT
#define SD_IS_SOFT_DETECT()             0
#endif // SD_SOFT_DETECT_EN

#if USER_ADKEY_MUX_LED
#undef  USER_ADKEY
#undef  ADKEY_PU10K_EN

#define USER_ADKEY                      1
#define ADKEY_PU10K_EN                  0
#endif // USER_ADKEY_MUX_LED


#if !I2C_SW_EN
#undef  I2C_MUX_SD_EN
#define I2C_MUX_SD_EN               0
#endif

#if ((GUI_SELECT & DISPLAY_LCD) == DISPLAY_LCD)
#define GUI_LCD_EN                      1
#else
#define GUI_LCD_EN                      0
#endif

#ifndef EX_SPIFLASH_SUPPORT
#define EX_SPIFLASH_SUPPORT             0
#endif

#if !FUNC_REC_EN
#undef  REC_SPEAKER_EN
#undef  REC_AUX_EN
#undef  REC_FAST_PLAY
#undef  REC_MP3_SUPPORT
#undef  REC_WAV_SUPPORT
#undef  REC_ADPCM_SUPPORT
#undef  REC_INTERNAL_FLASH

#define  REC_SPEAKER_EN             0
#define  REC_AUX_EN                 0
#define  REC_FAST_PLAY              0
#define  REC_MP3_SUPPORT            0
#define  REC_WAV_SUPPORT            0
#define  REC_ADPCM_SUPPORT          0
#define  REC_INTERNAL_FLASH         0
#endif //FUNC_REC_EN

#if !HUART_EN
#undef HUART_EQ_DBG_EN
#undef HUART_DUMP_EN
#define HUART_EQ_DBG_EN             0
#define HUART_DUMP_EN               0
#endif

#if !HUART_DUMP_EN
#undef HUART_DUMP_SPK_EN
#undef HUART_DUMP_AUX_EN
#define HUART_DUMP_SPK_EN           0
#define HUART_DUMP_AUX_EN           0
#endif

/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PA4)
    #if (SD0_MAPPING == SD0MAP_G3 || SD0_MAPPING == SD0MAP_G4)
    #undef  MUSIC_SDCARD_EN
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD_EN         0
    #define MUSIC_SDCARD1_EN        0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    #undef  FUNC_USBDEV_EN
    #undef  MUSIC_UDISK_EN
    #define FUNC_USBDEV_EN          0
    #define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)

    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SDCLK_MUX_DETECT_SD
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef IS_DET_SD_BUSY
#undef SD0_DET_GPIO_SEL

#define SD_DETECT_INIT()
#define SD0_DET_GPIO_SEL            IO_NONE
#define SDCLK_MUX_DETECT_SD         0
#define SD_IS_ONLINE()              0
#define IS_DET_SD_BUSY()            0
#else
    #if (SD0_DET_GPIO_SEL == IO_MUX_SDCLK || SD0_DET_GPIO_SEL == IO_MUX_SDCMD)
    #define SDCLK_MUX_DETECT_SD     1
    #else
    #define SDCLK_MUX_DETECT_SD     0
    #endif
#endif

#if !MUSIC_SDCARD1_EN
#undef SD1_DETECT_INIT
#undef SD1_IS_ONLINE
#undef IS_DET_SD1_BUSY

#define SD1_DETECT_INIT()
#define SD1_IS_ONLINE()              0
#define IS_DET_SD1_BUSY()            0
#endif


/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN
    #undef  UDE_STORAGE_EN
    #undef  UDE_SPEAKER_EN
    #undef  UDE_HID_EN
    #undef  UDE_MIC_EN
    #undef  UDE_ENUM_TYPE

    #define UDE_STORAGE_EN              0
    #define UDE_SPEAKER_EN              0
    #define UDE_HID_EN                  0
    #define UDE_MIC_EN                  0
    #define UDE_ENUM_TYPE               0
#else
    #define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08 + UDE_MIDI_EN*0x10)
#endif


/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_FUNC_MUSIC
#undef WARNING_FUNC_BT
#undef WARNING_FUNC_CLOCK
#undef WARNING_FUNC_FMRX
#undef WARNING_FUNC_AUX
#undef WARNING_FUNC_USBDEV
#undef WARNING_FUNC_SPEAKER
#undef WARNING_LOW_BATTERY
#undef WARNING_USB_SD
#undef WARNING_MAX_VOLUME
#undef WARNING_UPDATE_DONE

#define WARNING_POWER_ON                0
#define WARNING_POWER_OFF               0
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 0
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             0
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              0
#define WARNING_UPDATE_DONE             0
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN) && (!MUSIC_SDCARD1_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

#define SD_SUPPORT_EN                  (MUSIC_SDCARD_EN | MUSIC_SDCARD1_EN)
#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN)

#if MUSIC_SDCARD_EN
#define SDCMD_MUX_DETECT_EN            1
#define SDCLK_MUX_DETECT_EN            1
#else
#undef  I2C_MUX_SD_EN

#define I2C_MUX_SD_EN                  0
#define SDCMD_MUX_DETECT_EN            0
#define SDCLK_MUX_DETECT_EN            0
#endif // MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#endif

/*****************************************************************************
 * Module    : 音效配置
 *****************************************************************************/
#if VOICE_CHANGER_ROBOT_EN
#undef  VOICE_CHANGER_FS_EN
#undef  FFT_PROC_EN

#define VOICE_CHANGER_FS_EN                 1
#define FFT_PROC_EN                         1
#endif

#if SPK_MIC_DNR_FRE_EN
#undef  FFT_PROC_EN

#define FFT_PROC_EN                         1
#endif
#endif // __CONFIG_EXTRA_H__

