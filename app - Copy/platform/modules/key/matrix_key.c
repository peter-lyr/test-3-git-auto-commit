#include "include.h"
#include "matrix_key.h"

#if MAXTRIX_TRIGLE_KEYBOARD_EN

maxtrix_key_t maxtrix_key;

gpio_t *g0 = &maxtrix_key.gpio0;
gpio_t *g1 = &maxtrix_key.gpio1;
gpio_t *g2 = &maxtrix_key.gpio2;
gpio_t *g3 = &maxtrix_key.gpio3;
gpio_t *g4 = &maxtrix_key.gpio4;
gpio_t *g5 = &maxtrix_key.gpio5;
gpio_t *g6 = &maxtrix_key.gpio6;
gpio_t *g7 = &maxtrix_key.gpio7;

static u8 maxtrix_scan_flag = 1;

void maxtrix_key_gpio_input_dir(void)
{
    g0->sfr[GPIOxDIR]  |=  g0->pin;
    g1->sfr[GPIOxDIR]  |=  g1->pin;
    g2->sfr[GPIOxDIR]  |=  g2->pin;
    g3->sfr[GPIOxDIR]  |=  g3->pin;
    g4->sfr[GPIOxDIR]  |=  g4->pin;
    g5->sfr[GPIOxDIR]  |=  g5->pin;
    g6->sfr[GPIOxDIR]  |=  g6->pin;
    g7->sfr[GPIOxDIR]  |=  g7->pin;
}

void maxtrix_key_gpio_output_dir(void)
{
    g0->sfr[GPIOxDIR]  &=  ~g0->pin;
    g1->sfr[GPIOxDIR]  &=  ~g1->pin;
    g2->sfr[GPIOxDIR]  &=  ~g2->pin;
    g3->sfr[GPIOxDIR]  &=  ~g3->pin;
    g4->sfr[GPIOxDIR]  &=  ~g4->pin;
    g5->sfr[GPIOxDIR]  &=  ~g5->pin;
    g6->sfr[GPIOxDIR]  &=  ~g6->pin;
    g7->sfr[GPIOxDIR]  &=  ~g7->pin;
}

void maxtrix_key_gpio_input_pushup(void)
{
    maxtrix_key_gpio_input_dir();

    g0->sfr[GPIOxPU]  |=  g0->pin;
    g1->sfr[GPIOxPU]  |=  g1->pin;
    g2->sfr[GPIOxPU]  |=  g2->pin;
    g3->sfr[GPIOxPU]  |=  g3->pin;
    g4->sfr[GPIOxPU]  |=  g4->pin;
    g5->sfr[GPIOxPU]  |=  g5->pin;
    g6->sfr[GPIOxPU]  |=  g6->pin;
    g7->sfr[GPIOxPU]  |=  g7->pin;
}

void maxtrix_key_gpio_input_pushdown(void)
{
    maxtrix_key_gpio_input_dir();

    g0->sfr[GPIOxPD]  |=  g0->pin;
    g1->sfr[GPIOxPD]  |=  g1->pin;
    g2->sfr[GPIOxPD]  |=  g2->pin;
    g3->sfr[GPIOxPD]  |=  g3->pin;
    g4->sfr[GPIOxPD]  |=  g4->pin;
    g5->sfr[GPIOxPD]  |=  g5->pin;
    g6->sfr[GPIOxPD]  |=  g6->pin;
    g7->sfr[GPIOxPD]  |=  g7->pin;
}

void maxtrix_key_gpio_cancel_pushup(void)
{
    maxtrix_key_gpio_output_dir();

    g0->sfr[GPIOxPU]  &=  ~g0->pin;
    g1->sfr[GPIOxPU]  &=  ~g1->pin;
    g2->sfr[GPIOxPU]  &=  ~g2->pin;
    g3->sfr[GPIOxPU]  &=  ~g3->pin;
    g4->sfr[GPIOxPU]  &=  ~g4->pin;
    g5->sfr[GPIOxPU]  &=  ~g5->pin;
    g6->sfr[GPIOxPU]  &=  ~g6->pin;
    g7->sfr[GPIOxPU]  &=  ~g7->pin;
}

