#include "include.h"

extern u32 __pdn_comm_vma, __pdn_comm_lma, __pdn_comm_size;

#if USER_MULTI_PRESS_EN
u8 dblkey_num[5] AT(.buf.key);
const u16 tbl_double_key[] = USER_KEY_DOUBLE;
#endif

#if TKEY_MUL_SCAN_EN
u8 tkey_num[5] AT(.buf.key);
#endif

u32 getcfg_reset_sel(void);
u16 getcfg_vddio_val_ofs(void);
void vddio_voltage_update(u16 vbat_val);
u16 get_cur_vddio_vol(void);
extern bool tkey_finish_flag;

AT(.com_rodata.key.table)
const key_shake_tbl_t key_shake_table = {
    .scan_cnt = KEY_SCAN_TIMES,
    .up_cnt   = KEY_UP_TIMES,
    .long_cnt = KEY_LONG_TIMES,
    .hold_cnt = KEY_LONG_HOLD_TIMES,

    .uid_short_up = KEY_SHORT_UP,
    .uid_long     = KEY_LONG,
    .uid_long_up  = KEY_LONG_UP,
    .uid_hold     = KEY_HOLD,
};


#if (USER_ADKEY)
//AT(.com_text.str)
//const char str_1[] = "key_val:%d\n";
AT(.com_text.port.key)
static u8 get_adkey(u8 key_val, u8 key_config_en)
{
    u8 num = 0;

    //配置工具是否使能
    if (!key_config_en) {
        return NO_KEY;
    }

    //my_printf(str_1,key_val);
    while (key_val > adkey_table[num].adc_val) {
        num++;
    }
    return adkey_table[num].usage_id;
}
#endif // USER_ADKEY

#if USER_ADKEY2
AT(.com_text.port.key)
static u8 get_adkey2(u8 key_val)
{
    u8 num = 0;

    //配置工具是否使能了第二组ADKEY2？
    if (!xcfg_cb.user_adkey2_en) {
        return NO_KEY;
    }

    while (key_val > adkey2_table[num].adc_val) {
        num++;
    }
    return adkey2_table[num].usage_id;
}
#endif // USER_ADKEY2


AT(.text.bsp.power)
bool is_powron_frist_enable(u32 rtccon9)
{
    if (!PWRON_FRIST_BAT_EN) {
        return false;
    }
    if (rtccon9 & BIT(7)) {                                        //GPIO wakeup不能直接开机
        return false;
    }

    return true;
}

