#include "mmio.h"
#define PM_PASSWORD 0x5a000000

#define PM_RSTC      ((volatile unsigned int*)(MMIO_BASE+0x10001c))
#define PM_WDOG      ((volatile unsigned int*)(MMIO_BASE+0x100024))

void set(unsigned int reg, int tick){
    *(unsigned int*)reg = tick;  
}

void reset(int tick){ // reboot after watchdog timer expire
  set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
  set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() {
  set(PM_RSTC, PM_PASSWORD | 0); // full reset
  set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
}