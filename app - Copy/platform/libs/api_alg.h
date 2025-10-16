#ifndef _API_ALG_H
#define _API_ALG_H


///------------------------------------------------------------------------------------------
///freq_shift2算法结构体以及相关API声明
void holwing_ex_init(unsigned short freq, signed short hz, signed short hz_step);   //freq表示音频数据的采样率, //level表示移多少HZ,如15表示正向移15HZ, -15表示负向移15HZ   //内部是以5HZ为单位去移的,level这里最好是5的倍速数
void howlingex_process(short *out, short *in, int samples);

#endif
