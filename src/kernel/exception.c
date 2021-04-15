#include "io.h"
#include "timer.h"
void irq_parser(void *source_addr)
{

    int source = *((int *)source_addr);
    if (source == 2) {
        // counter
        printf("%d\n", source);
        // core_timer_reset();
        
    }
}