#include "reboot.h"

void raspi3_reboot(int ticks) // reboot after watchdog timer expire
{ 
	*PM_RSTC = PM_PASSWORD | 0x20; // full reset
	*PM_WDOG = PM_PASSWORD | ticks; // number of watchdog tick
}