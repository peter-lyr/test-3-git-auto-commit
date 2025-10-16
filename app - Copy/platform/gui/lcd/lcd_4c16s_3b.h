#ifndef _LCDSEG_4C16S_H
#define _LCDSEG_4C16S_H

#define PA_0_CHANNEL        0
#define PA_1_CHANNEL        1
#define PA_2_CHANNEL        2
#define PA_3_CHANNEL        3
#define PA_4_CHANNEL        4
#define PA_5_CHANNEL        5
#define PA_6_CHANNEL        6
#define PA_10_CHANNEL       7
#define PA_11_CHANNEL       8
#define PA_12_CHANNEL       9
#define PA_13_CHANNEL       10
#define PA_14_CHANNEL       11
#define PA_15_CHANNEL       12
#define PB_0_CHANNEL        13
#define PB_1_CHANNEL        14
#define PB_2_CHANNEL        15
#define PB_3_CHANNEL        16
#define PB_4_CHANNEL        17
#define PB_5_CHANNEL        18
#define PB_6_CHANNEL        19

// LCDDRVCON = (LCDDRVCON & ~0xffffff) | BIT(PA_0_CHANNEL);
#define PA_0_SET_H()            {LCDDRVSEL0 |= (3<<(PA_0_CHANNEL*2));}  //PA0
#define PA_0_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_0_CHANNEL*2));}
#define PA_0_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_0_CHANNEL*2));}
#define PA_0_SET_L()            {LCDDRVSEL0 |= (0<<(PA_0_CHANNEL*2));}
#define PA_0_CLR()              {LCDDRVSEL0 &= ~(3<<PA_0_CHANNEL*2);}

#define PA_1_SET_H()            {LCDDRVSEL0 |= (3<<(PA_1_CHANNEL*2));}  //PA1
#define PA_1_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_1_CHANNEL*2));}
#define PA_1_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_1_CHANNEL*2));}
#define PA_1_SET_L()            {LCDDRVSEL0 |= (0<<(PA_1_CHANNEL*2));}
#define PA_1_CLR()              {LCDDRVSEL0 &= ~(3<<PA_1_CHANNEL*2);}

#define PA_2_SET_H()            {LCDDRVSEL0 |= (3<<(PA_2_CHANNEL*2));}  //PA2
#define PA_2_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_2_CHANNEL*2));}
#define PA_2_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_2_CHANNEL*2));}
#define PA_2_SET_L()            {LCDDRVSEL0 |= (0<<(PA_2_CHANNEL*2));}
#define PA_2_CLR()              {LCDDRVSEL0 &= ~(3<<PA_2_CHANNEL*2);}

#define PA_3_SET_H()            {LCDDRVSEL0 |= (3<<(PA_3_CHANNEL*2));}  //PA3
#define PA_3_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_3_CHANNEL*2));}
#define PA_3_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_3_CHANNEL*2));}
#define PA_3_SET_L()            {LCDDRVSEL0 |= (0<<(PA_3_CHANNEL*2));}
#define PA_3_CLR()              {LCDDRVSEL0 &= ~(3<<PA_3_CHANNEL*2);}

#define PA_4_SET_H()            {LCDDRVSEL0 |= (3<<(PA_4_CHANNEL*2));}  //PA4
#define PA_4_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_4_CHANNEL*2));}
#define PA_4_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_4_CHANNEL*2));}
#define PA_4_SET_L()            {LCDDRVSEL0 |= (0<<(PA_4_CHANNEL*2));}
#define PA_4_CLR()              {LCDDRVSEL0 &= ~(3<<PA_4_CHANNEL*2);}

#define PA_5_SET_H()            {LCDDRVSEL0 |= (3<<(PA_5_CHANNEL*2));}  //PA5
#define PA_5_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_5_CHANNEL*2));}
#define PA_5_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_5_CHANNEL*2));}
#define PA_5_SET_L()            {LCDDRVSEL0 |= (0<<(PA_5_CHANNEL*2));}
#define PA_5_CLR()              {LCDDRVSEL0 &= ~(3<<PA_5_CHANNEL*2);}

