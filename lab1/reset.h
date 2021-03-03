#ifndef __RESET_H_
#define __RESET_H_

#include <stdint.h>
#define __REG_TYPE volatile uint32_t
#define __REG __REG_TYPE *

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     ((__REG) 0x3F10001c)
#define PM_WDOG     ((__REG) 0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

#endif
