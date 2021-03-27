#include "reboot.h"
#include "mmio.h"
void reset (int tick) { // reboot after watchdog timer expire
    *mmio(PM_RSTC) = PM_PASSWORD | 0x20; // full reset
    *mmio(PM_WDOG) = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset () {
    *mmio(PM_RSTC) = PM_PASSWORD; // full reset
    *mmio(PM_WDOG) = PM_PASSWORD; // number of watchdog tick
}
