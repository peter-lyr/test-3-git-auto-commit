#include "include.h"
#include "spiflash1.h"

#if EX_SPIFLASH_SUPPORT

#if 0
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


void spi1_cs_en(void);
void spi1_cs_dis(void);
void spi1_cs_init(void);

#define SPI1_CS_INIT()           spi1_cs_init()
#define SPI1_CS_EN()             spi1_cs_en()
#define SPI1_CS_DIS()            spi1_cs_dis()

#define SF_PROGRAM              0x02                //SPIFlash编程
#define SF_READ                 0x03                //SPIFlash读取
#define SF_READSSR              0x05                //SPIFlash读状态寄存器
#define SF_WRITE_EN             0x06                //SPIFlash写使能
#define SF_ERASE                0x20                //SPIFlash擦除 //sector erase(4K擦除)
#define SF_RDID                 0x9F                //SPIFlash Read ID
#define SF_ENTER_4BYTE_MDOE     0xB7                //SPIFlash 4Byte模式
#define SF_EXIT_4BYTE_MODE      0xE9                //SPIFlash 退出4Byte模式

gpio_t spi1_gpio AT(.ext_msc.spi1);
spiflash1_cb_t spi1flash_cb AT(.ext_msc.spi1);

AT(.text.spiflash1_drv)
void spi1_cs_init(void)
{
    gpio_t *g = &spi1_gpio;
    gpio_cfg_init(g, SPIFLASH_CS_IO);
    if (g->sfr) {
        g->sfr[GPIOxFEN] &= ~g->pin;
        g->sfr[GPIOxDE]  |=  g->pin;
        g->sfr[GPIOxDIR] &= ~g->pin;
        g->sfr[GPIOxSET]  =  g->pin;
    }
}

AT(.text.spiflash1_drv)
void spi1_cs_en(void)
{
    gpio_t *g = &spi1_gpio;
    g->sfr[GPIOxCLR]  =  g->pin;
}

AT(.text.spiflash1_drv)
void spi1_cs_dis(void)
{
    gpio_t *g = &spi1_gpio;
    g->sfr[GPIOxSET]  =  g->pin;
}

///SPIFlash Delay
AT(.text.spiflash1_drv)
static void spi1_delay(void)
{
    uint cnt = 20;
    while (cnt--) {
        asm("nop");
    }
}

///SPI接口获取1Byte数据
AT(.text.spiflash1_drv)
static u8 spi1_getbyte(void)
{
    SPI1CON |= (BIT(4) | BIT(6));                   //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    SPI1CON &= ~BIT(6);
    return SPI1BUF;
}

///SPI接口发送1Byte数据
AT(.text.spiflash1_drv)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
}

AT(.text.spiflash1_drv)
u32 spiflash1_id_read(void)
{
    u8 id[4], i;
    u32 flash_id;
    SPI1_CS_EN();
    spi1_delay();
    spi1_sendbyte(SF_RDID);
    for (i = 0; i < 4; i++) {
        id[i] = spi1_getbyte();
    }
    SPI1_CS_DIS();
    flash_id = (id[0] << 24) | (id[1] << 16) | (id[2] << 8) | id[3];
    return flash_id;
}

//groundNum^topNum
u32 pow_cal(u32 groundNum, u32 topNum)
{
    u32 result = 1;
    int i;
    for(i = 0; i < topNum; i++){
        result = result * groundNum;
    }
    return result;
}

AT(.text.spiflash1_drv)
u32 spiflash1_capacity_cal(u32 flash_id)
{
    u32 flash_capacity;
    flash_capacity = 64 * pow_cal(2, (((flash_id >> 8) & 0xff) - 0x10)) * 1024;
    return flash_capacity;
}

