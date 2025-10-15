/*
*
*   软件SPI DUMP RAW数据，通过逻辑分析仪抓SPI通信并导出数据。
*
*/

#include "include.h"

#if SPI_DUMP_EN

#define SCLK_GP             A
#define SDAT_GP             A
#define SCLK_BIT            BIT(8)
#define SDAT_BIT            BIT(9)

#define SCLK_GPIODE         SET_MACRO(GPIO, SET_MACRO(SCLK_GP, DE))
#define SCLK_GPIOFEN        SET_MACRO(GPIO, SET_MACRO(SCLK_GP, FEN))
#define SCLK_GPIODIR        SET_MACRO(GPIO, SET_MACRO(SCLK_GP, DIR))
#define SCLK_GPIOSET        SET_MACRO(GPIO, SET_MACRO(SCLK_GP, SET))
#define SCLK_GPIOCLR        SET_MACRO(GPIO, SET_MACRO(SCLK_GP, CLR))
#define SDAT_GPIODE         SET_MACRO(GPIO, SET_MACRO(SDAT_GP, DE))
#define SDAT_GPIOFEN        SET_MACRO(GPIO, SET_MACRO(SDAT_GP, FEN))
#define SDAT_GPIODIR        SET_MACRO(GPIO, SET_MACRO(SDAT_GP, DIR))
#define SDAT_GPIOSET        SET_MACRO(GPIO, SET_MACRO(SDAT_GP, SET))
#define SDAT_GPIOCLR        SET_MACRO(GPIO, SET_MACRO(SDAT_GP, CLR))

void my_spi_init(void)
{
    SCLK_GPIODE |= SCLK_BIT;
    SCLK_GPIOFEN &= ~SCLK_BIT;
    SCLK_GPIODIR &= ~SCLK_BIT;
    SCLK_GPIOSET = SCLK_BIT;

    SDAT_GPIODE |= SDAT_BIT;
    SDAT_GPIOFEN &= ~SDAT_BIT;
    SDAT_GPIODIR &= ~SDAT_BIT;
    SDAT_GPIOSET = SDAT_BIT;
}

AT(.com_text.spi.putc)
void my_spi_putc(u8 ch)
{
   GLOBAL_INT_DISABLE();
   u8 i;
   for(i=0; i<8; i++) {
       SCLK_GPIOCLR = SCLK_BIT;
       if(ch & 0x80) {
           SDAT_GPIOSET = SDAT_BIT;
       } else {
           SDAT_GPIOCLR = SDAT_BIT;
       }
       ch <<= 1;
       SCLK_GPIOSET = SCLK_BIT;
   }
   GLOBAL_INT_RESTORE();
}
#endif // SPI_DUMP_EN
