#include "reboot.h"

#define PM_RSTC      ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG      ((volatile unsigned int*)0x3F100024)
#define PM_PASSWORD  0x5a000000

void reboot(){
	int tick=87;
	*PM_RSTC=PM_PASSWORD|0x20;
	*PM_WDOG=PM_PASSWORD|tick;
	while(1){
		//rebooting...
	}
}