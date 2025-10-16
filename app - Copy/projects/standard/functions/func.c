#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);

AT(.text.func.process)
void print_info(void)
{
   static u32 ticks = 0;
   if (tick_check_expire(ticks,1000)) {
       ticks = tick_get();
       my_printf(".");
       //my_printf("DACVOLCON = 0x%X, SRC0VOLCON: 0x%X, SRC1VOLCON: 0x%X\n", DACVOLCON&0xFFFF, SRC0VOLCON&0x7FFF, SRC1VOLCON&0x7FFF);
   }
}

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
    print_info();
#if MUSIC_DECODE_BK_EN
    muisc_decode_process(wav_layer_tbl);
#endif
#if VBAT_DETECT_EN
    bsp_vbat_lpwr_process();
#if VDDIO_FOLLOW_VBAT_EN
    vddio_follow_vbat_process();
#endif
#endif // VBAT_DETECT_EN
#if USER_UART0_EN
    uart_deal_process();
#endif
#if MIDI_UART0_EN
    midi_uart_rx_process();
    midi_uart_tx_process();
#endif
#if MAXTRIX_SQUA_KEYBOARD_EN
    maxtrix_squa_scan_process();
#endif // MAXTRIX_SQUA_KEYBOARD_EN
#if MAXTRIX_TRIGLE_KEYBOARD_EN
    maxtrix_triangle_scan_process();
#endif // MAXTRIX_TRIGLE_KEYBOARD_EN
#if RGB_SERIAL_EN
    spi1_led_data_send_proc();
#endif
#if TKEY_PRESS_UPDATE
    tkey_press_update();
#endif
    gui_display_test();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {
    case KU_PLAY:
        case KL_PLAY:
        case KH_PLAY:
        case KD_PLAY:
#if MIDI_TEST_EN && (!MIDI_OKON_EN) && MIDI_EN
            midi_control(MIDI_STOP);
#endif
#if MIDI_OKON_EN && MIDI_EN
            set_midi_okon_next(1);
#endif
            break;

        case KU_NEXT:
#if MIDI_TEST_EN && MIDI_EN
        #if MIDI_DEC_BK_EN
            #if MIDI_OKON_EN
                midi_song_play(RES_BUF_MID_DEMO_MIDI0_MID, RES_LEN_MID_DEMO_MIDI0_MID);     //标准midi okon播放示例,先播放再set_midi_okon_next
            #else
                vmidi_song_play(RES_BUF_MID_DEMO_MIDI1_VMID, RES_LEN_MID_DEMO_MIDI1_VMID);  //vmidi大循环播放示例
            #endif
        #else
            vmidi_res_play(RES_BUF_MID_DEMO_MIDI1_VMID, RES_LEN_MID_DEMO_MIDI1_VMID);       //vmidi独占播放示例
        #endif
#endif
        case K_NEXT:
#if MIDI_REC_TEST_EN
            midi_rec_stop();
            midi_rec_play();
#endif
            break;

        case KU_PREV:
#if MIDI_TEST_EN && MIDI_EN
        #if MIDI_DEC_BK_EN
            midi_song_play(RES_BUF_MID_DEMO_MIDI0_MID, RES_LEN_MID_DEMO_MIDI0_MID);         //标准midi okon播放示例,先播放再set_midi_okon_next
        #else
            vmidi_res_play(RES_BUF_MID_DEMO_MIDI2_VMID, RES_LEN_MID_DEMO_MIDI2_VMID);
        #endif
#endif
            break;
        case K_PREV:
#if MIDI_REC_TEST_EN
            midi_rec_play_switch();
#endif
            break;

        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
            func_cb.sta = FUNC_NULL;
            break;

        case KU_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
            bsp_set_volume(bsp_volume_inc(sys_cb.vol, 1));
            break;

        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
#if MIDI_REC_TEST_EN
            if(get_midi_rec_state() == REC_STATE_NULL) {
                midi_rec_start();
            }
            else{
                midi_rec_stop();
            }
#endif
            bsp_set_volume(bsp_volume_dec(sys_cb.vol, 1));
            break;

        case KD_VOL_DOWN:
            break;

        case K_EQ:
#if (MIDI_KEYS_TEST || MIDI_REC_TEST_EN)
        mkey_note_onoff(0x96, 20, 100);
#endif
            break;
        case KU_EQ:
#if (MIDI_KEYS_TEST || MIDI_REC_TEST_EN)
        mkey_note_onoff(0x96, 20, 0);
#endif
            break;
#if (MIDI_KEYS_TEST || MIDI_REC_TEST_EN)
        mkey_note_onoff(0x96, 20, 0);
#endif
            break;

#if MIX_PWR_DOWN_EN
        case KLH_PLAY_POWER:
            mix_pwr_down(1);
            break;
        case KLH_MODE_POWER:
            mix_pwr_down(0);
            break;
#else
        case KU_PLAY_POWER:
            // wav_song_play(MSC_LAYER1, RES_BUF_DNOTE_WAV, RES_LEN_DNOTE_WAV);
            break;

        //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_PLAY_POWER:
        case KLH_MODE_POWER:
        case KLH_HSF_POWER:
        case KLH_POWER:
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

#if MAXTRIXKEY_PWRON_IO_EN
//        case KEY_MAXT_1:
//            mix_pwr_down(0);
//            break;
//
//        case KEY_MAXT_2:
//            mix_pwr_down(0);
//            break;
//
//        case KEY_MAXT_3:
//            mix_pwr_down(0);
//            break;
//
//        case KEY_MAXT_4:
//            mix_pwr_down(0);
//            break;
#endif // MAXTRIXKEY_PWRON_IO_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
        case EVT_SD1_INSERT:
            if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD1_EN

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK)) {
                sys_cb.cur_dev = DEV_UDISK;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_UDISK_EN

#if LINEIN_DETECT_EN
        case EVT_LINEIN_INSERT:
            if (dev_is_online(DEV_LINEIN)) {
                func_cb.sta = FUNC_AUX;
            }
            break;
#endif // LINEIN_DETECT_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                func_cb.sta = FUNC_USBDEV;
            }
            break;
