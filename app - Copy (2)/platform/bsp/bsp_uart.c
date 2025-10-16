#include "include.h"
#if USER_UART0_EN

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

isr_t register_isr(int vector, isr_t isr);
uint8_t checksum_calculation(uint8_t *ptr, uint16_t start, uint16_t len);

S_UART_BUF sUartbuf AT(.uart_buf);//TX/RX
SUART_DECODE_PACKET sUartPacket AT(.uart_buf);

AT(.text.bsp.uart)
void bsp_uart_init(void)
{
    memset(&sUartbuf, 0, sizeof(sUartbuf));
    memset(&sUartPacket, 0, sizeof(sUartPacket));
    udet_init(1500000);
    register_uart0_isr_init(uart0_isr);
}

AT(.text.bsp.uart)
void udet_init(u32 baudrate)
{
    u32 baud = 0;
    baud = ((SYS_CLK + (baudrate / 2)) / baudrate) - 1;
    CLKGAT1 |= BIT(11);                             //enable uart0 clk
    UART0CON = 0;

#if (!UART_CROSSBAR)
#if UART0_DUPLEX_SEL
    #if (UART0_TX_PORT == UTX0MAP_PA6)
        GPIOADE  |=  BIT(6);
        GPIOAPU  |=  BIT(6);
        GPIOADIR &= ~BIT(6);
        GPIOAFEN |=  BIT(6);
    #elif (UART0_TX_PORT == UTX0MAP_PA0)
        GPIOADE  |=  BIT(0);
        GPIOAPU  |=  BIT(0);
        GPIOADIR &= ~BIT(0);
        GPIOAFEN |=  BIT(0);
    #elif (UART0_TX_PORT == UTX0MAP_PB9)
        GPIOBDE  |=  BIT(9);
        GPIOBPU  |=  BIT(9);
        GPIOBDIR &= ~BIT(9);
        GPIOBFEN |=  BIT(9);
    #elif (UART0_TX_PORT == UTX0MAP_PB1)
        GPIOBDE  |=  BIT(1);
        GPIOBPU  |=  BIT(1);
        GPIOBDIR &= ~BIT(1);
        GPIOBFEN |=  BIT(1);
    #elif (UART0_TX_PORT == UTX0MAP_PB2)
        GPIOBDE  |=  BIT(2);
        GPIOBPU  |=  BIT(2);
        GPIOBDIR &= ~BIT(2);
        GPIOBFEN |=  BIT(2);
    #elif (UART0_TX_PORT == UTX0MAP_PB0)
        GPIOBDE  |=  BIT(0);
        GPIOBPU  |=  BIT(0);
        GPIOBDIR &= ~BIT(0);
        GPIOBFEN |=  BIT(0);
    #endif

    #if (UART0_RX_PORT == URX0MAP_PA7)
        GPIOADE  |= BIT(7);
        GPIOAPU  |= BIT(7);
        GPIOADIR |= BIT(7);
        GPIOAFEN |= BIT(7);
    #elif (UART0_RX_PORT == URX0MAP_PA1)
        GPIOADE  |= BIT(1);
        GPIOAPU  |= BIT(1);
        GPIOADIR |= BIT(1);
        GPIOAFEN |= BIT(1);
    #elif (UART0_RX_PORT == URX0MAP_PB2)
        GPIOBDE  |= BIT(2);
        GPIOBPU  |= BIT(2);
        GPIOBDIR |= BIT(2);
        GPIOBFEN |= BIT(2);
    #elif (UART0_RX_PORT == URX0MAP_PB0)
        GPIOBDE  |= BIT(0);
        GPIOBPU  |= BIT(0);
        GPIOBDIR |= BIT(0);
        GPIOBFEN |= BIT(0);
    #endif
    FUNCMCON0 = UART0_TX_PORT | UART0_RX_PORT;
#else
    #if (UART0_TX_PORT == UTX0MAP_PA6)
        GPIOADE  |=  BIT(6);
        GPIOAPU  |=  BIT(6);
        GPIOADIR |=  BIT(6);
        GPIOAFEN |=  BIT(6);
    #elif (UART0_TX_PORT == UTX0MAP_PA0)
        GPIOADE  |=  BIT(0);
        GPIOAPU  |=  BIT(0);
        GPIOADIR |=  BIT(0);
        GPIOAFEN |=  BIT(0);
    #elif (UART0_TX_PORT == UTX0MAP_PB9)
        GPIOBDE  |=  BIT(9);
        GPIOBPU  |=  BIT(9);
        GPIOBDIR |=  BIT(9);
        GPIOBFEN |=  BIT(9);
    #elif (UART0_TX_PORT == UTX0MAP_PB1)
        GPIOBDE  |=  BIT(1);
        GPIOBPU  |=  BIT(1);
        GPIOBDIR |=  BIT(1);
        GPIOBFEN |=  BIT(1);
    #elif (UART0_TX_PORT == UTX0MAP_PB2)
        GPIOBDE  |=  BIT(2);
        GPIOBPU  |=  BIT(2);
        GPIOBDIR |=  BIT(2);
        GPIOBFEN |=  BIT(2);
    #elif (UART0_TX_PORT == UTX0MAP_PB0)
        GPIOBDE  |=  BIT(0);
        GPIOBPU  |=  BIT(0);
        GPIOBDIR |=  BIT(0);
        GPIOBFEN |=  BIT(0);
    #endif
    FUNCMCON0 =  UART0_TX_PORT | URX0MAP_TX;                //RX map to TX
#endif
#endif
    UART0BAUD = (baud << 16) | baud;

#if UART_CROSSBAR
    if(UART_CROSSBAR_IO > IO_PA15){
        GPIOBFEN |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOBDE  |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOBDIR |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOAPU  |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
    }else{
        GPIOAFEN |= BIT(UART_CROSSBAR_IO-1);
        GPIOADE  |= BIT(UART_CROSSBAR_IO-1);
        GPIOADIR |= BIT(UART_CROSSBAR_IO-1);
        GPIOAPU  |= BIT(UART_CROSSBAR_IO-1);
    }

#if UART_TX_OR_RX
    FUNCMCON0 = ((FUNCMCON0 & ~(0x7 << 12))  | (5 << 12) );                            //uart0 rx map to channel2, RX map to TX
    FUNCINCON = ((FUNCINCON & ~(0x3f << 16)) | ((UART_CROSSBAR_IO - 1) << 16));        //channel2_rx
#else
    FUNCOUTMCON = ((FUNCOUTMCON & ~(0x1f << 16)) | (UART_CROSSBAR_IO << 16));          //channel2_tx
    FUNCOUTCON  = ((FUNCOUTCON & ~(0xf << 16)) | (6 << 16));                           //function select uart0_tx
#endif
    UART0CON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);       //RX EN, One line, fix baud, 2 Stop bits, Uart EN
