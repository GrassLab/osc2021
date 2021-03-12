#include "reboot.h"

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}