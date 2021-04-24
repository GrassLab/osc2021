#ifndef UART_H_
#define UART_H_ 
#include <gpio.h>
#include <types.h>
#include <circular_queue.h>

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
//irq setting
#define UART_IRQ1_ENABLE (volatile unsigned int*)(MMIO_BASE + 0xb210)
#define UART_IRQ1_DISABLE (volatile unsigned int*)(MMIO_BASE + 0xb21c)
#define UART_IRQ_BASIC_PENDING (volatile unsigned int*)(MMIO_BASE + 0xb200)
#define UART_IRQ_PENDING1 (volatile unsigned int*)(MMIO_BASE + 0xb204)
//buffer
struct circular_queue uart_write_buffer;
struct circular_queue uart_read_buffer;

void uart_init();
void uart_send(unsigned int c);
__attribute__((__section__ (".bootloader"))) char uart_getc();
void uart_puts(char* s);
void uart_hex(unsigned int d);
size_t sys_uart_read(char buf[], size_t size);
size_t sys_uart_write(const char buf[], size_t size);
__attribute__((__section__ (".bootloader"))) size_t do_uart_read(char buf[], size_t size);
size_t do_uart_write(const char buf[], size_t size);
size_t uart_readline(char* buf, size_t count);

void uart_tx_interrupt_enable();
void uart_rx_interrupt_enable();
void uart_tx_interrupt_disable();
void uart_rx_interrupt_disable();
void uart_async_send(unsigned int c);
char uart_async_getc();
size_t uart_async_write(char* buf, size_t count);
size_t uart_async_read(char* buf, size_t size);
#endif
