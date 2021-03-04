#define PM_PASSWORD     0x5a000000
#define PM_RSTC         ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)

float get_timestamp() {
    register unsigned long long f, c;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f)); // get current counter frequency
    asm volatile ("mrs %0, cntpct_el0" : "=r"(c)); // read current counter
    return (float) c / f;
}

void reset(){ // reboot after watchdog timer expire
    *PM_RSTC = PM_PASSWORD | 0x20; // full reset
    *PM_WDOG = PM_PASSWORD | 100;   // number of watchdog tick
}

void cancel_reset() {
    *PM_RSTC = PM_PASSWORD | 0; // full reset
    *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}