AT(.text.bsp.power)
void power_on_check(void)
{
    int up_cnt = 0, ticks = 0;
    int pwrkey_pressed_flag = 0;
    u32 pwron_press_nms;
    bool bo_mulpwr = false;
    u8 key_val = 0;

    u32 rtccon9 = RTCCON9;                          //wakeup pending
    // u32 rtccon10 = RTCCON10;
    // printf("power_on_check: %08x, %08x\n", rtccon9, rtccon10);
    // printf("RTCCON: %X, RTCCON0: %X, RTCCON3: %x, RTCCON9: %x\n", RTCCON, RTCCON0, RTCCON3, RTCCON9);
    // printf("RTCCON10: %X, RTCCON12: %X\n", RTCCON10, RTCCON12);
#if(POWER_DOWN_MODE == PWR_RTC1S)
    if(RTCCON9 & BIT(1)){
        RTCCON3 |= 0X7;
    }
#endif
    RTCCON9  = 0xfff;                               //Clr pending
    RTCCON10 = 0x7fff;                              //Clr pending
    // RTCCPND  = 0xb0060000;                          //Clr RTC pending

    PORTINTEDG  = 0;
    PORTINTEN   = 0;
    WKUPCON &= ~(BIT(6) | BIT(7));
    WKUPCPND =  BIT(22);                            //clear 6 pending
    WKUPCPND =  BIT(23);			                //clear 7 pending
    WKUPCON &= ~BIT(17);                            //wakup sniff enable

    if (!SOFT_POWER_ON_OFF) {
        return;
    }

#if MIX_PWR_DOWN_EN || MAXTRIXKEY_PWRON_IO_EN
    return;
#endif

    if (LVDCON & BIT(19)) {
        return;                                     //长按WKO 10S复位后直接开机。
    }

    pwron_press_nms = PWRON_PRESS_TIME;
    if (pwron_press_nms == 0) {
        pwron_press_nms = 15;                       //最小开机时间在100ms左右
    }

    //第一次上电是否直接开机
    if (is_powron_frist_enable(rtccon9)) {
        return;
    }

#if ((POWER_ON_FALL_NUM != 0) || (POWER_ON_RISE_NUM != 0))
    u16 gpioa_fen, gpiob_fen, gpioa_de, gpiob_de, gpioa_pu, gpiob_pu, gpioa_pd, gpiob_pd;
    gpioa_fen = GPIOAFEN;
    gpioa_de  = GPIOADE;
    gpioa_pu  = GPIOAPU;
    gpioa_pd  = GPIOAPD;
    gpiob_fen = GPIOBFEN;
    gpiob_de  = GPIOBDE;
    gpiob_pu  = GPIOBPU;
    gpiob_pd  = GPIOBPD;
    poweron_key_init();
#endif
    while (1) {
    WDT_CLR();
    delay_ms(5);

#if MUL_PWRON_IO_EN
    u8 mulpwr_press = get_mulpwr_key();
    if(mulpwr_press != 0){//有其中一个IO按下
        bo_mulpwr = true;
    }else{
        bo_mulpwr = false;
    }
#endif

#if ((POWER_ON_FALL_NUM != 0) || (POWER_ON_RISE_NUM != 0))
    key_val = get_poweron_key();
    if(key_val != 0){//有其中一个IO符合触发条件
        key_val = K_PWR;
    }
#endif
        if (((key_val & K_PWR_MASK) == K_PWR) ||  bo_mulpwr) {
            up_cnt = 0;
            if (!pwrkey_pressed_flag) {
                ticks = tick_get();
                sys_cb.ms_ticks = ticks;                                //记录PWRKEY按键按下的时刻
                pwrkey_pressed_flag = 1;
            }
            if (!sys_cb.poweron_flag) {
                if (tick_check_expire(ticks, pwron_press_nms)) {        //长按开机时间配置
                    sys_cb.poweron_flag = 1;
                }
            }
        } else {
            if (up_cnt < 3) {
                up_cnt++;
            }
            if (up_cnt == 3) {
                up_cnt = 10;
                pwrkey_pressed_flag = 0;
                sys_cb.poweron_flag = 0;
            }
        }

        if (sys_cb.poweron_flag) {
#if VBAT_DETECT_EN
            if (sys_cb.vbat <= 2950) {  //电压小于2.95v不开机
                continue;
            }
#endif
            break;
        } else {
            //PWKKEY松开，立刻关机
            if (!pwrkey_pressed_flag) {
                //bsp_saradc_exit();
                memcpy(&__pdn_comm_vma, &__pdn_comm_lma, (u32)&__pdn_comm_size);//复用aram
                sfunc_pwroff();
            }
        }
#if MUL_PWRON_IO_EN
        msg_enqueue(mulpwr_press);
#endif
    }
#if ((POWER_ON_FALL_NUM != 0) || (POWER_ON_RISE_NUM != 0))
    GPIOAFEN = gpioa_fen;
    GPIOADE  = gpioa_de;
    GPIOAPU  = gpioa_pu;
    GPIOAPD  = gpioa_pd;
    GPIOBFEN = gpiob_fen;
    GPIOBDE  = gpiob_de;
    GPIOBPU  = gpiob_pu;
    GPIOBPD  = gpiob_pd;
    poweron_key_exit();
#endif
}

