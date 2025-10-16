#include "include.h"

//rx部分
#if IRRX_SW_EN
ir_cb_t ir_cb AT(.buf.ir.cb);

AT(.com_text.str)
const char ir_str[] = "get_irkey: %04x, %04x %x\n";

AT(.com_text.ir)
u8 get_irkey(void)
{
    u8 key_val = NO_KEY;
    if (ir_cb.cnt != 32) {
        return NO_KEY;
    }

    if (ir_cb.addr == IR_NULL && ir_cb.cmd == IR_NULL)
        return 0xff;

#if IR_ADDR_DF20_EN
    if (ir_cb.addr == IR_ADDR_DF20) {
        key_val = ir_tbl_DF20[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_FF00_EN
    if (ir_cb.addr == IR_ADDR_FF00) {
        key_val = ir_tbl_FF00[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_BF00_EN
    if (ir_cb.addr == IR_ADDR_BF00) {
        key_val = ir_tbl_BF00[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_BF00_EN

#if IR_ADDR_FD02_EN
    if (ir_cb.addr == IR_ADDR_FD02) {
        key_val = ir_tbl_FD02[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FD02_EN

#if IR_ADDR_FE01_EN
    if (ir_cb.addr == IR_ADDR_FE01) {
        key_val = ir_tbl_FE01[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FE01_EN

#if IR_ADDR_7F80_EN
    if (ir_cb.addr == IR_ADDR_7F80) {
        key_val = ir_tbl_7F80[ir_cb.cmd & 0xff];
    }
#endif

    my_printf(ir_str, ir_cb.cmd, ir_cb.addr, key_val);
    return key_val;
}

AT(.com_text.isr.timer)
void timer2_isr(void)
{
    u32 tmrcnt;

    if (TMR2CON & BIT(10)) {
        //timer1 capture interrupt
        TMR2CNT  = TMR2CNT - TMR2CPT;
        tmrcnt = TMR2CPT;
        TMR2CPND = BIT(10);
        tmrcnt = tmrcnt /TMR2_RCLK;                //convert to ms
    } else if (TMR2CON & BIT(9)){
        //timer1 overflow interrupt
        TMR2CPND = BIT(9);
        tmrcnt = 110;                   //110ms overflow
    } else {
        return;
    }

    //processing repeat code
    if (ir_cb.cnt == 32) {
        if ((tmrcnt >= 10) && (tmrcnt <= 12)) {
            //repeat code is simply 9ms+2.25ms
            ir_cb.rpt_cnt = 0;
        } else {
            ir_cb.rpt_cnt += tmrcnt;
            if (ir_cb.rpt_cnt > 108) {
                ir_cb.rpt_cnt = 0;
                ir_cb.cnt = 0;          //ir key release
            }
        }
        return;
    } else if ((tmrcnt > 7) || (tmrcnt == 0)) {     //A message is started by a 9ms AGC burst
        ir_cb.rpt_cnt = 0;
        ir_cb.cnt = 0;                  //ir key message started
        return;
    }

    ir_cb.cmd >>= 1;
    ir_cb.cnt++;
    if (tmrcnt == 2) {                  //Bit time of 1.125ms(0) or 2.25ms(1)
        ir_cb.cmd |= 0x8000;
    }

    if (ir_cb.cnt == 16) {
        ir_cb.addr = ir_cb.cmd;         //save address data
    } else if (ir_cb.cnt == 32) {
        //got ir key message
        if ((u8)ir_cb.cmd > 96) {
            ir_cb.cmd = NO_KEY;
        }
//        printf("ir: %04x, %02x\n",ir_cb.addr, (u8)ir_cb.cmd);
    }
}

AT(.text.bsp.ir)
void ir_key_clr(void)
{
    ir_cb.cnt = 0;
}

AT(.text.bsp.ir)
void timer2_init(void)
{
    sys_irq_init(IRQ_TMR2_VECTOR, 0, timer2_isr);
    TMR2CON = 0;
	TMR2CNT = 0;
	TMR2PR  = TMR2_RCLK*110 - 1;                            //110ms Timer overflow interrupt
    TMR2CON = (2 << 1) | (3 << 4) | (2 << 14) | BIT(8) | BIT(7) |  BIT(0) ;       //timer2 clk = xosc24m / 8
    PICPR &= ~BIT(IRQ_TMR2_VECTOR);
	PICEN |= BIT(IRQ_TMR2_VECTOR);
}

void ir_filter_init(void)
{
    //clk
    CLKCON1 = (CLKCON1 & ~(0x3 << 30)) | (3 << 30);
    CLKGAT1 |= BIT(30);

    //mcp
    FUNCINCON = (FUNCINCON & ~(0x3f << 0)) | (1 << 0);                                     //PA1 TMR2CAP
    FUNCMCON0 = (FUNCMCON0 & ~(0xf << 28)) | (2 << 28) ;    //IRFOMAP TMR2

    //irflt
    IRFLTCON |= (10 << 8);
    IRFLTCON |= BIT(0);
}

AT(.text.bsp.ir)
void irrx_sw_init(void)
{
    my_printf("%s\n",__func__);
    IR_CAPTURE_PORT();
    FUNCMCON1 = (2 << 16);              //enable timer1 map
    FUNCINCON = (IRRX_MAPPING - 1);
    memset(&ir_cb, 0, sizeof(ir_cb));

#if IR_FILTER_EN
    ir_filter_init();
#endif // IR_FILTER_EN

    timer2_init();
}
#endif // IRRX_SW_EN


