#include "include.h"
#include "func.h"
#include "func_aux.h"

#if FUNC_AUX_EN
void func_aux_message(u16 msg)
{
    switch (msg) {

    case KU_PLAY:
        func_aux_pause_play();
        break;

    case KU_REC:
#if REC_AUX_EN
        if (!f_aux.rec_start) {
            bsp_record_start();
            f_aux.rec_start = 1;
        } else {
            f_aux.rec_start = 0;
            bsp_record_stop();
        }
#endif // REC_AUX_EN
        break;

#if LINEIN_DETECT_EN
    case EVT_LINEIN_REMOVE:
        func_cb.sta = FUNC_NULL;
        break;
#endif // LINEIN_DETECT_EN

    default:
        func_message(msg);
        break;
    }
}
#endif // FUNC_AUX_EN

