#ifndef __MMIO_H_
#define __MMIO_H_

#define MMIO_BASE 0x3F000000

#define UART_MAP_OFFSET 0x00215000
#define GPIO_MAP_OFFSET 0x00200000

#define AUX_IRQ ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x00))
#define AUX_ENABLES                                                            \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x04))
#define AUX_MU_IO_REG                                                          \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x40))
#define AUX_MU_IER_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x44))
#define AUX_MU_IIR_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x48))
#define AUX_MU_LCR_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x4C))
#define AUX_MU_MCR_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x50))
#define AUX_MU_LSR_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x54))
#define AUX_MU_MSR_REG                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x58))
#define AUX_MU_SCRATCH                                                         \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x5C))
#define AUX_MU_CNTL_REG                                                        \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x60))
#define AUX_MU_STAT_REG                                                        \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x64))
#define AUX_MU_BAUD_REG                                                        \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x68))
#define AUX_SPI0_CNTL0_REG                                                     \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x80))
#define AUX_SPI0_CNTL1_REG                                                     \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x84))
#define AUX_SPI0_STAT_REG                                                      \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x88))
#define AUX_SPI0_IO_REG                                                        \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x90))
#define AUX_SPI0_PEEK_REG                                                      \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0x94))
#define AUX_SPI1_CNTL0_REG                                                     \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0xC0))
#define AUX_SPI1_CNTL1_REG                                                     \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0xC4))
#define AUX_SPI1_STAT_REG                                                      \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0xC8))
#define AUX_SPI1_IO_REG                                                        \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0xD0))
#define AUX_SPI1_PEEK_REG                                                      \
  ((volatile unsigned int *)(MMIO_BASE + UART_MAP_OFFSET + 0xD4))

#define GPFSEL0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x00))
#define GPFSEL1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x04))
#define GPFSEL2 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x08))
#define GPFSEL3 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x0C))
#define GPFSEL4 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x10))
#define GPFSEL5 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x14))
#define GPSET0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x1C))
#define GPSET1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x20))
#define GPCRL0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x28))
#define GPCRL1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x2C))
#define GPLEV0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x34))
#define GPLEV1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x38))
#define GPEDS0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x40))
#define GPEDS1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x44))
#define GPREN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x4C))
#define GPREN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x50))
#define GPFEN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x58))
#define GPFEN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x5C))
#define GPHEN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x64))
#define GPHEN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x68))
#define GPLEN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x70))
#define GPLEN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x74))
#define GPAREN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x7C))
#define GPAREN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x80))
#define GPAFEN0 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x88))
#define GPAFEN1 ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x8C))
#define GPPUD ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x94))
#define GPPUDCLK0                                                              \
  ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x98))
#define GPPUDCLK1                                                              \
  ((volatile unsigned int *)(MMIO_BASE + GPIO_MAP_OFFSET + 0x9C))

#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024


void uart_init();
char uart_getc();
void uart_setc(char);
void reset(int);

#endif // __MMIO_H_
