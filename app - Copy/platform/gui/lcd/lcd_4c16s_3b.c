#include "include.h"
#include "lcd_4c16s_3b.h"

#if (GUI_SELECT == GUI_LCDSEG)

#define COM1              BIT(0)
#define COM2              BIT(1)
#define COM3              BIT(2)
#define COM4              BIT(3)

#define SEG_A             BIT(0)      //com1
#define SEG_B             BIT(4)      //com1
#define SEG_C             BIT(6)      //com3
#define SEG_D             BIT(3)      //com4
#define SEG_E             BIT(2)      //com3
#define SEG_F             BIT(1)      //com2
#define SEG_G             BIT(5)      //com2
#define SEG_H             BIT(7)      //com4

#define F_LCDSEG_0        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define F_LCDSEG_1        (SEG_B | SEG_C)
#define F_LCDSEG_2        (SEG_A | SEG_B | SEG_G | SEG_E | SEG_D)
#define F_LCDSEG_3        (SEG_A | SEG_B | SEG_G | SEG_C | SEG_D)
#define F_LCDSEG_4        (SEG_B | SEG_C | SEG_F | SEG_G)
#define F_LCDSEG_5        (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define F_LCDSEG_6        (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define F_LCDSEG_7        (SEG_A | SEG_B | SEG_C)
#define F_LCDSEG_8        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define F_LCDSEG_9        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)

#define S_SEG_A           BIT(7)
#define S_SEG_B           BIT(3)
#define S_SEG_C           BIT(1)
#define S_SEG_D           BIT(4)
#define S_SEG_E           BIT(5)
#define S_SEG_F           BIT(6)
#define S_SEG_G           BIT(2)
#define S_SEG_H           BIT(0)

#define S_LCDSEG_0        (S_SEG_A | S_SEG_B | S_SEG_C | S_SEG_D | S_SEG_E | S_SEG_F)
#define S_LCDSEG_1        (S_SEG_B | S_SEG_C)
#define S_LCDSEG_2        (S_SEG_A | S_SEG_B | S_SEG_G | S_SEG_E | S_SEG_D)
#define S_LCDSEG_3        (S_SEG_A | S_SEG_B | S_SEG_G | S_SEG_C | S_SEG_D)
#define S_LCDSEG_4        (S_SEG_B | S_SEG_C | S_SEG_F | S_SEG_G)
#define S_LCDSEG_5        (S_SEG_A | S_SEG_C | S_SEG_D | S_SEG_F | S_SEG_G)
#define S_LCDSEG_6        (S_SEG_A | S_SEG_C | S_SEG_D | S_SEG_E | S_SEG_F | S_SEG_G)
#define S_LCDSEG_7        (S_SEG_A | S_SEG_B | S_SEG_C)
#define S_LCDSEG_8        (S_SEG_A | S_SEG_B | S_SEG_C | S_SEG_D | S_SEG_E | S_SEG_F | S_SEG_G)
#define S_LCDSEG_9        (S_SEG_A | S_SEG_B | S_SEG_C | S_SEG_D | S_SEG_F | S_SEG_G)

//显示buf填充
#define DISP_COM4(x)      {lcd_com_buf[6] |= x; lcd_com_buf[7] |= x;}
#define DISP_COM3(x)      {lcd_com_buf[4] |= x; lcd_com_buf[5] |= x;}
#define DISP_COM2(x)      {lcd_com_buf[2] |= x; lcd_com_buf[3] |= x;}
#define DISP_COM1(x)      {lcd_com_buf[0] |= x; lcd_com_buf[1] |= x;}

u16 lcd_com_buf[8] AT(.srodata.lcdbuf);
u16 lcd_scan_buf[8] AT(.srodata.lcdbuf);
u32 lcd_icon[2] AT(.srodata.lcdbuf);
lcdseg_cb_t lcdseg_cb AT(.srodata.lcdbuf.cb);

