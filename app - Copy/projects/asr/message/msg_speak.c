#include "include.h"
#include "func.h"
#include "func_speaker.h"

#if FUNC_SPEAKER_EN
bool spiflash_rec_play_last_file(void);
void func_speaker_message(u16 msg){
    switch (msg) {

    case KU_PLAY:
        break;

    case KU_REC:
#if REC_SPEAKER_EN
        if (!f_spk.rec_start) {
            bsp_record_start();
            f_spk.rec_start = 1;
        } else {
            f_spk.rec_start = 0;
            bsp_record_stop();
            #if REC_FAST_PLAY_INTERNAL
            spiflash_rec_play_last_file();
            #endif
        }
#endif // REC_SPEAKER_EN
        break;

    default:
        func_message(msg);
        break;
    }
}
#endif // FUNC_SPEAKER_EN

