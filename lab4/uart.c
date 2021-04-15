/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "gpio.h"

/* Auxilary mini UART registers */
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))


//lab4
#define IRQs1           ((volatile unsigned int*)(MMIO_BASE+0x0000B210))
#define GPU_INTERRUPTS_ROUTING  ((volatile unsigned int*)(0x4000000C))
#define CORE0_INTERRUPT_SOURCE  ((volatile unsigned int*)(0x40000060))
char read_buffer[1024];
char write_buffer[1024];


/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart  //3 modules : UART, SPI1, SPI2. 
    *AUX_MU_CNTL = 0;       
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;    // disable interrupts
    *AUX_MU_BAUD = 270;    // 115200 baud
    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx

    while(*AUX_MU_LSR&0x01)
    {
        char c = (char)(*AUX_MU_IO);
    }

}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x20));
    /* write the character to the buffer */
    *AUX_MU_IO=c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x01));
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

// void uart_interupt_init()
// {
//     //asm volatile("msr DAIFSet, 0xf");
//     //asm volatile("msr DAIFClr, 0xf");

//     uart_puts("test!!!\n\n");
//     *AUX_MU_IER = 2;
//     *IRQs1 = *IRQs1 | (1 << 29);

//     *GPU_INTERRUPTS_ROUTING = 0x00;

//     //enable_irq();
//     // while (1) 
//     // {
//     //     asm volatile("wfi");
//     // }
// }













// static int uart_read_isr(const uint32_t value) {
//   (void)value;
//   mem_dmb();

//   while (1) {
//     const uint32_t iir = mem_read32(BASE_ADDR + AUX_MU_IIR_REG);

//     if ((iir & 1) == 1) {
//       // No Mini UART interrupt pending, return.
//       break;
//     }
    
//     if ((iir & 6) == 4) {
//       // Character available, remove it from the receive FIFO.
//       const uint32_t k = mem_read32(BASE_ADDR + AUX_MU_IO_REG) & 0xff;
//       // Send it to user code.
//       s_callback(k);
//     }
//   }

//   mem_dmb();
//   return 0;
// }

// void uart_init(const unsigned baudrate, uart_newchar_t newchar) {
//   // Get the clock used with the Mini UART.
//   const uint32_t clock_rate = prop_getclockrate(PROP_CLOCK_CORE);

//   // Set pins 14 and 15 to use with the Mini UART.
//   gpio_select(14, GPIO_FUNCTION_5);
//   gpio_select(15, GPIO_FUNCTION_5);

//   // Turn pull up/down off for those pins.
//   gpio_setpull(14, GPIO_PULL_OFF);
//   gpio_setpull(15, GPIO_PULL_OFF);

//   mem_dmb();

//   // Enable only the Mini UART.
//   mem_write32(BASE_ADDR + AUX_ENABLES, 1);
//   // Disable receiving and transmitting while we configure the Mini UART.
//   mem_write32(BASE_ADDR + AUX_MU_CNTL_REG, 0);
//   // Turn off interrupts.
//   mem_write32(BASE_ADDR + AUX_MU_IER_REG, 0);
//   // Set data size to 8 bits.
//   mem_write32(BASE_ADDR + AUX_MU_LCR_REG, 3);
//   // Put RTS high.
//   mem_write32(BASE_ADDR + AUX_MU_MCR_REG, 0);
//   // Clear both receive and transmit FIFOs.
//   mem_write32(BASE_ADDR + AUX_MU_IIR_REG, 0xc6);

//   // Set the desired baudrate.
//   const uint32_t divisor = clock_rate / (8 * baudrate) - 1;
//   mem_write32(BASE_ADDR + AUX_MU_BAUD_REG, divisor);

//   if (newchar != NULL) {
//     // Install the IRQ handler and enable read interrupts.
//     s_callback = newchar;
//     isr_addhandler(ISR_IRQ, uart_read_isr);
//     mem_write32(BASE_ADDR + AUX_MU_IER_REG, 5);
//     isr_enablebasic(29);
//   }

//   // Enable receiving and transmitting.
//   mem_write32(BASE_ADDR + AUX_MU_CNTL_REG, 3);
// }