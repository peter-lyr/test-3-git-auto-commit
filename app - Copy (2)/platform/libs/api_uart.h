#ifndef __API_UART_H__
#define __API_UART_H__

enum {
    HUART_TR_PA9    = 0,
    HUART_TR_PB3,
    HUART_TR_PB4,
    HUART_TR_PA12,
    HUART_TR_PA0,
    HUART_TR_PA7,
    HUART_TR_G7,            //NULL
    HUART_TR_G8,            //G8 CrossBus
    HUART_TR_G9,            //G9 CrossBus
};

typedef struct {
    union {
        struct {
            uint8_t  tx_port : 4;
            uint8_t  rx_port : 4;
            uint8_t  rxisr_en : 1;
            uint8_t  txisr_en : 1;
            uint8_t  rxbuf_loop : 1;
            uint8_t  tx_1st : 1;
        };
        uint16_t all_setting;
    };
    uint16_t rxbuf_size;
    uint8_t *rxbuf;
    gpio_t gpio_tx;
    gpio_t gpio_rx;
} huart_t;

void huart_init(huart_t *huart, uint32_t baud_rate);
void huart_exit(void);
void huart_set_baudrate(uint baudrate);
void huart_tx(const void *buf, uint len);
uint huart_get_rxcnt(void);
char huart_getchar(void);
void huart_putchar(const char ch);
void huart_rxfifo_clear(void);
void huart_mux_set_tx(void);
void huart_mux_set_rx(void);

#endif
