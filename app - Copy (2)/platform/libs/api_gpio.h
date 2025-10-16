#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

enum {
    GPIOxSET    =   0,
    GPIOxCLR,
    GPIOx,
    GPIOxDIR,
    GPIOxDE,
    GPIOxFEN,
    GPIOxDRV,
    GPIOxPU,
    GPIOxPD,
    GPIOxPU500K,
    GPIOxPD500K,
};

typedef struct {
    psfr_t sfr;             //GPIO SFR ADDR
    u8 num;
    u8 type;                //type = 1,高压IO,可以上拉10k或下拉60k, type = 0,普通IO,可上拉10k/500k或下拉60k/500k.
    u16 pin;
} gpio_t;

#define bsp_gpio_cfg_init(x, y)         gpio_cfg_init(x, y)

void gpio_cfg_init(gpio_t *g, u8 io_num);       //根据GPIO number初始化GPIO结构体
u8 get_adc_gpio_num(u8 adc_ch);
void adcch_io_pu10k_enable(u8 adc_ch);
u16 get_cur_vddio_vol(void);
void vddio_voltage_update(u16 vbat_val);


///port wakeup and interrupt
#define PORT_INT0_VECTOR            0
#define PORT_INT1_VECTOR            1
#define PORT_INT2_VECTOR            2
#define PORT_INT3_VECTOR            3
#define PORT_INT4_VECTOR            4
#define PORT_INT5_VECTOR            5
#define PORT_INT6_VECTOR            6
#define PORT_INT7_VECTOR            7

void port_var_init(void);
void port_irq_register(int irq_num, isr_t isr);             //注册一条PORT中断（最大8条, 对照pinfunction查询GPIO的INT编号）
void port_irq_free(int irq_num);
u8 port_wakeup_get_status(void);
bool port_wakeup_init(u8 io_num, u8 edge, u8 pupd_sel);     //参数edge: 0->上升沿, 1->下降沿,  参数pupd_sel: 0->不开内部上拉, 1->开内部上拉, 2->开内部下拉
bool port_wakeup_exit(u8 io_num);
void port_wakeup_all_init(u8 io_num, u8 edge, u8 pupd_sel);

#endif // _BSP_GPIO_H