#define PA_6_SET_H()            {LCDDRVSEL0 |= (3<<(PA_6_CHANNEL*2));}  //PA6
#define PA_6_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PA_6_CHANNEL*2));}
#define PA_6_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PA_6_CHANNEL*2));}
#define PA_6_SET_L()            {LCDDRVSEL0 |= (0<<(PA_6_CHANNEL*2));}
#define PA_6_CLR()              {LCDDRVSEL0 &= ~(3<<PA_6_CHANNEL*2);}

#define PA_10_SET_H()           {LCDDRVSEL0 |= (3<<(PA_10_CHANNEL*2));}  //PA10
#define PA_10_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_10_CHANNEL*2));}
#define PA_10_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_10_CHANNEL*2));}
#define PA_10_SET_L()           {LCDDRVSEL0 |= (0<<(PA_10_CHANNEL*2));}
#define PA_10_CLR()             {LCDDRVSEL0 &= ~(3<<PA_10_CHANNEL*2);}

#define PA_11_SET_H()           {LCDDRVSEL0 |= (3<<(PA_11_CHANNEL*2));}  //PA11
#define PA_11_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_11_CHANNEL*2));}
#define PA_11_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_11_CHANNEL*2));}
#define PA_11_SET_L()           {LCDDRVSEL0 |= (0<<(PA_11_CHANNEL*2));}
#define PA_11_CLR()             {LCDDRVSEL0 &= ~(3<<PA_11_CHANNEL*2);}

#define PA_12_SET_H()           {LCDDRVSEL0 |= (3<<(PA_12_CHANNEL*2));}  //PA12
#define PA_12_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_12_CHANNEL*2));}
#define PA_12_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_12_CHANNEL*2));}
#define PA_12_SET_L()           {LCDDRVSEL0 |= (0<<(PA_12_CHANNEL*2));}
#define PA_12_CLR()             {LCDDRVSEL0 &= ~(3<<PA_12_CHANNEL*2);}

#define PA_13_SET_H()           {LCDDRVSEL0 |= (3<<(PA_13_CHANNEL*2));}  //PA13
#define PA_13_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_13_CHANNEL*2));}
#define PA_13_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_13_CHANNEL*2));}
#define PA_13_SET_L()           {LCDDRVSEL0 |= (0<<(PA_13_CHANNEL*2));}
#define PA_13_CLR()             {LCDDRVSEL0 &= ~(3<<PA_13_CHANNEL*2);}

#define PA_14_SET_H()           {LCDDRVSEL0 |= (3<<(PA_14_CHANNEL*2));}  //PA14
#define PA_14_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_14_CHANNEL*2));}
#define PA_14_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_14_CHANNEL*2));}
#define PA_14_SET_L()           {LCDDRVSEL0 |= (0<<(PA_14_CHANNEL*2));}
#define PA_14_CLR()             {LCDDRVSEL0 &= ~(3<<PA_14_CHANNEL*2);}

#define PA_15_SET_H()           {LCDDRVSEL0 |= (3<<(PA_15_CHANNEL*2));}  //PA15
#define PA_15_SET_BIAS2_3()     {LCDDRVSEL0 |= (2<<(PA_15_CHANNEL*2));}
#define PA_15_SET_BIAS1_3()     {LCDDRVSEL0 |= (1<<(PA_15_CHANNEL*2));}
#define PA_15_SET_L()           {LCDDRVSEL0 |= (0<<(PA_15_CHANNEL*2));}
#define PA_15_CLR()              {LCDDRVSEL0 &= ~(3<<PA_15_CHANNEL*2);}

#define PB_0_SET_H()            {LCDDRVSEL0 |= (3<<(PB_0_CHANNEL*2));}  //PB0
#define PB_0_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PB_0_CHANNEL*2));}
#define PB_0_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PB_0_CHANNEL*2));}
#define PB_0_SET_L()            {LCDDRVSEL0 |= (0<<(PB_0_CHANNEL*2));}
#define PB_0_CLR()              {LCDDRVSEL0 &= ~(3<<PB_0_CHANNEL*2);}

