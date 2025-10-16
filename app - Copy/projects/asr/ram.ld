#include "config.h"

ENTRY(_start)

/*  sram0空间分配图示,具体查看下方MEMORY项实际分配
|-------sram ------|0x0001 0000
    sram0
    |-----------|0x0001 0000
    |   boot
    |-----------|0x0001 0400
    |   stack
    |-----------|0x0001 0800
    |   data
    |-----------|0x0001 4800
    |           |-----------|0x0001 4800
    |   aram    |   pdn_comm
    |           |-----------|0x0001 6500
    |-----------|0x0001 8000

    cache ram
    |-----------|0x0007 0000
    |   comm
    |-----------|0x0007 1800
|------- end ------|0x0007 4000
*/

/* Define the flash max size */
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

/* Define the area */
__comm_vma              = 0x70000;
__comm_ram_size         = 0x1800;
__data_vma              = 0x10800;
__data_ram_size         = 9K;
__aram_vma              = 0x12c00;
__aram_size             = 21k;

__pdn_comm_vma          = 0x12c00;  //开关机代码区

__aram_pitch_vma        = 0x15A00;  //wav_pitch变量区起始地址
__aram_pitch_size       = 0xa00;    //wav_pitch变量区长度


MEMORY
{
    init                : org = __base,                 len = 512
    flash(rx)           : org = __base + 512,           len = __max_flash_size
    comm(rx)            : org = __comm_vma,             len = __comm_ram_size
    pdn_comm(rx)        : org = __pdn_comm_vma,         len = 0x1500

    stack               : org = 0x10400,                len = 1k
    data                : org = __data_vma,             len = __data_ram_size
    aram                : org = __aram_vma,             len = __aram_size

    aram_pitch          : org = __aram_pitch_vma,       len = __aram_pitch_size /* 开wav变调需和MP3复用，不能叠加播放了 */
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        KEEP(*(.vector))
        *(.plt)
        *(.com_text*)
        *(.com_rodata*)
        *(.avio_buf)
        *(.wav_buf)
        *(.srodata*)
        *debug.o(.rodata*)
        *(.data*)
        *(.sdata*)
        *(.music_buf)
	#if SPK_MIC_FREQ_SHIFT2_EN
        *(.text.freq_shift2_proc*)
	#endif
        . = ALIGN(512);
    } > comm AT > flash

    .pdn_comm : {
        *(.pdncom_text*)
        *(.pdncom_buf*)
        . = ALIGN(512);
    } > pdn_comm AT > flash

    .flash : {
        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_text2_start_kws = .;
        *(.kws_text2.*)
        . = ALIGN(512);
        __code_text2_end_kws = .;

        __code_start_usbdev = .;
        *(.usbdev.com*)
        . = ALIGN(512);
        __code_end_usbdev = .;

         __code_start_mp3 = .;
        *(.mp3_text*)
        . = ALIGN(512);
        __code_end_mp3 = .;

        __code_start_esbc = .;
        *(.text.esbc.asm.*)
        . = ALIGN(512);
        __code_end_esbc = .;

        __code_start_lsbcdec = .;
        *(.text.lsbc.*)
        *(.rodata.lsbc.*)
        . = ALIGN(512);
        __code_end_lsbcdec = .;

        __code_start_mp3enc = .;
        *(.text.mp3enc.*)
        *(.rodata.mp3enc.*)
        . = ALIGN(512);
        __code_end_mp3enc = .;

        #if FFT_PROC_EN
        __code_start_fft = .;
        *(.text.fft*)
        *(.fft_rodata.windown)
        *(.fft_rodata.sqrt)
        . = ALIGN(512);
        __code_end_fft = .;
        #endif

        #if SPK_MIC_APF_CHG_EN
        __code_start_allpass_filter_change = .;
        *(.text.allpass_filter_change_proc*)
        __code_end_allpass_filter_change = .;
        #endif

        __code_start_drc = .;
        *(.drc_text*)
        *(.drc_rodata*)
        . = ALIGN(512);
        __code_end_drc = .;

        __code_start_echo = .;
        *(.text.echo_proc*)
        . = ALIGN(512);
        __code_end_echo = .;

        __code_start_reverb = .;
        *(.text.reverb_proc*)
        . = ALIGN(512);
        __code_end_reverb = .;

        *(.text*)
        *(.music_text*)
        *(.midi_text*)
        *(.lcd_text*)
        *(.wavdec*)
        *(.rodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);
    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x200;
        __irq_stack = .;
        //cache stack使用
        . = 0x400;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)

        *(.huart_buf)
        *(.dac_obuf*)
        *(.sdadc_buf)

        *(.m_buf*)
        *(.uart_buf)
        *(.eq_buf)
        
        *(.msc_buf)
        *(.mav_buf)

        *(.drc_buf*)
        *(.pitch_buf*)
        *(.angle_change_buf*)
        
        #if SPK_MIC_FREQ_SHIFT2_EN
        *(.buf.freq_shift2*)
        #endif

        #if FFT_PROC_EN
        *(.fft_buf*)
        #endif

        #if SPI_AUDIO_TEST_EN
        *(.spi_buf)
        #endif

        #if RGB_SERIAL_EN
        *(.spi_led_buf)
        #endif

        #if (SPI1_AUDIO_EN && (!SPI_SLAVE_OR_MASTER_EN))
        *(.spi_recv_buf*)
        #endif
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;

    .fs_aram __aram_vma (NOLOAD) : {
        . = 0x1000;
        *(.pff.scan.tempbuf)
        *(.fname_buf)
    } > aram

    .midi_aram __aram_vma (NOLOAD) : {
        *(.midi_buf*)
        *(.gsf_buf*)
    } > aram

    .mp3_aram __aram_vma (NOLOAD) : {
        *(.diskio_buf)
        *(.pff.buf)
        *(.pff_buf.*)

        *(.mp3buf.dec)
        *(.mp3_decram_buf*)
        *(.ext_msc*)
    } > aram

    .lsbc_aram __aram_vma (NOLOAD) : {
        *(.lsbc_buf*)
    } > aram

    .esbc_aram __aram_vma (NOLOAD) : {
        *(.esbc_buf*)
    } > aram

    .mp3enc_aram __aram_vma (NOLOAD) : {
        *(.mp3enbuf*)
        *(.rec_buf*)
    } > aram

    .ude_aram __aram_vma (NOLOAD) : {
        *(.udev_buf*)
        *(.usb_buf*)
        *(.ude.aubuf)
        *(.usb.device.desc.buf)
        *(.usb.device.midi.buf)
    } > aram
    
    .echo_aram __aram_vma (NOLOAD) : {
        *(.echo_buf.echo*)
        . = 0x2C00;
    } > aram

    .reverb_aram __aram_vma (NOLOAD) : {
        *(.reverb_buf*)
    } > aram
    
    .asr_aram __aram_vma (NOLOAD) : {
        *(.asr_buf*)
    } > aram
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__pdn_comm_lma = LOADADDR(.pdn_comm);
__pdn_comm_size = SIZEOF(.pdn_comm);