AT(.text.bsp.key.init)
void key_init(void)
{
    key_var_init();

#if USER_IOKEY
    io_key_init();
#endif

#if USER_MULTI_PRESS_EN
    if (xcfg_cb.key_multi_config_en) {
        dblkey_num[0] = key_config_table[xcfg_cb.dblkey_num0];
        dblkey_num[1] = key_config_table[xcfg_cb.dblkey_num1];
        dblkey_num[2] = key_config_table[xcfg_cb.dblkey_num2];
        dblkey_num[3] = key_config_table[xcfg_cb.dblkey_num3];
        dblkey_num[4] = key_config_table[xcfg_cb.dblkey_num4];
    }
#endif

#if USER_ADKEY
    if (xcfg_cb.user_adkey1_en) {
        saradc_set_channel(BIT(ADKEY_CH));
    #if ADKEY_PU10K_EN
        adcch_io_pu10k_enable(ADKEY_CH);        //开内部10K上拉
    #endif // ADKEY_PU10K_EN
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    saradc_set_channel(BIT(ADKEY2_CH));

#endif // USER_ADKEY2

	saradc_set_channel(BIT(ADCCH_BGOP));
#if VBAT_DETECT_EN
    saradc_set_channel(BIT(VBAT2_ADCCH));
#endif // VBAT_DETECT_EN

#if MATRIX_XY_KEY_EN
    matrix_xy_key_init();
#endif

    bsp_saradc_init();

#if MAXTRIX_TRIGLE_KEYBOARD_EN
    maxtrix_triangle_key_init();
#endif // MAXTRIX_TRIGLE_KEYBOARD_EN

#if MAXTRIX_SQUA_KEYBOARD_EN
    maxtrix_squa_key_init();
#endif // MAXTRIX_SQUA_KEYBOARD_EN

#if MUL_PWRON_IO_EN
    mulpwr_key_init();
#endif

    power_on_check();
    CRSTPND = 0x1ff0000;          //clear reset pending
}

#if USER_MULTI_PRESS_EN
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    u8 i;
    if (key_return == NO_KEY) {
        return false;
    }

    //工具配置了哪些按键支持双击功能？
    i = check_key_double_configure(key_return);
    if (i > 0) {
        return (i - 1);
    }

    for (i=0; i<sizeof(tbl_double_key)/2; i++) {
        if (key_return == tbl_double_key[i]) {
            return true;
        }
    }
    return false;
}
#else
bool check_key_return(u16 key_return)
{
    return false;
}
#endif // USER_KEY_DOUBLE_EN

/*
AT(.com_text.bsp.key)
void pwrkey10s_counter_clr(void)
{
//    if (MCLR_L_RESET) {
//        RTCCON10 = BIT(10);                 //clear pwrkey10s pending and counter
//    }
}
*/

AT(.text.bsp.key)
u8 get_pwroff_pressed_time(void)
{
    return PWROFF_PRESS_TIME;
}

AT(.text.bsp.key)
u8 get_double_key_time(void)
{
    if(DOUBLE_KEY_TIME > 7) {
        return 60;
    } else {
        return (u8)((u8)(DOUBLE_KEY_TIME + 2) * 20 + 1);
    }
}

AT(.com_text.bsp.key)
bool get_poweron_flag(void)
{
    return sys_cb.poweron_flag;
}

AT(.com_text.bsp.key)
void set_poweron_flag(bool flag)
{
    sys_cb.poweron_flag = flag;
}

#if USER_KEY_KNOB_EN
AT(.com_text.bsp.key)
void key_knob_process(u16 adc_val, const u8 *knob_level, u8 *key_val)
{
    u8 step = 0;
    if (s_abs((int)knob_level[*key_val] - (int)adc_val) > 4) {    //取绝对值消抖
        while (adc_val > knob_level[step]) {
            step++;
        }
        *key_val = step;
    }
}
#endif

#if USER_KEY_KNOB2_EN
AT(.com_text.bsp.key)
u16 key_knob_process2(u16 *key_val)
{
    static u8 key_flag = 0;
    static u8 key_cnt_0 = 0, key_cnt_1 = 0;
//    my_printf(test_printf,*key_val);
    if (*key_val != KEY_VOL_DOWN_PREV && *key_val != KEY_VOL_UP_NEXT) {     //过滤掉其他按键
        key_flag = 0;
        if (*key_val == NO_KEY) {
            return NO_KEY;
        }
        return 0xffff;
    }

    if (*key_val == KEY_VOL_DOWN_PREV) {
        if (key_cnt_0 > 1) {    //消抖
            key_cnt_0 = 0;
            if (key_flag & BIT(1)) {
                key_flag = 0;
                return KEY_VOL_UP_NEXT | KEY_SHORT_UP;   //顺时针
            } else {
                key_flag |= BIT(0);
            }
        } else {
            key_cnt_0++;
        }
    }
    if (*key_val == KEY_VOL_UP_NEXT) {
        if (key_cnt_1 > 1) {    //消抖
            key_cnt_1 = 0;
            if (key_flag & BIT(0)) {
                key_flag = 0;
                return KEY_VOL_DOWN_PREV | KEY_SHORT_UP; //逆时针
            } else {
                key_flag |= BIT(1);
            }
        } else {
            key_cnt_1++;
        }
    }
    *key_val = NO_KEY;
    return NO_KEY;
}
#endif

