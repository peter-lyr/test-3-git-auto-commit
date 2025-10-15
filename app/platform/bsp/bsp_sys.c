#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_EVT(...)          printf(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_EVT(...)
#endif

sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
xcfg_cb_t xcfg_cb;

bool cfg_spiflash_speed_up_en   = SPIFLASH_SPEED_UP_EN;
bool cfg_spiflash_quard_en      = FLASH_QUAD_READ;
bool cfg_spiflash_2wire_en      = FLASH_2WIRE_EN;

uint32_t cfg_dev_feat = (DEV_FEAT_SDCARD*MUSIC_SDCARD_EN) | (DEV_FEAT_USB*(MUSIC_UDISK_EN|FUNC_USBDEV_EN));

void sd_detect(void);
void tbox_uart_isr(void);
void testbox_init(void);
bool exspiflash_init(void);
void ledseg_6c6s_clr(void);
void bsp_stretch_init(void);
void huart_dump_init(void);
void freqdet_init(void);
bool drc_v3_init(const void *bin, int bin_size);
void ir_rx_110ms_isr(void);

#if TRACE_EN
AT(.com_text.str_sddet)
const char str_sd_insert[] = "->sd insert\n";
AT(.com_text.str_sddet)
const char str_sd_remove[] = "->sd remove\n";
AT(.com_text.str_usbdet)
const char str_udisk_insert[] = "->udisk insert\n";
AT(.com_text.str_usbdet)
const char str_udisk_remove[] = "->udisk remove\n";
AT(.com_text.str_usbdet)
const char str_pc_insert[] = "->pc insert\n";
AT(.com_text.str_usbdet)
const char str_pc_remove[] = "->pc remove\n";
#endif

#if LINEIN_DETECT_EN
AT(.com_text.detect)
void linein_detect(void)
{
    if (IS_DET_LINEIN_BUSY()) {
        return;
    }

    if (LINEIN_IS_ONLINE()) {
        if (dev_online_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_INSERT);
        }
    } else {
        if (dev_offline_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_REMOVE);
        }
    }
}
#endif // LINEIN_DETECT_EN

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_detect(void)
{
    if (IS_DET_SD_BUSY()) {
        return;
    }
    if (SD_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
            TRACE_EVT(str_sd_insert);
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
            TRACE_EVT(str_sd_remove);
        }
    }
}
#endif // MUSIC_SDCARD_EN

#if USB_SUPPORT_EN
AT(.com_text.detect)
void usb_detect(void)
{
#if FUNC_USBDEV_EN
    u8 usb_sta = usbchk_connect(USBCHK_OTG);
#else
    u8 usb_sta = usbchk_connect(USBCHK_ONLY_HOST);
#endif

    if (usb_sta == USB_UDISK_CONNECTED) {
        if (dev_online_filter(DEV_UDISK)) {
            udisk_insert();
            msg_enqueue(EVT_UDISK_INSERT);
            TRACE_EVT(str_udisk_insert);
        }
    } else {
        if (dev_offline_filter(DEV_UDISK)) {
            udisk_remove();
            msg_enqueue(EVT_UDISK_REMOVE);
            TRACE_EVT(str_udisk_remove);
        }
    }
#if FUNC_USBDEV_EN
    if (usb_sta == USB_PC_CONNECTED) {
        if (dev_online_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_INSERT);
            TRACE_EVT(str_pc_insert);
        }
    } else {
        if (dev_offline_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_REMOVE);
            pc_remove();
            TRACE_EVT(str_pc_remove);
        }
    }
#endif
}
#endif // USB_SUPPORT_EN