AT(.text.spiflash1_drv)
void spi1_init(u32 baud)
{
    memset(&spi1flash_cb, 0, sizeof(spiflash1_cb_t));
#if   SPIFLASH_MAP == SPI1MAP_G1
#if SPI_4LINE_EN
    //DI_PB4, DO_PB3
    GPIOBFEN |=   BIT(4);           //DI input
    GPIOBDE  |=   BIT(4);
    GPIOBDIR |=   BIT(4);
#else
    GPIOBDIR |=   BIT(3);           //DO output
#endif
    //SPI1_G1: CLK_PA3, DIO_PB3,
    GPIOBFEN |=  BIT(3);
    GPIOBDE  |=  BIT(3);
    GPIOBDIR &= ~BIT(3);            //CLK,DO output

    GPIOAFEN |=  BIT(3);
    GPIOADE  |=  BIT(3);
    GPIOADIR &= ~BIT(3);            //CLK,DO output

    GPIOBPU  |=   BIT(3);           //DO pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G1;
#elif SPIFLASH_MAP == SPI1MAP_G2
#if SPI_4LINE_EN
    //DI_PA10, DO_PA12
    GPIOAFEN |=   BIT(10);          //DI input
    GPIOADE  |=   BIT(10);
    GPIOADIR |=   BIT(10);
#else
    GPIOADIR |=   BIT(12);          //DO output
#endif
    //SPI1_G2: CLK_PA11, DIO_PA12,
    GPIOAFEN |=   BIT(11)|BIT(12);
    GPIOADE  |=   BIT(11)|BIT(12);
    GPIOADIR &= ~(BIT(11)|BIT(12)); //CLK,DO output

    GPIOAPU  |=   BIT(12);          //DO pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G2;
#elif SPIFLASH_MAP == SPI1MAP_G3
#if SPI_4LINE_EN
    //DI_PA6, DO_PA5
    GPIOAFEN |=   BIT(6);           //DI input
    GPIOADE  |=   BIT(6);
    GPIOADIR |=   BIT(6);
#else
    GPIOADIR |=   BIT(5);           //DO output
#endif
    //SPI1_G3: CLK_PA4, DIO_PA5,
    GPIOAFEN |=   BIT(4)|BIT(5);
    GPIOADE  |=   BIT(4)|BIT(5);
    GPIOADIR &= ~(BIT(4)|BIT(5));   //CLK,DO output

    GPIOAPU  |=   BIT(5);           //DO pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G3;
#elif SPIFLASH_MAP == SPI1MAP_G4
#if SPI_4LINE_EN
    //DI_PA13, DO_PA15
    GPIOAFEN |=   BIT(13);          //DI input
    GPIOADE  |=   BIT(13);
    GPIOADIR |=   BIT(13);
#else
    GPIOADIR |=   BIT(15);          //DO output
#endif
    //SPI1_G4: CLK_PA14, DO_PA15,
    GPIOAFEN |=   BIT(14)|BIT(15);
    GPIOADE  |=   BIT(14)|BIT(15);
    GPIOADIR &= ~(BIT(14)|BIT(15));  //CLK,DO output

    GPIOAPU  |=   BIT(15);           //DO pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G4;
#endif
    SPI1_CS_INIT();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SPI1BAUD = get_sysclk_nhz()/baud - 1;
#if MIDI_EN && EXSPI_MIDI_EN
    SPI1BAUD = 3;       //使用SPI1flash播放midi时,提高SPI1频率
#endif
#if SPI_4LINE_EN
    SPI1CON =  0x01 | (0<<2);
#else
    SPI1CON =  0x01 | (1<<2);
#endif
}

///SPIFlash写使能
AT(.text.spiflash1_drv)
void spiflash1_init(void)
{
    spi1_init(SPIFLASH_BAUD);
    spiflash1_release_pd();
    //while(1) {WDT_CLR();printf("Flash ID = 0x%X\n", spiflash1_id_read());delay_ms(300);}
}

///SPIFlash写使能
AT(.text.spiflash1_drv)
static void spiflash1_write_enable(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(SF_WRITE_EN);
    SPI1_CS_DIS();
    spi1_delay();
}

///SPIFlash读状态寄存器
AT(.text.spiflash1_drv)
static uint spiflash1_readssr(void)
{
    uint ssr;
    SPI1_CS_EN();
    spi1_sendbyte(SF_READSSR);
    ssr = spi1_getbyte();
    SPI1_CS_DIS();
    spi1_delay();
    return ssr;
}

