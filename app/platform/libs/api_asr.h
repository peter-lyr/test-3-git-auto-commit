#ifndef _API_ASR_H
#define _API_ASR_H

//tdnn set in/out dimension
void tdnn_set_dimwnsion(uint32_t outdim, uint32_t indim);

//tdnn enable
void tdnn_start(void);

//tdnn finish
void tdnn_fini(void);

//tdnn mode: 0- flash input data 8bit, 1-flash input data 4bit,
void tdnn_mode_set(u8 mode);

//tdnn kick start
void tdnn_kick(int32_t *out, int8_t *in);

//tdnn wait calulate done
void tdnn_wait(void);

#endif