//timer thread(5ms)
AT(.com_text.timer)
void usr_tmr5ms_isr(void)
{
    sys_cb.tmr5ms_cnt++;

    bsp_saradc_process(1);
    dac_fade_process();

#if !USER_KEY_KNOB2_EN && !MATRIX_XY_KEY_SCAN_SEL
    bsp_key_scan();
#endif

#if LINEIN_DETECT_EN
    linein_detect();
#endif

#if MUSIC_SDCARD_EN
    sd_detect();
#endif

#if MUSIC_SDCARD1_EN
    sd1_detect();
#endif

#if USB_SUPPORT_EN
    usb_detect();
#endif

    //20ms timer process
    if ((sys_cb.tmr5ms_cnt % 4) == 0) {
        dac_dnr_detect();
    }

#if LED_DISP_EN
    if ((sys_cb.tmr5ms_cnt % 10) == 0) {
        led_scan();
    }
#endif // LED_DISP_EN

    //100ms timer process
    if ((sys_cb.tmr5ms_cnt % 20) == 0) {
#if UDE_HID_EN
        if (func_cb.sta == FUNC_USBDEV) {
            ude_tmr_isr();
        }
 #endif // UDE_HID_EN

        if (sys_cb.lpwr_cnt > 0) {
            sys_cb.lpwr_cnt++;
        }
    }

#if TKEY_SCAN_SWITCH_EN
    tkey_cir_scan_en();
#endif

    //500ms timer process
    if ((sys_cb.tmr5ms_cnt % 100) == 0) {
        sys_cb.cm_times++;
#if FUNC_CLOCK_EN
        msg_enqueue(MSG_SYS_500MS);
#endif // FUNC_CLOCK_EN
    }

    //1s timer process
    if ((sys_cb.tmr5ms_cnt % 200) == 0) {
        msg_enqueue(MSG_SYS_1S);
        sys_cb.tmr5ms_cnt = 0;
        sys_cb.lpwr_warning_cnt++;
    }
}

//timer tick interrupt(1ms)
AT(.com_text.timer)
void usr_tmr1ms_isr(void)
{
#if (GUI_SELECT & DISPLAY_LCDSEG)
    gui_scan();                     //按COM扫描时，1ms间隔
#endif

#if !USER_KEY_KNOB2_EN && MATRIX_XY_KEY_SCAN_SEL
    bsp_key_scan();
#endif
}

AT(.text.bsp.sys)
void bsp_clr_mute_sta(void)
{
    if (sys_cb.mute) {
        sys_cb.mute = 0;
// #if DAC_DNR_EN
//         dac_dnr_set_sta(sys_cb.dnr_sta);
// #endif
    }
}

AT(.text.bsp.sys)
bool bsp_get_mute_sta(void)
{
    return sys_cb.mute;
}

void sys_ram_info_dump(void)
{
    extern u32 __comm_size;
    extern u32 __bss_size;

    extern u32 __data_vma;
    extern u32 __data_ram_size;

    extern u32 __comm_vma;
    extern u32 __comm_ram_size;

    u32 comm_free  = (u32)&__comm_ram_size - (u32)&__comm_size;
    u32 data_free = (u32)&__data_ram_size - (u32)&__bss_size;
    u32 ram_total = comm_free + data_free;

    printf("ram_info:\n");
    printf("data:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", data_free,  (u32)&__data_ram_size,  (u32)&__data_vma, (u32)&__data_vma + (u32)&__data_ram_size);
    printf("comm:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", comm_free, (u32)&__comm_ram_size, (u32)&__comm_vma, (u32)&__comm_vma + (u32)&__comm_ram_size);
    printf("remain_total: %dK\n",ram_total / 1024);
}

AT(.text.bsp.sys.init)
static void rtc_32k_configure(void)
{
    u32 temp = RTCCON0;
#if EXT_32K_EN
    GPIOAFEN    &= ~BIT(5);
    GPIOADE     &= ~BIT(5);
    GPIOAPU     &= ~BIT(5);
    GPIOAPU500K &= ~BIT(5);
    GPIOAFEN    &= ~BIT(6);
    GPIOADE     &= ~BIT(6);
    GPIOAPU     &= ~BIT(6);
    GPIOAPU500K &= ~BIT(6);
    temp |= BIT(1);                             //enable 32.768k osc
    temp |= BIT(6);                             //enable ext 32k
    temp = (temp & ~(0x3 << 14)) | (3 << 14);   //32k osc drvier sel
    temp |=  BIT(2);                            //RTC output clock to core enable
#else
    temp |=  BIT(0);                            //RC enable
    temp &= ~BIT(6);                            //use RTC internal 32K osc
    temp |=  BIT(2);                            //RTC output clock to core enable
    temp = (temp & ~(0x3 << 8)) | BIT(8);       //CLK2M in RTC power domain source select RC2M_RTC
#endif
    RTCCON0 = temp;
}