AT(.rodata.icon)
const u32 DISP_LY[2]        = {BIT(0), COM1};   //seg, com
AT(.rodata.icon)
const u32 DISP_KPA[2]       = {BIT(2), COM4};
AT(.rodata.icon)
const u32 DISP_POINT_1[2]   = {BIT(4), COM4};
AT(.rodata.icon)
const u32 DISP_MMHG[2]      = {BIT(6), COM4};
AT(.rodata.icon)
const u32 DISP_POINT_2[2]   = {BIT(9), COM1};
AT(.rodata.icon)
const u32 DISP_AVG[2]       = {BIT(12), COM1};
AT(.rodata.icon)
const u32 DISP_PERSON1[2]   = {BIT(12), COM4};
AT(.rodata.icon)
const u32 DISP_PERSON2[2]   = {BIT(12), COM3};
AT(.rodata.icon)
const u32 DISP_W6[2]        = {BIT(13), COM3};
AT(.rodata.icon)
const u32 DISP_W5[2]        = {BIT(13), COM2};
AT(.rodata.icon)
const u32 DISP_W4[2]        = {BIT(13), COM1};
AT(.rodata.icon)
const u32 DISP_BT[2]        = {BIT(13), COM4};
AT(.rodata.icon)
const u32 DISP_T[2]         = {BIT(14), COM3};
AT(.rodata.icon)
const u32 DISP_M[2]         = {BIT(14), COM4};

AT(.rodata.num)
const u8 lcdseg_num_table_1[10] =
{
    F_LCDSEG_0, F_LCDSEG_1, F_LCDSEG_2, F_LCDSEG_3, F_LCDSEG_4,
    F_LCDSEG_5, F_LCDSEG_6, F_LCDSEG_7, F_LCDSEG_8, F_LCDSEG_9,
};

AT(.rodata.num)
const u8 lcdseg_num_table_2[10] =
{
    S_LCDSEG_0, S_LCDSEG_1, S_LCDSEG_2, S_LCDSEG_3, S_LCDSEG_4,
    S_LCDSEG_5, S_LCDSEG_6, S_LCDSEG_7, S_LCDSEG_8, S_LCDSEG_9,
};

// AT(.com_text.detect1)
// void lcdseg_update_dispbuf_do(u8 *disp_buf)
// {
//     // ledseg_cb.disp_en = 0;
//     memcpy(lcdseg_cb.buf, disp_buf, sizeof(lcdseg_cb.buf));
//     // ledseg_cb.disp_en = 1;
// }

AT(.lcd_text.lcd)
void lcd_4c16s_dis_icon(const u32* icon)
{
    if(icon[1] == COM1){
        DISP_COM1(icon[0]);
    }
    else if(icon[1] == COM2){
        DISP_COM2(icon[0]);
    }
    else if(icon[1] == COM3){
        DISP_COM3(icon[0]);
    }
    else if(icon[1] == COM4){
        DISP_COM4(icon[0]);
    }
}

AT(.lcd_text.lcd)
void lcd_4c16s_dis_num(u16 dis_num, u16 seg_pin)
{
    if((dis_num & SEG_A) == SEG_A){
        lcd_icon[0] = seg_pin;           //seg num
        lcd_icon[1] = COM1;              //com
        DISP_COM1(lcd_icon[0]);
    }

    if((dis_num & SEG_F) == SEG_F){
        lcd_icon[0] = seg_pin;
        lcd_icon[1] = COM2;
        DISP_COM2(lcd_icon[0]);
    }

    if((dis_num & SEG_E) == SEG_E){
        lcd_icon[0] = seg_pin;
        lcd_icon[1] = COM3;
        DISP_COM3(lcd_icon[0]);
    }

    if((dis_num & SEG_D) == SEG_D){
        lcd_icon[0] = seg_pin;
        lcd_icon[1] = COM4;
        DISP_COM4(lcd_icon[0]);
    }

    if((dis_num & SEG_B) == SEG_B){
        lcd_icon[0] = seg_pin<<1;
        lcd_icon[1] = COM1;
        DISP_COM1(lcd_icon[0]);
    }

    if((dis_num & SEG_G) == SEG_G){
        lcd_icon[0] = seg_pin<<1;
        lcd_icon[1] = COM2;
        DISP_COM2(lcd_icon[0]);
    }

    if((dis_num & SEG_C) == SEG_C){
        lcd_icon[0] = seg_pin<<1;
        lcd_icon[1] = COM3;
        DISP_COM3(lcd_icon[0]);
    }

    if((dis_num & SEG_H) == SEG_H){
        lcd_icon[0] = seg_pin<<1;
        lcd_icon[1] = COM4;
        DISP_COM4(lcd_icon[0]);
    }
}

