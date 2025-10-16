#ifndef _BSP_RTC_H
#define _BSP_RTC_H


void rtc_set_alarm(u32 alarm);
u32 rtc_get_alarm(void);
void rtc_clr_alarm(void);

void rtc_prescale_set(uint32_t psc);
u32 rtc_prescale_get(void);

u32 rtc_counter_get(void);
void rtc_counter_set(u32 cnt);

#endif  //_BSP_RTC_H