AT(.text.bsp.sys.init)
bool rtc_init(void)
{
    u32 temp;
    rtc_32k_configure();
    sys_cb.rtc_first_pwron = 0;
    temp = RTCCON0;
    if (temp & BIT(7)) {
        temp &= ~BIT(7);                        //clear first poweron flag
        RTCCON0 = temp;
        sys_cb.rtc_first_pwron = 1;
#if FUNC_CLOCK_EN
        rtc_clock_init();
#endif // FUNC_CLOCK_EN
        printf("rtc 1st pwrup\n");
        return false;
    }
    return true;
}

//UART0打印信息输出GPIO选择，UART0默认G1(PA7)
void uart0_mapping_sel(void)
{
    //等待uart0发送完成
    if(UART0CON & BIT(0)) {
        while (!(UART0CON & BIT(8)));
    }
    GPIOBDE  &= ~BIT(3);
    GPIOBFEN &= ~BIT(3);
    GPIOBPU  &= ~BIT(3);
    FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping

#if (UART0_PRINTF_SEL == PRINTF_PA0)
    GPIOADE  |= BIT(0);
    GPIOAPU  |= BIT(0);
    GPIOADIR |= BIT(0);
    GPIOAFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA0;
#elif (UART0_PRINTF_SEL == PRINTF_PA4)
    GPIOADE  |= BIT(4);
    GPIOAPU  |= BIT(4);
    GPIOADIR |= BIT(4);
    GPIOAFEN |= BIT(4);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA4;
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    GPIOBDE  |= BIT(3);
    GPIOBPU  |= BIT(3);
    GPIOBDIR |= BIT(3);
    GPIOBFEN |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB3;
#elif (UART0_PRINTF_SEL == PRINTF_PA9)
    GPIOADE  |= BIT(9);
    GPIOAPU  |= BIT(9);
    GPIOADIR |= BIT(9);
    GPIOAFEN |= BIT(9);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA9;
#endif
}

AT(.rodata.vol)
const u8 maxvol_tbl[4] = {16, 32, 50};

//开user timer前初始化的内容
AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
    memset(&sys_cb, 0, sizeof(sys_cb));
    sys_cb.ms_ticks = tick_get();
    sys_cb.vol_max = maxvol_tbl[xcfg_cb.vol_max];
    if (SYS_INIT_VOLUME > sys_cb.vol_max) {
        SYS_INIT_VOLUME = sys_cb.vol_max;
    }
    if (WARNING_VOLUME > sys_cb.vol_max) {
        WARNING_VOLUME = sys_cb.vol_max;
    }

    // key_var_init();
    plugin_var_init();

    msg_queue_init();

    dac_cb_init((DAC_CLASSD_SPEED << 9) | (DAC_CLASSD_SW_EN << 8) | (DAC_DRV_SEL << 4) | (DAC_CLASSD_MODE << 2) | (DAC_CLASSD_EN << 1) | DAC_EN);

    dev_init();

    saradc_var_init();
    sdadc_var_init();
    mic_bias_var_init();
    sdadc_dc_var_init();

    port_var_init();

#if (MUSIC_UDISK_EN || MUSIC_SDCARD_EN || MUSIC_SDCARD1_EN)
    fs_var_init();
#endif

#if FUNC_AUX_EN
    memset(&f_aux, 0, sizeof(func_aux_t));
#endif

#if FUNC_REC_EN
    bsp_record_var_init();
#endif
}

AT(.text.bsp.sys.init)
static void bsp_io_init(void)
{
#if FLASH_QUAD_READ
    GPIOADE = BIT(6) | BIT(5);          //四线用的PA6, PA5连接HOLD, WP
#else
    GPIOADE = 0;
#endif
	GPIOBDE = 0;
    GPIOGDE = 0x3F;             //MCP FLASH
    uart0_mapping_sel();        //调试UART IO选择或关闭
    mclr_reset_init();

#if MUSIC_SDCARD_EN
    SD_DETECT_INIT();
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
    SD1_DETECT_INIT();
#endif // MUSIC_SDCARD1_EN
}

AT(.text.bsp.sys.init)
void bsp_update_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();

    // power init
    pmu_init(0);

    // clock init
    adpll_init(SYS_CLK_SEL);
    sys_clk_set(SYS_CLK_SEL);

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);
    plugin_init();

    led_init();
    key_init();

    /// enable user timer for display & dac
    sys_set_tmr_enable(1, 1);