AT(.lcd_text.lcd)
void lcd_4c16s_dis_number_1(u16 num)
{
    u8 seg_buf[3];
    if (num > 999) {
        num = 999;
    }
    seg_buf[0] = lcdseg_num_table_1[num / 100];
    seg_buf[1] = lcdseg_num_table_1[(num % 100) / 10];
    seg_buf[2] = lcdseg_num_table_1[(num % 10)];

    lcd_4c16s_dis_num(seg_buf[0], BIT(1));
    lcd_4c16s_dis_num(seg_buf[1], BIT(3));
    lcd_4c16s_dis_num(seg_buf[2], BIT(5));
}

AT(.lcd_text.lcd)
void lcd_4c16s_dis_number_2(u16 num)
{
    u8 seg_buf[3];
    if (num > 999) {
        num = 999;
    }
    seg_buf[0] = lcdseg_num_table_2[num / 100];
    seg_buf[1] = lcdseg_num_table_2[(num % 100) / 10];
    seg_buf[2] = lcdseg_num_table_2[(num % 10)];

    lcd_4c16s_dis_num(seg_buf[2], BIT(7));
    lcd_4c16s_dis_num(seg_buf[1], BIT(9));
}

// AT(.com_text.rodata)
// const char com_buf[] = "lcd_com_buf:%x %x %x %x %x %x\n";

AT(.com_text.lcd.set)
void lcd_4c16s_val_set(u8 num)
{
    memcpy(lcd_scan_buf,lcd_com_buf,sizeof(lcd_com_buf));
    u16 com_data = lcd_scan_buf[num];  //取得COMx中要显示的值;

    // my_printf(com_buf,lcd_com_buf[0],lcd_com_buf[1],lcd_com_buf[2],lcd_com_buf[3],lcd_com_buf[4],lcd_com_buf[5]);
    if (com_data & BIT(0)){
        SEG0_CLR();
        if(num % 2){
            SEG0_SET_L();
        }
        else{
            SEG0_SET_H();
        }
    }

    if (com_data & BIT(1)){
        SEG1_CLR();
        if(num % 2){
            SEG1_SET_L();
        }
        else{
            SEG1_SET_H();
        }
    }

    if (com_data & BIT(2)){
        SEG2_CLR();
        if(num % 2){
            SEG2_SET_L();
        }
        else{
            SEG2_SET_H();
        }
    }

    if (com_data & BIT(3)){
        SEG3_CLR();
        if(num % 2){
            SEG3_SET_L();
        }
        else{
            SEG3_SET_H();
        }
    }

    if (com_data & BIT(4)){
        SEG4_CLR();
        if(num % 2){
            SEG4_SET_L();
        }
        else{
            SEG4_SET_H();
        }
    }

    if (com_data & BIT(5)){
        SEG5_CLR();
        if(num % 2){
            SEG5_SET_L();
        }
        else{
            SEG5_SET_H();
        }
    }

    if (com_data & BIT(6)){
        SEG6_CLR();
        if(num % 2){
            SEG6_SET_L();
        }
        else{
            SEG6_SET_H();
        }
    }

    if (com_data & BIT(7)){
        SEG7_CLR();
        if(num % 2){
            SEG7_SET_L();
        }
        else{
            SEG7_SET_H();
        }
    }

    if (com_data & BIT(8)){
        SEG8_CLR();
        if(num % 2){
            SEG8_SET_L();
        }
        else{
            SEG8_SET_H();
        }
    }

    if (com_data & BIT(9)){
        SEG9_CLR();
        if(num % 2){
            SEG9_SET_L();
        }
        else{
            SEG9_SET_H();
        }
    }

    if (com_data & BIT(10)){
        SEG10_CLR();
        if(num % 2){
            SEG10_SET_L();
        }
        else{
            SEG10_SET_H();
        }
    }

    if (com_data & BIT(11)){
        SEG11_CLR();
        if(num % 2){
            SEG11_SET_L();
        }
        else{
            SEG11_SET_H();
        }
    }

    if (com_data & BIT(12)){
        SEG12_CLR();
        if(num % 2){
            SEG12_SET_L();
        }
        else{
            SEG12_SET_H();
        }
    }

    if (com_data & BIT(13)){
        SEG13_CLR();
        if(num % 2){
            SEG13_SET_L();
        }
        else{
            SEG13_SET_H();
        }
    }

    if (com_data & BIT(14)){
        SEG14_CLR();
        if(num % 2){
            SEG14_SET_L();
        }
        else{
            SEG14_SET_H();
        }
    }

    if (com_data & BIT(15)){
        SEG15_CLR();
        if(num % 2){
            SEG15_SET_L();
        }
        else{
            SEG15_SET_H();
        }
    }
}

