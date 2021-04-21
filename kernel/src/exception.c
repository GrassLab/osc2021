#include "io.h"
#include "timer.h"
void irq_parser(void *source_addr, long int cntpct_el0)
{
    int source = *((int *)source_addr);
    if (source == 2) {
        core_timer_disable();
        printf("cntpct_el0: %ld\n", cntpct_el0/62500000);
        core_timer_enable();
    }
}
