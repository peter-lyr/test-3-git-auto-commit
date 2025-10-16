#ifndef __BSP_ASR_H
#define __BSP_ASR_H

enum {
    ASR_CMD_1_CASE,
    ASR_CMD_2_CASE,
    ASR_CMD_3_CASE,
    ASR_CMD_4_CASE,
    ASR_CMD_5_CASE,
    ///...
};

void tdnn_test(void);
void bsp_asr_init(void);
void bsp_asr_start(void);
void bsp_asr_stop(void);
#endif
