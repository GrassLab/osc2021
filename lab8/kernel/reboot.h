#ifndef REBOOT
#define REBOOT

#include "mmu.h"

#define PM_PASSWORD 0x5A000000

#define PM_RSTC ((volatile unsigned int*)0x3F10001C + KVA)
#define PM_WDOG ((volatile unsigned int*)0x3F100024 + KVA)

void raspi3_reboot(int ticks); // reboot after watchdog timer expire
void cancel_reset(); // cancel_reset

#endif