void maxtrix_key_gpio_cancel_pushdown(void)
{
    maxtrix_key_gpio_output_dir();

    g0->sfr[GPIOxPD]  &=  ~g0->pin;
    g1->sfr[GPIOxPD]  &=  ~g1->pin;
    g2->sfr[GPIOxPD]  &=  ~g2->pin;
    g3->sfr[GPIOxPD]  &=  ~g3->pin;
    g4->sfr[GPIOxPD]  &=  ~g4->pin;
    g5->sfr[GPIOxPD]  &=  ~g5->pin;
    g6->sfr[GPIOxPD]  &=  ~g6->pin;
    g7->sfr[GPIOxPD]  &=  ~g7->pin;
}

u8 maxtrix_triangle_scan(void)
{
    //先扫描右边I组，J组
    maxtrix_key_gpio_input_pushup();

    delay_us(5);
/*
    if(!(g0->sfr[GPIOx] & g0->pin)){                  //KI0
        while(!(g0->sfr[GPIOx] & g0->pin));
        my_printf("KI0\n");
    }
    if(!(g1->sfr[GPIOx] & g1->pin)){                  //KI1
        while(!(g1->sfr[GPIOx] & g1->pin));
        my_printf("KI1\n");
    }
    if(!(g2->sfr[GPIOx] & g2->pin)){                  //KI2
        while(!(g2->sfr[GPIOx] & g2->pin));
        my_printf("KI2\n");
    }
    if(!(g3->sfr[GPIOx] & g3->pin)){                  //KI3
        while(!(g3->sfr[GPIOx] & g3->pin));
        my_printf("KI3\n");
    }
    if(!(g4->sfr[GPIOx] & g4->pin)){                  //KI4
        while(!(g4->sfr[GPIOx] & g4->pin));
        my_printf("KI4\n");
    }
    if(!(g5->sfr[GPIOx] & g5->pin)){                  //KI5
        while(!(g5->sfr[GPIOx] & g5->pin));
        my_printf("KI5\n");
    }
    if(!(g6->sfr[GPIOx] & g6->pin)){                  //KI6
        while(!(g6->sfr[GPIOx] & g6->pin));
        my_printf("KI6\n");
    }
    if(!(g7->sfr[GPIOx] & g7->pin)){                  //KI7
        while(!(g7->sfr[GPIOx] & g7->pin));
        my_printf("KI7\n");
    }

    delay_us(5);
*/
    if(((g0->sfr[GPIOx]&0xff) == 0xfe) || ((g1->sfr[GPIOx]&0xff) == 0xfd) || ((g2->sfr[GPIOx]&0xff) == 0xfb) || ((g3->sfr[GPIOx]&0xff) == 0xf7) || ((g4->sfr[GPIOx]&0xff) == 0xef) || ((g5->sfr[GPIOx]&0xff) == 0xdf) || ((g6->sfr[GPIOx]&0xff) == 0xbf) || ((g7->sfr[GPIOx]&0xff) == 0x7f)){
        maxtrix_scan_flag = 0;
    }else{
        maxtrix_scan_flag = 1;
    }
    maxtrix_key_gpio_cancel_pushup();
    delay_us(5);
    maxtrix_key_gpio_input_pushdown();
    delay_us(5);
/*
    if(g0->sfr[GPIOx] & g0->pin){                  //KJ0
        //while(g0->sfr[GPIOx] & g0->pin);
        my_printf("KJ0\n");
    }
    if(g1->sfr[GPIOx] & g1->pin){                  //KJ1
        //while(g1->sfr[GPIOx] & g1->pin);
        my_printf("KJ1\n");
    }
    if(g2->sfr[GPIOx] & g2->pin){                  //KJ2
        //while(g2->sfr[GPIOx] & g2->pin);
        my_printf("KJ2\n");
    }
    if(g3->sfr[GPIOx] & g3->pin){                  //KJ3
        //while(g3->sfr[GPIOx] & g3->pin);
        my_printf("KJ3\n");
    }
    if(g4->sfr[GPIOx] & g4->pin){                  //KJ4
        //while(g4->sfr[GPIOx] & g4->pin);
        my_printf("KJ4\n");
    }
    if(g5->sfr[GPIOx] & g5->pin){                  //KJ5
        //while(g5->sfr[GPIOx] & g5->pin);
        my_printf("KJ5\n");
    }
    if(g6->sfr[GPIOx] & g6->pin){                  //KJ6
        //while(g6->sfr[GPIOx] & g6->pin);
        my_printf("KJ6\n");
    }
    if(g7->sfr[GPIOx] & g7->pin){                  //KJ7
        //while(g7->sfr[GPIOx] & g7->pin);
        my_printf("KJ7\n");
    }
*/
    maxtrix_key_gpio_cancel_pushdown();

    delay_us(5);

    if(1){
        //再扫左边按键键盘
        //第一行输入上拉，第二行输出拉低
        maxtrix_key_gpio_input_pushup();

        g0->sfr[GPIOxDIR] &= ~g0->pin;
        g0->sfr[GPIOxCLR] = g0->pin;

        delay_us(5);

        if(!(g1->sfr[GPIOx] & g1->pin)){        //KA1
            //value = KEY_1;
            while(!(g1->sfr[GPIOx] & g1->pin));
            my_printf("KA1\n");
        }
        if(!(g2->sfr[GPIOx] & g2->pin)){        //KA2
            //value = KEY_VOL_DOWN;
            while(!(g2->sfr[GPIOx] & g2->pin));
            my_printf("KA2\n");
        }
        if(!(g3->sfr[GPIOx] & g3->pin)){        //KA3
            //value = KEY_VOL_UP;
            while(!(g3->sfr[GPIOx] & g3->pin));
            my_printf("KA3\n");
        }
        if(!(g4->sfr[GPIOx] & g4->pin)){        //KA4
            while(!(g4->sfr[GPIOx] & g4->pin));
            my_printf("KA4\n");
        }
        if(!(g5->sfr[GPIOx] & g5->pin)){        //KA5
            while(!(g5->sfr[GPIOx] & g5->pin));
            my_printf("KA5\n");
        }
        if(!(g6->sfr[GPIOx] & g6->pin)){        //KA6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KA6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){        //KA7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KA7\n");
        }

        //第二行输入上拉，第三行输出拉低
        maxtrix_key_gpio_input_pushup();

        g1->sfr[GPIOxDIR] &= ~g1->pin;
        g1->sfr[GPIOxCLR] = g1->pin;

        delay_us(5);

        if(!(g2->sfr[GPIOx] & g2->pin)){                  //KB2
            while(!(g2->sfr[GPIOx] & g2->pin));
            my_printf("KB2\n");
        }
        if(!(g3->sfr[GPIOx] & g3->pin)){                  //KB3
            while(!(g3->sfr[GPIOx] & g3->pin));
            my_printf("KB3\n");
        }
        if(!(g4->sfr[GPIOx] & g4->pin)){                  //KB4
            while(!(g4->sfr[GPIOx] & g4->pin));
            my_printf("KB4\n");
        }
        if(!(g5->sfr[GPIOx] & g5->pin)){                  //KB5
            while(!(g5->sfr[GPIOx] & g5->pin));
            my_printf("KB5\n");
        }
        if(!(g6->sfr[GPIOx] & g6->pin)){                  //KB6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KB6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KB7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KB7\n");
        }

        //第三行输入上拉，第四行输出拉低

        maxtrix_key_gpio_input_pushup();

        g2->sfr[GPIOxDIR] &= ~g2->pin;
        g2->sfr[GPIOxCLR] = g2->pin;

        delay_us(5);

        if(!(g3->sfr[GPIOx] & g3->pin)){                  //KC3
            while(!(g3->sfr[GPIOx] & g3->pin));
            my_printf("KC3\n");
        }
        if(!(g4->sfr[GPIOx] & g4->pin)){                  //KC4
            while(!(g4->sfr[GPIOx] & g4->pin));
            my_printf("KC4\n");
        }
        if(!(g5->sfr[GPIOx] & g5->pin)){                  //KC5
            while(!(g5->sfr[GPIOx] & g5->pin));
            my_printf("KC5\n");
        }
        if(!(g6->sfr[GPIOx] & g6->pin)){                  //KC6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KC6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KC7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KC7\n");
        }

        //第四行输入上拉，第五行输出拉低
        maxtrix_key_gpio_input_pushup();

        g3->sfr[GPIOxDIR] &= ~g3->pin;
        g3->sfr[GPIOxCLR] = g3->pin;

        delay_us(5);

        if(!(g4->sfr[GPIOx] & g4->pin)){                  //KD4
            while(!(g4->sfr[GPIOx] & g4->pin));
            my_printf("KD4\n");
        }
        if(!(g5->sfr[GPIOx] & g5->pin)){                  //KD5
            while(!(g5->sfr[GPIOx] & g5->pin));
            my_printf("KD5\n");
        }
        if(!(g6->sfr[GPIOx] & g6->pin)){                  //KD6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KD6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KD7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KD7\n");
        }

        //第五行输入上拉，第六行输出拉低
        maxtrix_key_gpio_input_pushup();

        g4->sfr[GPIOxDIR] &= ~g4->pin;
        g4->sfr[GPIOxCLR] = g4->pin;

        delay_us(5);

        if(!(g5->sfr[GPIOx] & g5->pin)){                  //KE5
            while(!(g5->sfr[GPIOx] & g5->pin));
            my_printf("KE5\n");
        }
        if(!(g6->sfr[GPIOx] & g6->pin)){                  //KE6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KE6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KE7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KE7\n");
        }

        //第六行输入上拉，第七行输出拉低
        maxtrix_key_gpio_input_pushup();

        g5->sfr[GPIOxDIR] &= ~g5->pin;
        g5->sfr[GPIOxCLR] = g5->pin;

        delay_us(5);

        if(!(g6->sfr[GPIOx] & g6->pin)){                  //KF6
            while(!(g6->sfr[GPIOx] & g6->pin));
            my_printf("KF6\n");
        }
        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KF7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KF7\n");
        }

        //第七行输入上拉，第八行输出拉低
        maxtrix_key_gpio_input_pushup();

        g6->sfr[GPIOxDIR] &= ~g6->pin;
        g6->sfr[GPIOxCLR] = g6->pin;

        delay_us(5);

        if(!(g7->sfr[GPIOx] & g7->pin)){                  //KG7
            while(!(g7->sfr[GPIOx] & g7->pin));
            my_printf("KG7\n");
        }
        delay_us(5);
    }

    return 0;
}