#define PB_1_SET_H()            {LCDDRVSEL0 |= (3<<(PB_1_CHANNEL*2));}  //PB1
#define PB_1_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PB_1_CHANNEL*2));}
#define PB_1_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PB_1_CHANNEL*2));}
#define PB_1_SET_L()            {LCDDRVSEL0 |= (0<<(PB_1_CHANNEL*2));}
#define PB_1_CLR()              {LCDDRVSEL0 &= ~(3<<PB_1_CHANNEL*2);}

#define PB_2_SET_H()            {LCDDRVSEL0 |= (3<<(PB_2_CHANNEL*2));}  //PB2
#define PB_2_SET_BIAS2_3()      {LCDDRVSEL0 |= (2<<(PB_2_CHANNEL*2));}
#define PB_2_SET_BIAS1_3()      {LCDDRVSEL0 |= (1<<(PB_2_CHANNEL*2));}
#define PB_2_SET_L()            {LCDDRVSEL0 |= (0<<(PB_2_CHANNEL*2));}
#define PB_2_CLR()              {LCDDRVSEL0 &= ~(3<<PB_2_CHANNEL*2);}

#define PB_3_SET_H()            {LCDDRVSEL1 |= (3<<((PB_3_CHANNEL-16)*2));}  //PB3
#define PB_3_SET_BIAS2_3()      {LCDDRVSEL1 |= (2<<((PB_3_CHANNEL-16)*2));}
#define PB_3_SET_BIAS1_3()      {LCDDRVSEL1 |= (1<<((PB_3_CHANNEL-16)*2));}
#define PB_3_SET_L()            {LCDDRVSEL1 |= (0<<((PB_3_CHANNEL-16)*2));}
#define PB_3_CLR()              {LCDDRVSEL1 &= ~(3<<(PB_3_CHANNEL-16)*2);}

#define PB_4_SET_H()            {LCDDRVSEL1 |= (3<<((PB_4_CHANNEL-16)*2));}  //PB4
#define PB_4_SET_BIAS2_3()      {LCDDRVSEL1 |= (2<<((PB_4_CHANNEL-16)*2));}
#define PB_4_SET_BIAS1_3()      {LCDDRVSEL1 |= (1<<((PB_4_CHANNEL-16)*2));}
#define PB_4_SET_L()            {LCDDRVSEL1 |= (0<<((PB_4_CHANNEL-16)*2));}
#define PB_4_CLR()              {LCDDRVSEL1 &= ~(3<<(PB_4_CHANNEL-16)*2);}

#define PB_5_SET_H()            {LCDDRVSEL1 |= (3<<((PB_5_CHANNEL-16)*2));}  //PB5
#define PB_5_SET_BIAS2_3()      {LCDDRVSEL1 |= (2<<((PB_5_CHANNEL-16)*2));}
#define PB_5_SET_BIAS1_3()      {LCDDRVSEL1 |= (1<<((PB_5_CHANNEL-16)*2));}
#define PB_5_SET_L()            {LCDDRVSEL1 |= (0<<((PB_5_CHANNEL-16)*2));}
#define PB_5_CLR()              {LCDDRVSEL1 &= ~(3<<(PB_5_CHANNEL-16)*2);}

#define PB_6_SET_H()            {LCDDRVSEL1 |= (3<<((PB_6_CHANNEL-16)*2));}  //PB6
#define PB_6_SET_BIAS2_3()      {LCDDRVSEL1 |= (2<<((PB_6_CHANNEL-16)*2));}
#define PB_6_SET_BIAS1_3()      {LCDDRVSEL1 |= (1<<((PB_6_CHANNEL-16)*2));}
#define PB_6_SET_L()            {LCDDRVSEL1 |= (0<<((PB_6_CHANNEL-16)*2));}
#define PB_6_CLR()              {LCDDRVSEL1 &= ~(3<<(PB_6_CHANNEL-16)*2);}

