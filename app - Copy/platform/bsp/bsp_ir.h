#ifndef _BSP_IR_H
#define _BSP_IR_H

#define TMR2_RCLK               3000            //xosc24m_div8 3M

///红外遥控器地址码
#define IR_ADDR_FF00      0xFF00
#define IR_ADDR_BF00      0xBF00
#define IR_ADDR_FD02      0xFD02
#define IR_ADDR_FE01      0xFE01
#define IR_ADDR_7F80      0x7F80
#define IR_ADDR_DF20      0xDF20        //test

#define IR_NULL           0xffff

extern const u8 ir_tbl_FF00[96];
extern const u8 ir_tbl_BF00[32];
extern const u8 ir_tbl_FD02[32];
extern const u8 ir_tbl_FE01[32];
extern const u8 ir_tbl_7F80[32];
extern const u8 ir_tbl_DF20[96];

//IRTXCON
#define IRTX_EN_SHF         0
#define IRTX_LOADIE_EN_SHF  1
#define IRTX_FRMIE_EN_SHF   2
#define IRTX_KST_SHF        3
#define IRTX_LOAD_PND_SHF   4
#define IRTX_FRM_PND_SHF    5
#define IRTX_INVERT_SHF     6
#define IRTX_CARR_EN        7
#define IRTX_CARR_DIV_SHF   8
#define IRTX_CARR_DUTY_SHF  16
#define IRTX_BASEPR_SHF     24

typedef struct {
    u16 cnt;                            //ir data bit counter
    u16 rpt_cnt;                        //ir repeat counter
    u16 addr;                           //address,  inverted address   Extended NEC: 16bits address
    u16 cmd;                            //command,  inverted command
    u16 RPTERR;
    u16 DATERR;
    u16 ONEERR;
    u16 ZEROERR;
    u16 TOPR;
    u32 tmrlast;
} ir_cb_t;

void irrx_hw_init(void);
void irrx_irq_init(void);
void irrx_sw_init(void);
void ir_key_clr(void);
u8 get_irkey(void);
#endif // _BSP_IR_H