///SPIFlash等待BUSY
AT(.text.spiflash1_drv)
static void spiflash1_waitbusy(void)
{
    do {
        spi1_delay();
    } while (spiflash1_readssr() & 0x01);
}

///发送SPIFlash的地址
AT(.text.spiflash1_drv)
static void spiflash1_sendaddr(u32 addr)
{
    if(spi1flash_cb.mode_4byte_en){
        spi1_sendbyte(addr >> 24);
    }
    spi1_sendbyte(addr >> 16);
    spi1_sendbyte(addr >> 8);
    spi1_sendbyte(addr);
}

///SPIFlash读取
AT(.text.spiflash1_drv)
void spiflash1_read(void *buf, u32 addr, u32 len)
{
    TRACE("[r:0x%X,%d]",addr, len);
    u32 i;
    u8 *read_buf = (u8*)buf;
    SPI1_CS_EN();
    spi1_sendbyte(SF_READ);
    spiflash1_sendaddr(addr);
    for (i = 0; i < len; i++) {
       read_buf[i] = spi1_getbyte();
    }
    SPI1_CS_DIS();
}

///SPIFlash编程
AT(.text.spiflash1_drv)
void spiflash1_write(const void *buf, u32 addr, u32 len)
{
    TRACE("[w:0x%X,%d]",addr, len);
    u8 *write_buf = (u8*)buf;
    spiflash1_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_PROGRAM);
    spiflash1_sendaddr(addr);
#if 0
    SPI1CON &= ~BIT(4);                             //TX
    SPI1DMAADR = DMA_ADR(write_buf);
    SPI1DMACNT = len;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
#else
   u32 i;
   for (i = 0; i < len; i++) {
       spi1_sendbyte(write_buf[i]);
   }
#endif
    SPI1_CS_DIS();
    spiflash1_waitbusy();
}

///SPIFlash擦除
AT(.text.spiflash1_drv)
void spiflash1_erase(u32 addr)
{
    TRACE("spi erase: %x\n", addr);
    spiflash1_write_enable();

    SPI1_CS_EN();
    spi1_sendbyte(SF_ERASE);
    spiflash1_sendaddr(addr);
    SPI1_CS_DIS();

    spiflash1_waitbusy();
}

void spiflash1_enter_4byte_mode(void)
{
    spiflash1_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_ENTER_4BYTE_MDOE);
    SPI1_CS_DIS();
    spiflash1_waitbusy();
}

void spiflash1_exit_4byte_mode(void)
{
    spiflash1_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_EXIT_4BYTE_MODE);
    SPI1_CS_DIS();
    spiflash1_waitbusy();
}

bool s25fl512_enter_4byte_mode(void)
{
    bool bo_ret = true;
    u8 rd_byte;
    SPI1_CS_EN();
    spi1_delay();
    spi1_sendbyte(0x16);
    rd_byte = spi1_getbyte();
    SPI1_CS_DIS();
    spiflash1_waitbusy();
    if((rd_byte & 0x80) == 0){
        SPI1_CS_EN();
        spi1_delay();
        spi1_sendbyte(0x17);
        spi1_sendbyte(rd_byte | 0x80);
        SPI1_CS_DIS();
        spiflash1_waitbusy();

        SPI1_CS_EN();
        spi1_delay();
        spi1_sendbyte(0x16);
        rd_byte = spi1_getbyte();
        SPI1_CS_DIS();
        spiflash1_waitbusy();

        if((rd_byte & 0x80) == 0){
            printf("enter 4byte mode error\n");
            bo_ret = false;
        }
    }
    return bo_ret;
}


////#define SF_ERASE_BLOCK          0xD8              //SPIFlash擦除  //block erase   //64K
//AT(.text.spiflash1_drv)
//void spiflash1_erase_block(u32 addr)
//{
//    TRACE("spi erase: %x\n", addr);
//    spiflash1_write_enable();
//
//    SPI1_CS_EN();
//    spi1_sendbyte(SF_ERASE_BLOCK);
//    spiflash1_sendaddr(addr);
//    SPI1_CS_DIS();
//
//    spiflash1_waitbusy();
//}

