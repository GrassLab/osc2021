#ifndef __BCM2837_H
#define __BCM2837_H

#include <stdint.h>

#define SET_REG(REG, SELECT, VAL) {((REG)=((REG)&(~(SELECT)))|(VAL));};
#define BUS_TO_PHY (0x7E000000U - 0x3F000000U)

// GPIO Register
#define GPIO_BASE (0x7E200000U - BUS_TO_PHY)

#define GPFSEL0 ((volatile uint32_t *)(GPIO_BASE))  //GPIO Function Select 0
#define GPFSEL1 ((volatile uint32_t *)(GPIO_BASE + 0x04U))  //GPIO Function Select 1
#define GPFSEL2 ((volatile uint32_t *)(GPIO_BASE + 0x08U))  //GPIO Function Select 2
#define GPFSEL3 ((volatile uint32_t *)(GPIO_BASE + 0x0CU))  //GPIO Function Select 3
#define GPFSEL4 ((volatile uint32_t *)(GPIO_BASE + 0x10U))  //GPIO Function Select 4
#define GPFSEL5 ((volatile uint32_t *)(GPIO_BASE + 0x14U))  //GPIO Function Select 5

#define GPSET0 ((volatile uint32_t *)(GPIO_BASE + 0x1CU))  //GPIO Pin Output Set 0
#define GPSET1 ((volatile uint32_t *)(GPIO_BASE + 0x20U))  //GPIO Pin Output Set 1

#define GPCLR0 ((volatile uint32_t *)(GPIO_BASE + 0x28U))  //GPIO Pin Output Clear 0
#define GPCLR1 ((volatile uint32_t *)(GPIO_BASE + 0x2CU))  //GPIO Pin Output Clear 1

#define GPLEV0 ((volatile uint32_t *)(GPIO_BASE + 0x34U))  //GPIO Pin Level 0
#define GPLEV1 ((volatile uint32_t *)(GPIO_BASE + 0x38U))  //GPIO Pin Level 1

#define GPEDS0 ((volatile uint32_t *)(GPIO_BASE + 0x40U))  //GPIO Pin Event Detect Status 0
#define GPEDS1 ((volatile uint32_t *)(GPIO_BASE + 0x44U))  //GPIO Pin Event Detect Status 1

#define GPREN0 ((volatile uint32_t *)(GPIO_BASE + 0x4CU))  //GPIO Pin Rising Edge Detect Enable 0
#define GPREN1 ((volatile uint32_t *)(GPIO_BASE + 0x50U))  //GPIO Pin Rising Edge Detect Enable 1

#define GPFEN0 ((volatile uint32_t *)(GPIO_BASE + 0x58U))  //GPIO Pin Falling Edge Detect Enable 0
#define GPFEN1 ((volatile uint32_t *)(GPIO_BASE + 0x5CU))  //GPIO Pin Falling Edge Detect Enable 1

#define GPHEN0 ((volatile uint32_t *)(GPIO_BASE + 0x64U))  //GPIO Pin High Detect Enable 0
#define GPHEN1 ((volatile uint32_t *)(GPIO_BASE + 0x68U))  //GPIO Pin High Detect Enable 1

#define GPLEN0 ((volatile uint32_t *)(GPIO_BASE + 0x70U))  //GPIO Pin Low Detect Enable 0
#define GPLEN1 ((volatile uint32_t *)(GPIO_BASE + 0x74U))  //GPIO Pin Low Detect Enable 1

#define GPAREN0 ((volatile uint32_t *)(GPIO_BASE + 0x7CU))  //GPIO Pin Async. Rising Edge Detect 0
#define GPAREN1 ((volatile uint32_t *)(GPIO_BASE + 0x80U))  //GPIO Pin Async. Rising Edge Detect 1

#define GPAFEN0 ((volatile uint32_t *)(GPIO_BASE + 0x88U))  //GPIO Pin Async. Falling Edge Detect 0
#define GPAFEN1 ((volatile uint32_t *)(GPIO_BASE + 0x8CU))  //GPIO Pin Async. Falling Edge Detect 1

