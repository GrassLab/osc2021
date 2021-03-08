#ifndef UART_H_
#define UART_H_ 
#include <gpio.h>
#include <types.h>
#define AUX_ENABLES (volatile unsigned int*)(MMIO_BASE + 0x0215004)
#define AUX_MU_IO_REG (volatile unsigned int*)(MMIO_BASE + 0x215040)
#define AUX_MU_IER_REG (volatile unsigned int*)(MMIO_BASE + 0x215044)
#define AUX_MU_IIR_REG (volatile unsigned int*)(MMIO_BASE + 0x215048)
#define AUX_MU_LCR_REG (volatile unsigned int*)(MMIO_BASE + 0x21504c)
#define AUX_MU_MCR_REG (volatile unsigned int*)(MMIO_BASE + 0x215050)
#define AUX_MU_LSR_REG (volatile unsigned int*)(MMIO_BASE + 0x215054)
#define AUX_MU_MSR_REG (volatile unsigned int*)(MMIO_BASE + 0x215058)
#define AUX_MU_SCRATCH (volatile unsigned int*)(MMIO_BASE + 0x21505c)
#define AUX_MU_CNTL_REG (volatile unsigned int*)(MMIO_BASE + 0x215060)
#define AUX_MU_STAT_REG (volatile unsigned int*)(MMIO_BASE + 0x215064)
#define AUX_MU_BAUD_REG (volatile unsigned int*)(MMIO_BASE + 0x215068)

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char* s);
void uart_hex(unsigned int d);
size_t uart_read(char* buf, size_t count);
#endif