#if DAC_EN
    dac_init();
#endif

#if DAC_DRC_EN
    drc_v3_init((u8 *)RES_BUF_DRC_DAC_MUSIC_DRC, RES_LEN_DRC_DAC_MUSIC_DRC);
#endif

#if WARNING_UPDATE_DONE
    mp3_res_play(RES_BUF_UPDATE_DONE_MP3, RES_LEN_UPDATE_DONE_MP3);
#endif
}

AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();

    // power init
    pmu_init(0);

    // clock init
    adpll_init(SYS_CLK_SEL);
    sys_clk_set(SYS_CLK_SEL);
    //dbg_clk_out(9, 6);

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);
    plugin_init();

#if IRRX_SW_EN
    irrx_sw_init();
#endif // IRRX_SW_EN

    led_init();
    key_init();

    /// enable user timer for display & dac
    sys_set_tmr_enable(1, 1);

#if LED_DISP_EN
    led_power_up();
#endif // LED_DISP_EN

#if (GUI_SELECT != GUI_NO)
    gui_init();
#endif

#if DAC_EN
    dac_init();
#endif

    bsp_change_volume(sys_cb.vol);

#if DAC_DRC_EN
    drc_v3_init((u8 *)RES_BUF_DRC_DAC_MUSIC_DRC, RES_LEN_DRC_DAC_MUSIC_DRC);
#endif

#if EX_SPIFLASH_SUPPORT
    exspiflash_init();
#endif

#if SPI_DUMP_EN
    my_spi_init();
#endif

#if HUART_EQ_DBG_EN
    eq_dbg_init();
#endif

#if HUART_EN
    bsp_huart_init();

#if HUART_DUMP_EN
    huart_dump_init();
#endif
#endif

#if HUART_EXT_DUMP_EN
    huart_ext_dump_init();
#endif

#if USER_UART0_EN
    bsp_uart_init();
#endif

#if MIDI_UART0_EN
    midi_uart_init();
#endif

#if TKEY_MUL_SCAN_EN
    tkey_init();
#endif // TKEY_MUL_SCAN_EN

#if SPI_AUDIO_TEST_EN
    spi_test_init();
#endif

#if WARNING_TONE_EN || MIDI_DEC_BK_EN
    music_res_init();
#endif

#if SPK_MIC_VOICE_CHANGER_EN || WARNING_VOICE_CHANGER_PLAY_EN
    voice_changer_init();
#endif

#if WARNING_POWER_ON
    //mp3_res_play(RES_BUF_EN_POWERON_MP3, RES_LEN_EN_POWERON_MP3);
    //vmp3_res_play(RES_BUF_EN_POWERON_VMP3, RES_LEN_EN_POWERON_VMP3);
    //esbc_res_play(RES_BUF_SBC_POWERON_ESBC, RES_LEN_SBC_POWERON_ESBC);
    lsbc_res_play(RES_BUF_SBC_POWERON_LSBC, RES_LEN_SBC_POWERON_LSBC);
#endif

#if PWM_HW_EN
    pwm_hw_cfg_init();
#endif // PWM_HW_EN

#if PWM_TMR2_EN
    tmr2pwm_cfg_init();
#endif // PWM_TMR2_EN

#if FREQ_DET_EN
    freqdet_init();
#endif // FREQ_DET_EN

#if TMR2_US_EN
    timer2_init();
#endif  //TMR2_US_EN

#if RGB_SERIAL_EN
    spi1_led_init();
#endif

#if LCD_DISPLAY_EN
    lcd_display_init();
#endif

#if MIDI_EN
    midi_init();
#endif

#if MIDI_REC_EN
    midi_rec_init();
#endif

#if SPK_MIC_ECHO_EN
    bsp_echo_init();
#endif

#if SPK_MIC_REVERB_EN
    bsp_reverb_init();
#endif

#if SPK_MIC_WSOLA_REC_EN || WARNING_WSOLA_WAV_PLAY_EN
    bsp_wsola_init();
#endif

#if SPK_MIC_DNR_FRE_EN
    bsp_dnr_fre_init();
#endif

    func_cb.sta = FUNC_IDLE;

#if FUNC_USBDEV_EN
    if (dev_is_online(DEV_USBPC)) {
        func_cb.sta = FUNC_USBDEV;
    }
#endif
}

