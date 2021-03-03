#ifndef REBOOT
#define REBOOT

#define PM_PASSWORD 0x5A000000

#define PM_RSTC ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void raspi3_reboot(int ticks); // reboot after watchdog timer expire
void cancel_reset(); // cancel_reset

#endif