#endif // FUNC_USBDEV_EN

#if HUART_EQ_DBG_EN
        case EVT_ONLINE_SET_EQ:
            eq_parse_cmd();
            break;
#endif

#if IRRX_SW_EN || TKEY_SLEEP_MODE
        case KU_IR_POWER:
            func_cb.sta = FUNC_SLEEPMODE;
            break;
#endif

#if MUL_PWRON_IO_EN
        case KEY_NUM_0:
            dac_fade_in();
            dac_fade_out();
            break;
        case KEY_NUM_1:
            dac_fade_in();
            dac_fade_out();
            break;
        case KEY_NUM_2:
        case KEY_NUM_3:
        case KEY_NUM_4:
        case KEY_NUM_5:
        case KEY_NUM_6:
        case KEY_NUM_7:
        case KEY_NUM_8:
        case KEY_NUM_9:
        case KEY_NUM_P100:
            break;
#endif

        case MSG_SYS_1S:
            //printf("DACVOLCON: %x, SRC0VOLCON: %x\n", DACVOLCON, SRC0VOLCON);
#if (DAC_CLASSD_VDET && VBAT_DETECT_EN)
            dac_classd_adaptive_by_vbat(sys_cb.vbat);
#endif
            break;

        default:
            break;

    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        cm_sync();
    }
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
#if (GUI_SELECT != GUI_NO && GUI_SELECT != GUI_LCDSEG)
    gui_box_clear();
#endif
    param_sync();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
//    bsp_clr_mute_sta();
}

AT(.text.func)
void func_exit(void)
{
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任务
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    func_cb.sta = FUNC_MUSIC;
    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if EX_SPIFLASH_SUPPORT
        case FUNC_EXSPIFLASH_MUSIC:
            func_exspiflash_music();
            break;
#endif // EX_SPIFLASH_SUPPORT

#if FUNC_SPEAKER_EN
        case FUNC_SPEAKER:
            func_speaker();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_AUX_EN
        case FUNC_AUX:
            func_aux();
            break;
#endif // FUNC_AUX_EN

#if FUNC_USBDEV_EN
        case FUNC_USBDEV:
            func_usbdev();
            break;
#endif // FUNC_USBDEV_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        case FUNC_SLEEPMODE:
            func_sleepmode();
            break;

        default:
            func_exit();
            break;
        }
    }
}
