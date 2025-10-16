#ifndef _DUMP_H_
#define _DUMP_H_

void my_spi_init(void);
void my_spi_putc(u8 ch);

void huart_dump_init(void);
void huart_dump_tx(u32 ch, u32 index, u8 *ptr, u32 size);
void huart_dump_tx_done(void);
#endif // _DUMP_H_
