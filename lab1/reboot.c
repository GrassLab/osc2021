#include "reboot.h"

void raspi3_reboot(int ticks)
{ 
	*PM_RSTC = PM_PASSWORD | 0x20;
	*PM_WDOG = PM_PASSWORD | ticks;
}

void cancel_reset()
{
  *PM_RSTC = PM_PASSWORD | 0;
  *PM_WDOG = PM_PASSWORD | 0;
}