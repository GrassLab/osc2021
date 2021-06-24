#ifndef _GPIO_H_
#define  _GPIO_H_

#include "mmu.h"

#define MMIO_BASE    (KERNEL_MAPPING+0x3F000000)
#define GPIO_BASE    (MMIO_BASE+0x200000)

#define GPFSEL0      ((volatile unsigned int*)(GPIO_BASE+0x00))
#define GPFSEL1      ((volatile unsigned int*)(GPIO_BASE+0x04))
#define GPFSEL2      ((volatile unsigned int*)(GPIO_BASE+0x08))
#define GPFSEL3      ((volatile unsigned int*)(GPIO_BASE+0x0C))
#define GPFSEL4      ((volatile unsigned int*)(GPIO_BASE+0x10))
#define GPFSEL5      ((volatile unsigned int*)(GPIO_BASE+0x14))
#define GPFSET0      ((volatile unsigned int*)(GPIO_BASE+0x1C))
#define GPFSET1      ((volatile unsigned int*)(GPIO_BASE+0x20))
#define GPCLR0       ((volatile unsigned int*)(GPIO_BASE+0x28))
#define GPLEV0       ((volatile unsigned int*)(GPIO_BASE+0x34))
#define GPLEV1       ((volatile unsigned int*)(GPIO_BASE+0x38))
#define GPEDS0       ((volatile unsigned int*)(GPIO_BASE+0x40))
#define GPEDS1       ((volatile unsigned int*)(GPIO_BASE+0x44))
#define GPHEN0       ((volatile unsigned int*)(GPIO_BASE+0x64))
#define GPHEN1       ((volatile unsigned int*)(GPIO_BASE+0x68))
#define GPPUD        ((volatile unsigned int*)(GPIO_BASE+0x94))
#define GPPUDCLK0    ((volatile unsigned int*)(GPIO_BASE+0x98))
#define GPPUDCLK1    ((volatile unsigned int*)(GPIO_BASE+0x9C))

#endif
