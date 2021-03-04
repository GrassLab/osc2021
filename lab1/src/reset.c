#include "include/reset.h"
void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = (PM_PASSWORD | 0x20); // full reset
  *PM_WDOG = (PM_PASSWORD | tick); // number of watchdog tick
}