void maxtrix_triangle_scan_process(void)
{
    u8 key_val = 0;
    if(key_val == 0) {
        key_val = maxtrix_triangle_scan();
    }
    msg_enqueue(key_val);
}

void maxtrix_triangle_key_init(void)
{
    gpio_t *g0 = &maxtrix_key.gpio0;
    gpio_cfg_init(g0, MAX_KEY_0);

    gpio_t *g1 = &maxtrix_key.gpio1;
    gpio_cfg_init(g1, MAX_KEY_1);

    gpio_t *g2 = &maxtrix_key.gpio2;
    gpio_cfg_init(g2, MAX_KEY_2);

    gpio_t *g3 = &maxtrix_key.gpio3;
    gpio_cfg_init(g3, MAX_KEY_3);

    gpio_t *g4 = &maxtrix_key.gpio4;
    gpio_cfg_init(g4, MAX_KEY_4);

    gpio_t *g5 = &maxtrix_key.gpio5;
    gpio_cfg_init(g5, MAX_KEY_5);

    gpio_t *g6 = &maxtrix_key.gpio6;
    gpio_cfg_init(g6, MAX_KEY_6);

    gpio_t *g7 = &maxtrix_key.gpio7;
    gpio_cfg_init(g7, MAX_KEY_7);

    if (g0->sfr) {
        g0->sfr[GPIOxFEN] &= ~g0->pin;
        g0->sfr[GPIOxDE]  |= g0->pin;
        g0->sfr[GPIOxDIR] &=~g0->pin;
    }

    if (g1->sfr) {
        g1->sfr[GPIOxFEN] &= ~g1->pin;
        g1->sfr[GPIOxDE]  |= g1->pin;
        g1->sfr[GPIOxDIR] &=~g1->pin;
    }

    if (g2->sfr) {
        g2->sfr[GPIOxFEN] &= ~g2->pin;
        g2->sfr[GPIOxDE]  |= g2->pin;
        g2->sfr[GPIOxDIR] &=~g2->pin;
    }

    if (g3->sfr) {
        g3->sfr[GPIOxFEN] &= ~g3->pin;
        g3->sfr[GPIOxDE]  |= g3->pin;
        g3->sfr[GPIOxDIR] &=~g3->pin;
    }

    if (g4->sfr) {
        g4->sfr[GPIOxFEN] &= ~g4->pin;
        g4->sfr[GPIOxDE]  |= g4->pin;
        g4->sfr[GPIOxDIR] &=~g4->pin;
    }

    if (g5->sfr) {
        g5->sfr[GPIOxFEN] &= ~g5->pin;
        g5->sfr[GPIOxDE]  |= g5->pin;
        g5->sfr[GPIOxDIR] &=~g5->pin;
    }

    if (g6->sfr) {
        g6->sfr[GPIOxFEN] &= ~g5->pin;
        g6->sfr[GPIOxDE]  |= g6->pin;
        g6->sfr[GPIOxDIR] &=~g6->pin;
    }

    if (g7->sfr) {
        g7->sfr[GPIOxFEN] &= ~g7->pin;
        g7->sfr[GPIOxDE]  |= g7->pin;
        g7->sfr[GPIOxDIR] &=~g7->pin;
    }
    printf("%s\n",__func__);
}
#endif//MAXTRIX_TRIGLE_KEYBOARD_EN


