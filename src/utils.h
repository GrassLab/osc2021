#ifndef UTILS_H
#define UTILS_H

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024

void reset(int tick);

#endif