#define GPPUD ((volatile uint32_t *)(GPIO_BASE + 0x94U))  //GPIO Pin Pull-up/down Enable
#define GPPUDCLK0 ((volatile uint32_t *)(GPIO_BASE + 0x98U))  //GPIO Pin Pull-up/down Enable Clock 0
#define GPPUDCLK1 ((volatile uint32_t *)(GPIO_BASE + 0x9CU))  //GPIO Pin Pull-up/down Enable Clock 1


// Auxiliary peripherals Register
#define AUX_BASE (0x7E215000 - BUS_TO_PHY)

#define AUX_IRQ ((volatile uint32_t *)(AUX_BASE)  //Auxiliary Interrupt status
#define AUX_ENABLES ((volatile uint32_t *)(AUX_BASE + 0x04U))  //Auxiliary enables
#define AUX_MU_IO_REG ((volatile uint32_t *)(AUX_BASE + 0x40U))  //Mini Uart I/O Data
#define AUX_MU_IER_REG ((volatile uint32_t *)(AUX_BASE + 0x44U))  //Mini Uart Interrupt Enable
#define AUX_MU_IIR_REG ((volatile uint32_t *)(AUX_BASE + 0x48U))  //Mini Uart Interrupt Identify
#define AUX_MU_LCR_REG ((volatile uint32_t *)(AUX_BASE + 0x4CU))  //Mini Uart Line Control
#define AUX_MU_MCR_REG ((volatile uint32_t *)(AUX_BASE + 0x50U))  //Mini Uart Modem Control
#define AUX_MU_LSR_REG ((volatile uint32_t *)(AUX_BASE + 0x54U))  //Mini Uart Line Status
#define AUX_MU_MSR_REG ((volatile uint32_t *)(AUX_BASE + 0x58U))  //Mini Uart Modem Status
#define AUX_MU_SCRATCH ((volatile uint32_t *)(AUX_BASE + 0x5CU))  //Mini Uart Scratch
#define AUX_MU_CNTL_REG ((volatile uint32_t *)(AUX_BASE + 0x60U))  //Mini Uart Extra Control
#define AUX_MU_STAT_REG ((volatile uint32_t *)(AUX_BASE + 0x64U))  //Mini Uart Extra Status
#define AUX_MU_BAUD_REG ((volatile uint32_t *)(AUX_BASE + 0x68U))  //Mini Uart Baudrate
#define AUX_SPI0_CNTL0_REG ((volatile uint32_t *)(AUX_BASE + 0x80U))  //SPI 1 Control register 0
#define AUX_SPI0_CNTL1_REG ((volatile uint32_t *)(AUX_BASE + 0x84U))  //SPI 1 Control register 1
#define AUX_SPI0_STAT_REG ((volatile uint32_t *)(AUX_BASE + 0x88U))  //SPI 1 Status
#define AUX_SPI0_IO_REG ((volatile uint32_t *)(AUX_BASE + 0x90U))  //SPI 1 Data
#define AUX_SPI0_PEEK_REG ((volatile uint32_t *)(AUX_BASE + 0x94U))  //SPI 1 Peek
#define AUX_SPI1_CNTL0_REG ((volatile uint32_t *)(AUX_BASE + 0xC0U))  //SPI 2 Control register 0
#define AUX_SPI1_CNTL1_REG ((volatile uint32_t *)(AUX_BASE + 0xC4U))  //SPI 2 Control register 1
#define AUX_SPI1_STAT_REG ((volatile uint32_t *)(AUX_BASE + 0xC8U))  //SPI 2 Status
#define AUX_SPI1_IO_REG ((volatile uint32_t *)(AUX_BASE + 0xD0U))  //SPI 2 Data
#define AUX_SPI1_PEEK_REG ((volatile uint32_t *)(AUX_BASE + 0xD4U))  //SPI 2 Peek

#endif