#if MATRIX_XY_KEY_EN
AT(.com_rodata.matrix)
const u8 x_port_group[] = MATRIX_X_PORT_GROUP;
AT(.com_rodata.matrix)
const u8 y_port_group[] = MATRIX_Y_PORT_GROUP;

static volatile u8 ykey_out_num AT(.matrix_buf);                                    //使能Y轴输出记录
u8 xykey_tbl[ARRAY_NUM(x_port_group)][ARRAY_NUM(y_port_group)] AT(.matrix_buf);     //矩阵按键adc阈值记录
u16 xykey_map[ARRAY_NUM(y_port_group)] AT(.matrix_buf);                             //矩阵按键按键状态表

AT(.com_text.matrix_key)
void set_matrix_y_chk(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR &= ~(BIT(io_num-1-IO_PA15));
        GPIOBCLR  =  (BIT(io_num-1-IO_PA15));

    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR &= ~(BIT(io_num-1));
        GPIOACLR  =  (BIT(io_num-1));
    }
}

AT(.com_text.matrix_key)
void set_matrix_y_reset(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBFEN &= ~(BIT(io_num-1-IO_PA15));
        GPIOBDE  |=  (BIT(io_num-1-IO_PA15));
        GPIOBDIR &= ~(BIT(io_num-1-IO_PA15));
        GPIOBSET  =  (BIT(io_num-1-IO_PA15));

    }else{
        GPIOAFEN &= ~(BIT(io_num-1));
        GPIOADE  |=  (BIT(io_num-1));
        GPIOADIR &= ~(BIT(io_num-1));
        GPIOASET  =  (BIT(io_num-1));
    }
}

