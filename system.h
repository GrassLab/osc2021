#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_RSTS ((volatile unsigned int *)(0x3F100020))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
void reset(int tick);
void cancel_reset();

#endif
