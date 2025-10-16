#ifndef _API_FFT_H
#define _API_FFT_H

void fft_forward(s32 *buf);         //fft
void fft_inverse(s32* buf);         //ifft
void fft_512_init(void);            //fft处理512 byte
void fft_256_init(void);            //fft处理256 byte
void fft_128_init(void);            //fft处理128 byte
#endif