AT(.com_text.matrix_key)
void matrix_key_xy_set(u8 ykey_out_num)
{
    if(ykey_out_num >= ARRAY_NUM(y_port_group)){
        return;
    }
    for(u32 i = 0; i < ARRAY_NUM(y_port_group); i++){
        if(i == ykey_out_num){
            set_matrix_y_chk(y_port_group[i]);
        }else{
            set_matrix_y_reset(y_port_group[i]);
        }
    }
}

AT(.com_text.matrix_key)
void matrix_xy_key_scan_kick(void)
{
    ykey_out_num = 0;
    matrix_key_xy_set(ykey_out_num);
}

AT(.com_text.matrix_key)
void matrix_xy_key_scan(void)
{
    if(ykey_out_num < ARRAY_NUM(y_port_group)){
        for(u32 i = 0; i < (ARRAY_NUM(x_port_group)); i++){
            u16 adc_val = (u16)(saradc_cb.sfr[x_port_group[i]]);
            if(adc_val < MATRIX_KEY_ADC_TRIGGER){
                if(xykey_tbl[i][ykey_out_num] < MATRIX_KEY_BASE){
                    xykey_tbl[i][ykey_out_num] = MATRIX_KEY_BASE;
                }else if(xykey_tbl[i][ykey_out_num] < (MATRIX_KEY_BASE + MATRIX_KEY_PRESS_TIMES)){
                    xykey_tbl[i][ykey_out_num]++;
                }
            }else{
                if(xykey_tbl[i][ykey_out_num] > MATRIX_KEY_BASE){
                    xykey_tbl[i][ykey_out_num] = MATRIX_KEY_BASE;
                }else if(xykey_tbl[i][ykey_out_num] > (MATRIX_KEY_BASE - MATRIX_KEY_RLS_TIMES)){
                    xykey_tbl[i][ykey_out_num]--;
                }
            }
        }
        ykey_out_num++;
        matrix_key_xy_set(ykey_out_num);
        delay_us(10);
    }else{
        matrix_xy_key_scan_kick();
    }
}

