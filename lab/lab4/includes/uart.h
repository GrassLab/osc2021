#ifndef _UART
#define _UART

/* Auxilary mini UART registers */
#define MMIO_BASE 0x3F000000
#define AUX_ENABLE ((volatile unsigned int *)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO ((volatile unsigned int *)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER ((volatile unsigned int *)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR ((volatile unsigned int *)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR ((volatile unsigned int *)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR ((volatile unsigned int *)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR ((volatile unsigned int *)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR ((volatile unsigned int *)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int *)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL ((volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT ((volatile unsigned int *)(MMIO_BASE + 0x00215064))
// Set baud rate and characteristics (115200 8N1) and map to GPIO
#define AUX_MU_BAUD ((volatile unsigned int *)(MMIO_BASE + 0x00215068))

void uart_init(unsigned int);
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);

#define IRQ_PENDING_1 (MMIO_BASE + 0x0000B204)
#define ENB_IRQS1 (MMIO_BASE + 0x0000B210)
#define AUX_IRQ (1 << 29)
#define CORE0_INTERRUPT_SOURCE 0x40000060
#define buff_size 64
char get_buff[buff_size], send_buff[buff_size];
unsigned long long int get_top, get_buttom, interupt, dummy_pos[5];
void uart_asyn_puts(char *);
void uart_irq_send(unsigned int);
void _uart_irq_getc();
void _uart_irq_puts();
char _uart_getc();
char uart_getc();
void enable_uart_interrupt();

#endif /*_UART */