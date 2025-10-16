#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

///CM Init时，判断该Page参数是否有效，有效则加载
bool cm_loadparam(void *buff, uint page)
{
    return true;
}

///CM Init时，如果找不到有效的参数，则进行出厂值设置
void cm_factory(void *buff, uint page)
{
    u8 *buf = buff;
    memset(buf, 0, 250);

    TRACE("cm_factory: %d\n", page);
    if (page == SYS_CM_PAGE_NUM) {
        //系统参数初始化
//        f_msc.file_num = 1;
        sys_cb.vol = SYS_INIT_VOLUME;

#if MUSIC_BREAKPOINT_EN
        memset(&f_msc.brkpt, 0, 10);
#endif // MUSIC_BREAKPOINT_EN

        if (xcfg_cb.lang_id == 2) {
            sys_cb.lang_id = 0;             //出厂默认英文
        } else if (xcfg_cb.lang_id == 3) {
            sys_cb.lang_id = 1;             //出厂默认中文
        } else {
            sys_cb.lang_id = xcfg_cb.lang_id;
        }
//        PUT_LE16(buf + PARAM_MSC_NUM_SD, f_msc.file_num);
//        PUT_LE16(buf + PARAM_MSC_NUM_USB, f_msc.file_num);
        buf[PARAM_SYS_VOL] = sys_cb.vol;
        buf[PARAM_LANG_ID] = sys_cb.lang_id;
    }
}