///进入power_down模式
AT(.text.spiflash1_drv)
void spiflash1_power_down(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(0xB9);
    SPI1_CS_DIS();
    spi1_delay();
}

///解除power_down模式
AT(.text.spiflash1_drv)
void spiflash1_release_pd(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(0xAB);
    SPI1_CS_DIS();
    spi1_delay();
}

AT(.text.spiflash1_drv)
bool is_exspiflash_online(void)
{
    #if REC_INTERNAL_FLASH
        return true;
    #endif

    bool bo_ret = true;
    u8 i = 3;
    while(i--) {
        WDT_CLR();
        spi1flash_cb.flash_id = spiflash1_id_read();
        printf("flash id = 0x%X\n",spi1flash_cb.flash_id);
    }
    if(spi1flash_cb.flash_id != 0){
        spi1flash_cb.flash_capacity = spiflash1_capacity_cal(spi1flash_cb.flash_id);
        my_printf("flash_capacity: %d\n", spi1flash_cb.flash_capacity);
    }else{
        printf("--->!!!Read Flash ID Err\n",spi1flash_cb.flash_id);
        bo_ret = false;
    }
    if(spi1flash_cb.flash_capacity > 0x1000000){    //容量大于128Mbit
        spi1flash_cb.mode_4byte_en = 1;
        u8 manuf_id = (spi1flash_cb.flash_id >> 24) & 0xff;
        my_printf("manuf_id: 0x%X\n", manuf_id);
        if(manuf_id == 0x01){
            //S25FL512 flash
            if(!s25fl512_enter_4byte_mode()){
                spi1flash_cb.mode_4byte_en = 0;
            }
        }else{
            delay_5ms(10);
            spiflash1_enter_4byte_mode();
            delay_5ms(10);
        }
    }
    return bo_ret;
}

#if SPIFLASH_MUSIC_BIN_WRITE_TEST
extern u8 const exspiflash_music_bin[36864];
void write_music_bin_to_spiflash(void)
{
    spiflash1_init();
    u8 rx_buf[256];
    int block_total,block_ok, block_fail, i;
    block_total = sizeof(exspiflash_music_bin)/256;
    for (i = 0; i < sizeof(exspiflash_music_bin); ) {
        spiflash1_erase(i);
        printf("erase addr = 0x%X\n",i);
        i += 4096;
    }
    printf("write exspiflash_music_bin,len = %d, block_total = %d\n", sizeof(exspiflash_music_bin) ,block_total);
    //write
    for (i = 0; i < block_total; i++) {
        WDT_CLR();
        spiflash1_write((void*)&exspiflash_music_bin[i*256], i*256, 256);
        printf("[0x%X]",i*256);
    }
    //read_back and compare
    printf("\nread_back and compare\n");
    block_ok = 0;
    block_fail = 0;
    for (i = 0; i < block_total; i++) {
        spiflash1_read(rx_buf,i*256,256);
        if (memcmp(rx_buf,&exspiflash_music_bin[i*256],256)) {
            printf("[fail:0x%X]",i*256);
            block_fail++;
        } else {
            printf("[ok:0x%X]",i*256);
            block_ok++;
        }
    }
    printf("block_ok = %d  block_fail = %d\n", block_ok, block_fail);
}
#endif  //SPIFLASH_MUSIC_BIN_WRITE_TEST

#endif //(EX_SPIFLASH_SUPPORT & EXSPI_REC)


#if UFLASH_BIN_EN
#define BASE_ADDR               UFLASH_BIN_ADDR
#define BASE_LEN                UFLASH_BIN_DATA_LEN

//用户bin读取示例
void uflash_read_demo(void)
{
    u8 buf_data[10];
    spiflash_read_kick(buf_data, BASE_ADDR, 10);
    if(spiflash_read_wait()){
        printf("uflash_read_demo\n");
        print_r(buf_data, 10);
    }
}
#endif