#else
    #if UART0_DUPLEX_SEL
        UART0CON = BIT(7) | BIT(5) | BIT(4) | BIT(0);            //RX EN, TX/RX separate, fix baud, 2 Stop bits, Uart EN
    #else
        UART0CON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);   //RX EN, One line, fix baud, 2 Stop bits, Uart EN
    #endif
#endif
    UART0CON = (UART0CON & ~(0xf<<16)) | (0x5<<16);
    UART0CON = (UART0CON & ~(0xf<<20)) | (0x5<<20);
    UART0CON = (UART0CON & ~(0xf<<24)) | (0x5<<24);
    UART0CPND = BIT(10) | BIT(15) | BIT(18);                     //CLR Pending
}

AT(.text.bsp.uart)
void register_uart0_isr_init(isr_t isr)
{
    UART0CON  |= BIT(2);                 //Receive int enable
    UART0CPND |= BIT(9);                 //clear RX Pending

    register_isr(IRQ_UART_VECTOR, isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
    PICEN |= BIT(IRQ_UART_VECTOR);
}

#if UART0_RX_TEST_EN
AT(.com_text.uartstr)
const char struart1[] = "R[%d]%X\n";
#endif
AT(.com_text.uart)
void uart0_isr(void)
{
    u8 data;
    if(UART0CON & BIT(9))
    {
        data = UART0DATA;
        // my_printf("data : %x\n",data);
        UART0CPND = BIT(9);//clear RX pending
        sUartbuf.decodeBuf[sUartbuf.w_cnt] = data;
        #if UART0_RX_TEST_EN
        my_printf(struart1, sUartbuf.w_cnt, sUartbuf.decodeBuf[sUartbuf.w_cnt]);
        #endif
        if((sUartbuf.w_cnt + 1) >= UART_RX_BUF_LEN){
            sUartbuf.w_cnt = 0;
        }else{
            sUartbuf.w_cnt++;
        }
    }
}

//***************************************************************************************//
//uart0获取1字节数据接口
AT(.com_text.uart)
u8 bsp_uart0_get_data(u8 *charData)
{
    if(sUartbuf.r_cnt != sUartbuf.w_cnt)
    {
        *charData = sUartbuf.decodeBuf[sUartbuf.r_cnt];
        TRACE("Read sUartbuf.decodeBuf[%d]: 0x%X\n", sUartbuf.r_cnt, sUartbuf.decodeBuf[sUartbuf.r_cnt]);
        if((sUartbuf.r_cnt + 1) >= UART_RX_BUF_LEN){
            sUartbuf.r_cnt = 0;
        }else{
            sUartbuf.r_cnt++;
        }
        return 1;
    }
    return 0;
}

//uart0发送1字节数据接口
AT(.com_text.uart)
void bsp_uart0_putchar(char charData)
{
    UART0DATA = charData;
    while(!(UART0CON & BIT(8)));
}

//uart0发送多字节数据接口
AT(.text.bsp.uart)
bool bsp_send_uart0_data(u8 *buf, u8 length)
{
    bool boRet = false;
    for(u8 i = 0; i < length; i++)
    {
        bsp_uart0_putchar(buf[i]);
        //TRACE("uartSendData: %02x\n", buf[i]);
    }
    boRet = true;
    return boRet;
}

//***************************************************************************************//
AT(.text.bsp.uart)
void bsp_uart_clear_decodeBuf(void)
{
    memset(&sUartbuf, 0, sizeof(sUartbuf));
}

AT(.text.huart)
bool parse_uart_cmd_data(u8 uartData)
{
    bool boDecodeSuccess = false;
    if(CMD_UHEAD_ONE == uartData){
        sUartPacket.getHeader1 = true;
        sUartPacket.decodeStep = FIND_UHEAD;
    }else{
        switch(sUartPacket.decodeStep)
        {
            case FIND_UHEAD:
            {
                TRACE("FIND_UHEAD\n");
                if(sUartPacket.getHeader1){
                    if(CMD_UHEAD_TWO == uartData){
                        sUartPacket.decodeStep = CONFIRM_UID;
                    }else{
                        sUartPacket.getHeader1 = false;
                        sUartPacket.decodeStep = SAVE_UDATA;
                        sUartPacket.DataBuf[sUartPacket.w_cnt++] = CMD_UHEAD_ONE;//0x55是数据的情况下
                        sUartPacket.DataBuf[sUartPacket.w_cnt++] = uartData;
                    }
                }else{
                    TRACE("header error\n");
                    memset(&sUartPacket, 0, sizeof(sUartPacket));
                }
            }break;
            case CONFIRM_UID:
            {
                if(CMD_UID == uartData){
                    sUartPacket.getIdentification = true;
                    sUartPacket.decodeStep = GET_ULENGTH;
                }else{
                    TRACE("identification error\n");
                    memset(&sUartPacket, 0, sizeof(sUartPacket));
                }
            }break;
            case GET_ULENGTH:
            {
                TRACE("GET_ULENGTH uartData: 0x%X\n", uartData);
                if(uartData <= UART_CMD_MAX_SIZE){
                    sUartPacket.uLength = uartData;
                    sUartPacket.decodeStep = SAVE_UDATA;
                }else{
                    TRACE("cmd length oversize\n");
                    memset(&sUartPacket, 0, sizeof(sUartPacket));
                }
            }break;
            case SAVE_UDATA:
            {
                sUartPacket.DataBuf[sUartPacket.w_cnt & 0x3f] = uartData;
                sUartPacket.w_cnt++;
                if(sUartPacket.w_cnt >= sUartPacket.uLength){
                    TRACE("SAVE_UDATA uartData: 0x%X\n", uartData);
                    sUartPacket.decodeStep = CAL_UCHECKSUM;
                }else{
                    TRACE("SAVE_UDATA uartData: 0x%X\n", uartData);
                }
            }break;
            case CAL_UCHECKSUM:
            {
                TRACE("CAL_UCHECKSUM\n");
                sUartPacket.checkSum = uartData;
                u8 tempCheckSum = checksum_calculation(sUartPacket.DataBuf, 0, sUartPacket.uLength);
                TRACE("tempCheckSum: 0x%X\n", tempCheckSum);
                if(tempCheckSum == sUartPacket.checkSum){
                    TRACE("checksum right\n");
                    sUartPacket.decodeStep = CMD_USEND;
                }else{
                    TRACE("checksum error\n");
                    memset(&sUartPacket, 0, sizeof(sUartPacket));
                }
            }break;
            case CMD_USEND:
            {
                TRACE("cmd step error\n");
                memset(&sUartPacket, 0, sizeof(sUartPacket));
            }break;
            default:
                break;
        }
    }
    if(CMD_USEND == sUartPacket.decodeStep){
        TRACE("CMD_USEND\n");

        switch(sUartPacket.DataBuf[0])
        {
            case SYS_VOLUME_CTRL://0~16级 或 0~30级 或 0~50级
                TRACE("SYS_VOLUME_CTRL\n");
                if(sUartPacket.DataBuf[1] != sUartPacket.u_set_sys_vol){
                    sUartPacket.u_set_sys_vol = sUartPacket.DataBuf[1];
                    if(sUartPacket.u_set_sys_vol ==  VOL_MAX){//最大音量级数在setting上配置
                        bsp_set_volume(VOL_MAX);
                    }else{
                        if(sUartPacket.u_set_sys_vol <= 50){
                            TRACE("vol set: %d\n", sUartPacket.DataBuf[1]);
                            bsp_set_volume(sUartPacket.DataBuf[1]);
                        }
                    }
                }
                break;
            default:
                break;
        }
        boDecodeSuccess = true;
    }
    if(boDecodeSuccess)
    {
        memset(&sUartPacket, 0, sizeof(sUartPacket));
    }
    return boDecodeSuccess;
}

#if UART0_TX_TEST_EN
static u32 uart_tx_tick = 0;
AT(.rodata.uart_cmd)
static const u8 uart_tx_cmd[] = {
    0x55,0x12,  //data---
    0x34,0x56,  //data---
    0x78,0xFF,  //data---
    0x88,0x44,  //data---
    0x22,0x11,  //data---
};

//uart发送测试
AT(.text.bsp.uart)
void uart_send_test(void)
{
    u8 uart_tx_buf[10];
    memcpy(uart_tx_buf, uart_tx_cmd, 10);
    // my_print_r(uart_tx_buf, 10);
    bsp_send_uart0_data(uart_tx_buf, 10);
}
#endif

//将uart收到的数据进行处理
AT(.text.uart)
void uart_deal_process(void)
{
    u8 hidCharData;
    while(bsp_uart0_get_data(&hidCharData)){
        WDT_CLR();
        parse_uart_cmd_data(hidCharData);
    }
    #if UART0_TX_TEST_EN
        if(tick_check_expire(uart_tx_tick, 100)){
            uart_tx_tick = tick_get();
            uart_send_test();
        }
    #endif
}
#endif

#if 0  //port cpt

#define EDGE_BIT                  BIT(14)

AT(.com_rodata.test)
const char test1[] = "a\n";
AT(.com_rodata.test)
const char test2[] = "b\n";

AT(.com_text.test1) FIQ
void lt_sa_edge_isr()
{
    if(WKUPEDG & BIT(23))//rise
    {
        WKUPCPND = BIT(23);
        my_printf(test1);
    }
    if(WKUPEDG & BIT(22))//fall
    {
        WKUPCPND = BIT(22);
        my_printf(test2);
    }
}

void rise_edge_init(void)
{
    PORTINTEDG &= ~EDGE_BIT;   //0：rise  1:fall
    WKUPEDG |= BIT(7);
}

void fall_edge_init(void)
{
    PORTINTEDG |= EDGE_BIT;   //0：rise  1:fall
    WKUPEDG |= BIT(6);
}

AT(.com_text.test1)
void port_capture_isr(void)
{
    sys_irq_init(IRQ_PORT_VECTOR, 1, lt_sa_edge_isr);  //注册中断
}

void port_edge_cpt_init(void)
{
    GPIOAFEN &= ~EDGE_BIT;    //IO初始化 PA14
    GPIOADE  |=  EDGE_BIT;
    GPIOADIR |=  EDGE_BIT;

    PORTINTEN |= EDGE_BIT;     //PORT中断使能

    // rise_edge_init();
    fall_edge_init();
    WKUPCPND = (BIT(22) | BIT(23));     //clear pending

    WKUPCON |= BIT(6) | BIT(7) | BIT(16);   //wakeup 6and7 enable, interrupt enable
    port_capture_isr();
}
#endif // 0