//lcd 4com+16seg io扫描函数（1/3bias）
AT(.com_text.lcd.scan)
void lcd_4c16s_scan(void)
{
    static u8 com_num = 0;

    if (lcdseg_cb.disp_en == 0) {
        return;
    }

    com_num++;
    if (com_num > 7)
    {
        com_num = 0;
    }
    COM1_CLR();
    COM2_CLR();
    COM3_CLR();
    COM4_CLR();
    if (0 == com_num){
        COM1_SET_L();
        COM2_SET_BIAS2_3();
        COM3_SET_BIAS2_3();
        COM4_SET_BIAS2_3();
        SEG_ALL_SET_BIAS1_3();
    }
    else if (1 == com_num){
        COM1_SET_H();
        COM2_SET_BIAS1_3();
        COM3_SET_BIAS1_3();
        COM4_SET_BIAS1_3();
        SEG_ALL_SET_BIAS2_3();
    }
    else if (2 == com_num){
        COM1_SET_BIAS2_3();
        COM2_SET_L();
        COM3_SET_BIAS2_3();
        COM4_SET_BIAS2_3();
        SEG_ALL_SET_BIAS1_3();
    }
    else if (3 == com_num){
        COM1_SET_BIAS1_3();
        COM2_SET_H();
        COM3_SET_BIAS1_3();
        COM4_SET_BIAS1_3();
        SEG_ALL_SET_BIAS2_3();
    }
    else if (4 == com_num){
        COM1_SET_BIAS2_3();
        COM2_SET_BIAS2_3();
        COM3_SET_L();
        COM4_SET_BIAS2_3();
        SEG_ALL_SET_BIAS1_3();
    }
    else if (5 == com_num){
        COM1_SET_BIAS1_3();
        COM2_SET_BIAS1_3();
        COM3_SET_H();
        COM4_SET_BIAS1_3();
        SEG_ALL_SET_BIAS2_3();
    }
    else if (6 == com_num){
        COM1_SET_BIAS2_3();
        COM2_SET_BIAS2_3();
        COM3_SET_BIAS2_3();
        COM4_SET_L();
        SEG_ALL_SET_BIAS1_3();
    }
    else if (7 == com_num){
        COM1_SET_BIAS1_3();
        COM2_SET_BIAS1_3();
        COM3_SET_BIAS1_3();
        COM4_SET_H();
        SEG_ALL_SET_BIAS2_3();
    }
    lcd_4c16s_val_set(com_num);
}

AT(.lcd_text.lcd)
void lcd_4c16s_init(void)
{
    GPIOBDE |= BIT(0);
    GPIOBDIR &= ~BIT(0);
    GPIOBFEN |= BIT(0);

    GPIOBDE |= BIT(1);
    GPIOBDIR &= ~BIT(1);
    GPIOBFEN |= BIT(1);

    GPIOBDE |= BIT(2);
    GPIOBDIR &= ~BIT(2);
    GPIOBFEN |= BIT(2);

    GPIOBDE |= BIT(3);
    GPIOBDIR &= ~BIT(3);
    GPIOBFEN |= BIT(3);

    GPIOBDE |= BIT(4);
    GPIOBDIR &= ~BIT(4);
    GPIOBFEN |= BIT(4);
    // LCDDRVCON |= BIT(17);// channel se1

    GPIOBDE |= BIT(5);
    GPIOBDIR &= ~BIT(5);
    GPIOBFEN |= BIT(5);

    GPIOBDE |= BIT(6);
    GPIOBDIR &= ~BIT(6);
    GPIOBFEN |= BIT(6);

    GPIOADE |= BIT(0);
    GPIOADIR &= ~BIT(0);
    GPIOAFEN |= BIT(0);

    GPIOADE |= BIT(1);
    GPIOADIR &= ~BIT(1);
    GPIOAFEN |= BIT(1);

    GPIOADE |= BIT(2);
    GPIOADIR &= ~BIT(2);
    GPIOAFEN |= BIT(2);

    GPIOADE |= BIT(3);
    GPIOADIR &= ~BIT(3);
    GPIOAFEN |= BIT(3);

    GPIOADE |= BIT(4);
    GPIOADIR &= ~BIT(4);
    GPIOAFEN |= BIT(4);

    GPIOADE |= BIT(5);
    GPIOADIR &= ~BIT(5);
    GPIOAFEN |= BIT(5);

    GPIOADE |= BIT(6);
    GPIOADIR &= ~BIT(6);
    GPIOAFEN |= BIT(6);

    GPIOADE |= BIT(10);
    GPIOADIR &= ~BIT(10);
    GPIOAFEN |= BIT(10);

    GPIOADE |= BIT(11);
    GPIOADIR &= ~BIT(11);
    GPIOAFEN |= BIT(11);

    GPIOADE |= BIT(12);
    GPIOADIR &= ~BIT(12);
    GPIOAFEN |= BIT(12);

    GPIOADE |= BIT(13);
    GPIOADIR &= ~BIT(13);
    GPIOAFEN |= BIT(13);

    GPIOADE |= BIT(14);
    GPIOADIR &= ~BIT(14);
    GPIOAFEN |= BIT(14);

    GPIOADE |= BIT(15);
    GPIOADIR &= ~BIT(15);
    GPIOAFEN |= BIT(15);

    LCDDRVCON |= BIT(31);// lcddrv enable
    LCDDRVCON &= ~BIT(30);  // 1/3bias
    // LCDDRVCON = (LCDDRVCON & ~0xffffff);
    LCDDRVCON |= 0xffffff;
    memset(lcd_com_buf, 0, sizeof(lcd_com_buf));
}

