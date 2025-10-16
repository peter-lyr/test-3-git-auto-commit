#include "include.h"
#include "func.h"
#include "func_clock.h"

#if FUNC_CLOCK_EN
rtc_time_t rtc_tm AT(.buf.rtc.clock);
time_cb_t tm_cb AT(.buf.rtc.clock);

AT(.text.func.clock.tbl)
const char week_str[7][4] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

AT(.text.func.clock.init)
void rtc_clock_init(void)
{
    rtc_tm.tm_year = (2019-1970);
    rtc_tm.tm_mon  = 7;
    rtc_tm.tm_mday = 11;
    rtc_tm.tm_hour = 23;
    rtc_tm.tm_min  = 59;
    rtc_tm.tm_sec  = 0;
    rtc_tm.tm_wday = get_weekday(rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday);
    RTCCNT = rtc_tm_to_time(&rtc_tm);
}

//多少秒后闹钟响
AT(.text.func.clock)
void rtc_set_alarm_relative_time(u32 nsec)
{
    rtc_time_to_tm(RTCCNT, &rtc_tm);            //更新时间结构体
    RTCALM = rtc_tm_to_time(&rtc_tm) + nsec;    //设置闹钟相对于当前时间n秒后
}

//设置多少秒后闹钟唤醒
AT(.text.func.clock)
void rtc_set_alarm_wakeup(u32 nsec)
{
    uint rtccon3 = RTCCON3;

    RTCCPND = BIT(17);                          //clear RTC alarm pending
    RTCCON9 = BIT(0);                           //clear alarm pending
    rtc_set_alarm_relative_time(nsec);

    rtccon3 |= BIT(8);                          //RTC alarm wakeup enable
    RTCCON3 = rtccon3;
}

#if SLEEP_ALARM_WAKEUP_EN
//sleep mode下设置多少秒后闹钟唤醒
AT(.text.func.clock)
void rtc_set_alarm_wakeup_sleepmode(u32 nsec)
{
    RTCCPND = BIT(17);      //clear rtc alarm pending
    RTCCON9 = BIT(0);                           //clear alarm pending
    rtc_set_alarm_relative_time(nsec);
    RTCCON |= BIT(8);      //rtc alarm wakeup enable
}
#endif

AT(.text.func.clock)
void func_clock_process(void)
{
    func_process();

#if !GUI_LCD_EN
    //通过ALARM每隔5秒打印一次时间，仅用于测试
    if (RTCCON10 & BIT(0)) {
        RTCCON3 &= ~BIT(8);                             //latch setn set 1
        RTCCON10 = BIT(0);                              //clr latch rstn

        if (!tm_cb.setting) {
            rtc_time_to_tm(RTCCNT, &rtc_tm);            //更新时间结构体
            RTCALM = rtc_tm_to_time(&rtc_tm) + 4;       //设置闹钟相对于当前时间5秒后
            printf("time:  %d.%02d.%02d   %02d:%02d:%02d  %s\n", (rtc_tm.tm_year+1970), rtc_tm.tm_mon, rtc_tm.tm_mday,
                       rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec, week_str[rtc_tm.tm_wday]);
        }
        RTCCON3 |= BIT(8);                              //alm wake up en enable
    }
#endif
}

static void func_clock_enter(void)
{
    func_process();
    memset(&tm_cb, 0, sizeof(tm_cb));

    func_cb.mp3_res_play = mp3_res_play;
    sys_cb.rtc_first_pwron = 0;

#if EXT_32K_EN
    clk_clk32k_rtc_set(CLK32K_RTC_RC2M_RTC);
    u32 osc32k_clk = sys_get_osc32k_clkhz();
    rtc_prescale_set(osc32k_clk - 1);
    rtc_counter_set(0);
#else
    clk_clk32k_rtc_set(CLK32K_RTC_RC2M_RTC);
    u32 rc2m_clk = rc2m_clk_nhz_get(1);
    rtc_prescale_set(rc2m_clk - 1);
    rtc_counter_set(0);
#endif
    
    rtc_time_to_tm(RTCCNT, &rtc_tm);        //更新时间结构体
    rtc_set_alarm_wakeup(4);                //设置闹钟相对于当前时间5秒后
    func_clock_enter_display();
#if WARNING_FUNC_CLOCK
    mp3_res_play(RES_BUF_CLOCK_MODE_MP3, RES_LEN_CLOCK_MODE_MP3);
#endif // WARNING_FUNC_CLOCK
}

static void func_clock_exit(void)
{
    func_clock_exit_display();
    func_cb.last = FUNC_CLOCK;
}

AT(.text.func.clock)
void func_clock(void)
{
    printf("%s\n", __func__);

    func_clock_enter();

    while (func_cb.sta == FUNC_CLOCK) {
        func_clock_process();
        func_clock_message(msg_dequeue());
        func_clock_display();
    }

    func_clock_exit();
}
#endif  //FUNC_CLOCK_EN