//PA14\15丝印反
#define COM1_SET_H()            PA_0_SET_H()
#define COM1_SET_BIAS2_3()      PA_0_SET_BIAS2_3()
#define COM1_SET_BIAS1_3()      PA_0_SET_BIAS1_3()
#define COM1_SET_L()            PA_0_SET_L()
#define COM1_CLR()              PA_0_CLR()

#define COM2_SET_H()            PA_1_SET_H()
#define COM2_SET_BIAS2_3()      PA_1_SET_BIAS2_3()
#define COM2_SET_BIAS1_3()      PA_1_SET_BIAS1_3()
#define COM2_SET_L()            PA_1_SET_L()
#define COM2_CLR()              PA_1_CLR()

#define COM3_SET_H()            PA_2_SET_H()
#define COM3_SET_BIAS2_3()      PA_2_SET_BIAS2_3()
#define COM3_SET_BIAS1_3()      PA_2_SET_BIAS1_3()
#define COM3_SET_L()            PA_2_SET_L()
#define COM3_CLR()              PA_2_CLR()

#define COM4_SET_H()            PA_3_SET_H()
#define COM4_SET_BIAS2_3()      PA_3_SET_BIAS2_3()
#define COM4_SET_BIAS1_3()      PA_3_SET_BIAS1_3()
#define COM4_SET_L()            PA_3_SET_L()
#define COM4_CLR()              PA_3_CLR()

#define SEG0_SET_H()            PA_4_SET_H()
#define SEG0_SET_BIAS2_3()      PA_4_SET_BIAS2_3()
#define SEG0_SET_BIAS1_3()      PA_4_SET_BIAS1_3()
#define SEG0_SET_L()            PA_4_SET_L()
#define SEG0_CLR()              PA_4_CLR()

#define SEG1_SET_H()            PA_5_SET_H()
#define SEG1_SET_BIAS2_3()      PA_5_SET_BIAS2_3()
#define SEG1_SET_BIAS1_3()      PA_5_SET_BIAS1_3()
#define SEG1_SET_L()            PA_5_SET_L()
#define SEG1_CLR()              PA_5_CLR()

#define SEG2_SET_H()            PA_6_SET_H()
#define SEG2_SET_BIAS2_3()      PA_6_SET_BIAS2_3()
#define SEG2_SET_BIAS1_3()      PA_6_SET_BIAS1_3()
#define SEG2_SET_L()            PA_6_SET_L()
#define SEG2_CLR()              PA_6_CLR()

#define SEG3_SET_H()            PA_10_SET_H()
#define SEG3_SET_BIAS2_3()      PA_10_SET_BIAS2_3()
#define SEG3_SET_BIAS1_3()      PA_10_SET_BIAS1_3()
#define SEG3_SET_L()            PA_10_SET_L()
#define SEG3_CLR()              PA_10_CLR()

#define SEG4_SET_H()            PA_11_SET_H()
#define SEG4_SET_BIAS2_3()      PA_11_SET_BIAS2_3()
#define SEG4_SET_BIAS1_3()      PA_11_SET_BIAS1_3()
#define SEG4_SET_L()            PA_11_SET_L()
#define SEG4_CLR()              PA_11_CLR()

#define SEG5_SET_H()            PA_12_SET_H()
#define SEG5_SET_BIAS2_3()      PA_12_SET_BIAS2_3()
#define SEG5_SET_BIAS1_3()      PA_12_SET_BIAS1_3()
#define SEG5_SET_L()            PA_12_SET_L()
#define SEG5_CLR()              PA_12_CLR()

#define SEG6_SET_H()            PA_13_SET_H()
#define SEG6_SET_BIAS2_3()      PA_13_SET_BIAS2_3()
#define SEG6_SET_BIAS1_3()      PA_13_SET_BIAS1_3()
#define SEG6_SET_L()            PA_13_SET_L()
#define SEG6_CLR()              PA_13_CLR()

