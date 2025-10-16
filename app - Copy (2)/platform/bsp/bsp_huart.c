#include "include.h"

#if HUART_EN

const u8 huart_map_gpio_tbl[] = {
    IO_PA9, IO_PB3, IO_PB4, IO_PA12, IO_PA0, IO_PA7,
};

AT(.com_text.huart.tx_cb)
void huart_tx_done_cb(void)
{
#if HUART_DUMP_EN
    huart_dump_tx_done();
#endif

}

AT(.com_text.huart.rx_cb)
void huart_rx_done_cb(void)
{
#if HUART_EQ_DBG_EN
    if (bsp_eq_rx_done(eq_rx_buf)){
        return;
    }
#endif
}

void bsp_huart_init(void)
{
    huart_t huart0;
    u32 baud_rate = 1500000;
    u32 rx_io = IO_NONE, tx_io = IO_NONE;

    if (HUART_TX_MAP > HUART_TR_PA7 || HUART_RX_MAP > HUART_TR_G9) {
        return;
    }

    if (HUART_TX_MAP == HUART_TR_PA0 || HUART_RX_MAP == HUART_TR_PA0) {
        if (UART0_PRINTF_SEL == PRINTF_PA0) {
            FUNCMCON0 = (0x0f << 8) | (0xf << 12);
        }
    } else if (HUART_TX_MAP == HUART_TR_PA9 || HUART_RX_MAP == HUART_TR_PA9) {
        if (UART0_PRINTF_SEL == PRINTF_PA9) {
            FUNCMCON0 = (0x0f << 8) | (0xf << 12);
        }
    } else if (HUART_TX_MAP == HUART_TR_PB3 || HUART_RX_MAP == HUART_TR_PB3) {
        if (UART0_PRINTF_SEL == PRINTF_PB3) {
            FUNCMCON0 = (0x0f << 8) | (0xf << 12);
        }
    }
    tx_io = huart_map_gpio_tbl[HUART_TX_MAP];

    if (HUART_RX_MAP <= HUART_TR_PA7) {
        rx_io = huart_map_gpio_tbl[HUART_RX_MAP];
    } else {
        rx_io = HUART_CROSSBAR_IO;
    }
    if (HUART_RX_MAP >= HUART_TR_G8 && rx_io != IO_NONE) {
        if (HUART_RX_MAP == HUART_TR_G8) {
            CH2_FUNI_SEL((rx_io - 1));          //HSTRX-G8 cross bus配置
        } else {
            CH3_FUNI_SEL((rx_io - 1));          //HSTRX-G9 cross bus配置
        }
    }

    memset(&huart0, 0x00, sizeof(huart0));
#if HUART_EQ_DBG_EN
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
    huart0.rxbuf   = eq_rx_buf;
    huart0.rxbuf_size = EQ_BUFFER_LEN;
#endif
    huart0.rx_port = HUART_RX_MAP;
    huart0.tx_port = HUART_TX_MAP;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 1;
    bsp_gpio_cfg_init(&huart0.gpio_tx, tx_io);
    bsp_gpio_cfg_init(&huart0.gpio_rx, rx_io);

    huart_init(&huart0, baud_rate);
}
#endif //HUART_EN