AT(.com_text.bsp.key)
u16 bsp_key_process(u16 key_val)
{
    u16 key_return = NO_KEY;
#if USER_KEY_KNOB2_EN
 	static u8 timer1ms_cnt=0;
    timer1ms_cnt++;
    u16 key_ret = key_knob_process2(&key_val);
    if (key_ret != 0xffff && key_ret != NO_KEY) {
        return key_ret;
    }
    if(timer1ms_cnt > 5) {
        timer1ms_cnt=0;

        key_return = key_process(key_val);

    //双击处理
#if USER_MULTI_PRESS_EN
        //配置工具是否使能了按键2/3/4/5击功能？
        if (xcfg_cb.user_key_multi_press_en) {
            key_return = key_multi_press_process(key_return);
        }
#endif
        return key_return;
    }
    return NO_KEY;
#else
	key_return = key_process(key_val);

    //双击处理
#if USER_MULTI_PRESS_EN
    //配置工具是否使能了按键2/3/4/5击功能？
    if (xcfg_cb.user_key_multi_press_en) {
        key_return = key_multi_press_process(key_return);
    }
#endif
    return key_return;
#endif
}

// AT(.com_rodata.bsp.key)
// const char key_str[] = "enqueue: %04x\n";

// AT(.com_text.str)
// const char str_ii[] = "tkey:%d\n";

AT(.com_text.bsp.key)
u8 bsp_key_scan(void)
{
    u8 key_val = NO_KEY;
    u16 key = NO_KEY;

#if TKEY_MUL_SCAN_EN && !TKEY_SCAN_SWITCH_EN
    if(tkey_finish_flag) {
        if(key_val == NO_KEY) {
            key_val = get_tkey();
        }
    }
#endif // TKEY_MUL_SCAN_EN

#if USER_ADKEY
    if (key_val == NO_KEY) {
        key_val = get_adkey(saradc_get_value8(ADKEY_CH), xcfg_cb.user_adkey1_en);
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (key_val == NO_KEY) {
        key_val = get_adkey2(saradc_get_value8(ADKEY2_CH));
    }
#endif // USER_ADKEY2

#if USER_IOKEY
    if (key_val == NO_KEY) {
        key_val = get_iokey();
    }
#endif // USER_IOKEY

#if VBAT_DETECT_EN
    sys_cb.vbat = bsp_vbat_get_voltage(0);
#endif // VBAT_DETECT_EN

#if IRRX_SW_EN
    if (key_val == NO_KEY) {
        key_val = get_irkey();
    }
#endif // IRRX_SW_EN

#if MUL_PWRON_IO_EN
    if (key_val == NO_KEY) {
        key_val = get_mulpwr_key();
    }
#endif

#if MATRIX_XY_KEY_EN
    matrix_xy_key_scan();
    get_matrix_xy_key();
#endif

    key = bsp_key_process(key_val);
    if (key != NO_KEY) {
        //防止enqueue多次HOLD消息
        if ((key & KEY_TYPE_MASK) == KEY_LONG) {
            sys_cb.kh_vol_msg = (key & 0xff) | KEY_HOLD;
        } else if ((key & KEY_TYPE_MASK) == KEY_LONG_UP) {
            msg_queue_detach(sys_cb.kh_vol_msg, 0);
            sys_cb.kh_vol_msg = NO_KEY;
        } else if (sys_cb.kh_vol_msg == key) {
            msg_queue_detach(key, 0);
        }
        // printf(key_str, key);
        msg_enqueue(key);
    }
    return key_val;
}