AT(.lcd_text.lcd)
void lcd_4c16s_dis_test(void)
{
    static u32 lcd_tx_tick = 0;
    static u32 tmp_cnt = 0;
    static u8 icon_disp_cnt = 0;

    if(tick_check_expire(lcd_tx_tick, 1000)){  //1ms
        tmp_cnt++;
        lcd_tx_tick = tick_get();
        icon_disp_cnt++;
        lcdseg_cb.disp_en = 0;
        memset(lcd_com_buf, 0, sizeof(lcd_com_buf));
        lcdseg_cb.disp_en = 1;
    }
    
    if(icon_disp_cnt == 1){
        lcd_4c16s_dis_icon(DISP_LY);
        lcd_4c16s_dis_icon(DISP_KPA);
        lcd_4c16s_dis_icon(DISP_POINT_1);
    }
    else if(icon_disp_cnt == 2){
        lcd_4c16s_dis_icon(DISP_MMHG);
        lcd_4c16s_dis_icon(DISP_POINT_2);
        lcd_4c16s_dis_icon(DISP_AVG);
        lcd_4c16s_dis_icon(DISP_PERSON1);
    }
    else if(icon_disp_cnt == 3){
        lcd_4c16s_dis_icon(DISP_PERSON2);
        lcd_4c16s_dis_icon(DISP_W6);
        lcd_4c16s_dis_icon(DISP_W5);
        lcd_4c16s_dis_icon(DISP_W4);
    }
    else if(icon_disp_cnt == 4){
        lcd_4c16s_dis_icon(DISP_BT);
        lcd_4c16s_dis_icon(DISP_T);
        lcd_4c16s_dis_icon(DISP_M);
    }
    else{
        icon_disp_cnt = 0;
    }

    lcd_4c16s_dis_number_1(tmp_cnt);
    lcd_4c16s_dis_number_2(tmp_cnt);

//单独io示例
    // WDT_DIS();
    // while(1){
    //     WDT_CLR();
    //     LCDDRVSEL1 = (LCDDRVSEL1 & ~(3<<2)) | 0<<2;   // 0 ouput 0
    //     delay_ms(5);
    //     LCDDRVSEL1 = (LCDDRVSEL1 & ~(3<<2)) | 1<<2;      // 1 output 1/3 vddio       0.75v(bias=1)      1.0v(bias=0)
    //     delay_ms(5);
    //     LCDDRVSEL1 = (LCDDRVSEL1 & ~(3<<2)) | 2<<2;   // 2 ouput 1/2(bias=1)or 1/3(bias=0)vddio        //1.5v(bias=1)     2.0v(bias=0)
    //     delay_ms(5);
    //     LCDDRVSEL1 |= 3<<2;                           // 3 output vddio               //3.15v
    //     delay_ms(5);
    // }

//全部io示例
    // for(int i = 0; i < 21; i++){
    //     LCDDRVCON = LCDDRVCON & ~0xffffff | BIT(i);
    //     if(i < 16){
    //         for(int j = 0; j < 4; j++){
    //             LCDDRVSEL0 = (j<<(i*2));
    //         }
    //     } else {
    //         for(int j = 0; j < 4; j++){
    //             LCDDRVSEL1 = (j<<((i-16)*2));
    //         }
    //     }
    // }

}

#endif