AT(.com_text.matrix_key)
void get_matrix_xy_key(void)
{
    u8 key_num = NO_KEY;
    u16 key_return = 0;
    if(ykey_out_num < ARRAY_NUM(y_port_group)){
        return;
    }
    for(u32 i = 0; i < ARRAY_NUM(x_port_group); i++){
        for(u32 k = 0; k < ARRAY_NUM(y_port_group); k++){
            key_num = KEY_MAT_1 + (k * ARRAY_NUM(x_port_group) + i);
            if(xykey_tbl[i][k] == (MATRIX_KEY_BASE + MATRIX_KEY_PRESS_TIMES)){
                if(((xykey_map[k] & BIT(i)) >> i) == 0){
                    xykey_map[k] |= BIT(i);
                    key_return = key_num | KEY_SHORT;           //按键第一次按下
                }else{
                    key_return = key_num | KEY_HOLD;            //按键hold住
                }
            }else if(xykey_tbl[i][k] == (MATRIX_KEY_BASE - MATRIX_KEY_PRESS_TIMES)){
                if(((xykey_map[k] & BIT(i)) >> i) == 1){
                    xykey_map[k] &= ~BIT(i);
                    key_return = key_num | KEY_SHORT_UP;        //按键松开
                }
            }
            if(key_return != 0){
                msg_enqueue(key_return);
                key_return = 0;
            }
        }
    }
}

void matrix_x_adc_init(u8 x_adc_ch)
{
    u8 io_num;
    gpio_t gpio;
    io_num = get_adc_gpio_num(x_adc_ch);
    gpio_cfg_init(&gpio, io_num);
    if (gpio.sfr) {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE]  |=  BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |=  BIT(gpio.num);
        gpio.sfr[GPIOxPU]  |=  BIT(gpio.num);
    }
	saradc_set_channel(BIT(x_adc_ch));
}

void matrix_y_io_init(u8 io_num)
{
    gpio_t gpio;
    gpio_cfg_init(&gpio, io_num);
    if (gpio.sfr) {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE]  |=  BIT(gpio.num);
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxSET]  =  BIT(gpio.num);
    }
}

//X输入,Y输出1
void matrix_xy_key_init(void)
{
    u32 i;
    memset(xykey_map, 0, sizeof(xykey_map));
    memset(xykey_tbl, 0x80, sizeof(xykey_tbl));

    //X init
    for(i = 0; i < ARRAY_NUM(x_port_group); i++){
        matrix_x_adc_init(x_port_group[i]);
    }
    //Y init
    for(i = 0; i < ARRAY_NUM(y_port_group); i++){
        matrix_y_io_init(y_port_group[i]);
    }
    matrix_xy_key_scan_kick();
}
#endif
