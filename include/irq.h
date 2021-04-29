#ifndef _IRQ_H_
#define  _IRQ_H_

#include "gpio.h"

#define CORE0_IRQ_SRC ((volatile unsigned int*)(0x40000060))
#define BASIC_PEND    ((volatile unsigned int*)(MMIO_BASE+0xb200))
#define GPU_PEND1     ((volatile unsigned int*)(MMIO_BASE+0xb204))

void irq_route();

#endif