#define SEG7_SET_H()            PA_14_SET_H()
#define SEG7_SET_BIAS2_3()      PA_14_SET_BIAS2_3()
#define SEG7_SET_BIAS1_3()      PA_14_SET_BIAS1_3()
#define SEG7_SET_L()            PA_14_SET_L()
#define SEG7_CLR()              PA_14_CLR()

#define SEG8_SET_H()            PA_15_SET_H()
#define SEG8_SET_BIAS2_3()      PA_15_SET_BIAS2_3()
#define SEG8_SET_BIAS1_3()      PA_15_SET_BIAS1_3()
#define SEG8_SET_L()            PA_15_SET_L()
#define SEG8_CLR()              PA_15_CLR()

#define SEG9_SET_H()            PB_0_SET_H()
#define SEG9_SET_BIAS2_3()      PB_0_SET_BIAS2_3()
#define SEG9_SET_BIAS1_3()      PB_0_SET_BIAS1_3()
#define SEG9_SET_L()            PB_0_SET_L()
#define SEG9_CLR()              PB_0_CLR()

#define SEG10_SET_H()           PB_1_SET_H()
#define SEG10_SET_BIAS2_3()     PB_1_SET_BIAS2_3()
#define SEG10_SET_BIAS1_3()     PB_1_SET_BIAS1_3()
#define SEG10_SET_L()           PB_1_SET_L()
#define SEG10_CLR()             PB_1_CLR()

#define SEG11_SET_H()           PB_2_SET_H()
#define SEG11_SET_BIAS2_3()     PB_2_SET_BIAS2_3()
#define SEG11_SET_BIAS1_3()     PB_2_SET_BIAS1_3()
#define SEG11_SET_L()           PB_2_SET_L()
#define SEG11_CLR()             PB_2_CLR()

#define SEG12_SET_H()           PB_3_SET_H()
#define SEG12_SET_BIAS2_3()     PB_3_SET_BIAS2_3()
#define SEG12_SET_BIAS1_3()     PB_3_SET_BIAS1_3()
#define SEG12_SET_L()           PB_3_SET_L()
#define SEG12_CLR()             PB_3_CLR()

#define SEG13_SET_H()           PB_4_SET_H()
#define SEG13_SET_BIAS2_3()     PB_4_SET_BIAS2_3()
#define SEG13_SET_BIAS1_3()     PB_4_SET_BIAS1_3()
#define SEG13_SET_L()           PB_4_SET_L()
#define SEG13_CLR()             PB_4_CLR()

#define SEG14_SET_H()           PB_5_SET_H()
#define SEG14_SET_BIAS2_3()     PB_5_SET_BIAS2_3()
#define SEG14_SET_BIAS1_3()     PB_5_SET_BIAS1_3()
#define SEG14_SET_L()           PB_5_SET_L()
#define SEG14_CLR()             PB_5_CLR()

#define SEG15_SET_H()           PB_6_SET_H()
#define SEG15_SET_BIAS2_3()     PB_6_SET_BIAS2_3()
#define SEG15_SET_BIAS1_3()     PB_6_SET_BIAS1_3()
#define SEG15_SET_L()           PB_6_SET_L()
#define SEG15_CLR()             PB_6_CLR()

#define SEG_ALL_SET_BIAS1_3()   {LCDDRVSEL0 &= ~0xFFFFFF00; LCDDRVSEL0 |= 0x55555500; LCDDRVSEL1 &= ~0xFF; LCDDRVSEL1 |= 0x55;}
#define SEG_ALL_SET_BIAS2_3()   {LCDDRVSEL0 &= ~0xFFFFFF00; LCDDRVSEL0 |= 0xAAAAAA00; LCDDRVSEL1 &= ~0xFF; LCDDRVSEL1 |= 0xAA;}

typedef struct {
    u8 buf[7];
    u8 seg_cnt;
    u8 com_cnt;
    volatile u8 disp_en;            //中断是否刷屏
} lcdseg_cb_t;

void lcd_4c16s_init(void);
void lcd_4c16s_scan(void);
void lcd_4c16s_dis_test(void);

#endif