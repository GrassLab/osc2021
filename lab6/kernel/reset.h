#ifndef RESET_H
#define RESET_H
#include "gpio.h"
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)(MMIO_BASE + 0x10001c))
#define PM_WDOG ((volatile unsigned int*)(MMIO_BASE + 0x00100024))

void reset(int tick);
#endif
