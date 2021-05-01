#ifndef UART_H
#define UART_H
#include "gpio.h"
#include "utils.h"

#define async


#define MAX_BUFFER_LEN 128

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


#define CORE0_INTERRUPT_SOURCE ((volatile unsigned int *)(0x40000060))

#define ARM_IRQ_REG_BASE ((volatile unsigned int*)(MMIO_BASE + 0x0000b000))
#define IRQ_PENDING_1 	 ((volatile unsigned int*)(MMIO_BASE + 0x0000b204))
#define ENB_IRQS1 		 ((volatile unsigned int*)(MMIO_BASE + 0x0000b210))
#define DISABLE_IRQS1 	 ((volatile unsigned int*)(MMIO_BASE + 0x0000b21c))
#define AUX_IRQ (1 << 29)


#define PM_RSTC         ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)
#define PM_PASSWORD     (0x5a000000)

char read_buf[MAX_BUFFER_LEN];
char write_buf[MAX_BUFFER_LEN];
int read_buf_start, read_buf_end;
int write_buf_start, write_buf_end;

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init();
void uart_send(char c);
char uart_get();
void uart_puts(char *s);
int  uart_get_int();
void uart_put_int(unsigned long num);
void uart_puts_bySize(char *s, int size);
void uart_put_hex(unsigned long d) ;

void enable_uart_interrupt();
void disable_uart_interrupt();
void assert_transmit_interrupt();
void clear_transmit_interrupt();
void uart_handler();
char uart_async_getc();
void uart_async_puts(char *str);

void test_uart_async();



#endif

