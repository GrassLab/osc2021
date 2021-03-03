#ifndef GPIO_H
#define GPIO_H

#include "mmio.h"

// GPIO registers 
// Use volatile tells the compiler that the value of the variable may change at any time by hardware
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004)) // GPIO Function Select Registers
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094)) // GPIO Pull-up/down Register
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098)) // GPIO Pull-up/down Clock Register

#endif