#ifndef _API_SDADC_DC_H
#define _API_SDADC_DC_H

//模拟通道：1P--PA1 2P--PA3 3P--PA4 4P--PA10 pmu_vbg vbat--PA9
//			1N--PA2         3N--PB4
//数字通道：ch0     ch1     ch2     ch3      ch4     ch5


#define SDADCCH_PA1         0               //SD_ADC1P(PA1)    ch0
#define SDADCCH_PA3         1               //SD_ADC2P(PA3)    ch1
#define SDADCCH_PA4         2               //SD_ADC3P(PA4)    ch2
#define SDADCCH_PA10        3               //SD_ADC4P(PA10)   ch3
#define SDADCCH_VBG         4               //SD_ADC_VBG       ch4
#define SDADCCH_VBAT        5               //SD_ADC_VBAT(PA9) ch5


typedef struct {
    u16 channel;
    u16 value[6];
} sdadc_dc_cb_t;
extern sdadc_dc_cb_t sdadc_dc_cb;

#define sdadc_dc_get_value(n)   sdadc_dc_cb.value[n]    //获取某个channel的ADC值

void sdadc_dc_var_init(void);
uint16_t sdadc_dc_set_channel(uint16_t channel);        //设置ADC channel，返回值：设置前的channel
void sdadc_dc_clr_channel(uint16_t channel);            //清除ADC channel
uint16_t sdadc_dc_get_channel(void);                    //获取ADC channel
u32 sdadc_dc_get_voltage(u16 adc_val);                  //ADC值(无符号数)转换成电压(mv)
void sdadc_dc_init(void);                               //初始化SDADC DC Measurement
uint16_t sdadc_dc_exit(void);                           //关闭SDADC DC, 返回channel
void sdadc_dc_kick_start(void);                         //启动ADC转换并等待完成, 一个通道转换5us, 根据sdadc_dc_cb.channel启动已设置的channel
u16 volt_adc_trans(u16 vin);                            //电压转换成adc值，用于阈值关闭指定IO

//ADC值量化公式: target = 32768  * (Vin - Vref/2) / (Vref/2) / 4 + 32768, Vin测量电压, Vref = 2.5V
//dir: 0->指定通路ADC值小于target时产生中断; 1->指定通路ADC值大于target时产生中断
void sdadc_dc_set_compare(u16 target, u16 channel, u32 dir);

#endif
