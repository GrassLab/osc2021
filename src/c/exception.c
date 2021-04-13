#include "printf.h"
#include "timer.h"

void svc_router(unsigned long spsr, unsigned long elr, unsigned long esr)
{
    unsigned int svc_n = esr & 0xFFFFFF;

    switch (svc_n)
    {
    case 0:
        printf("\n");
        printf("spsr_el1\t%x\n", spsr);
        printf("elr_el1\t\t%x\n", elr);
        printf("esr_el1\t\t%x\n", esr);
        printf("\n");

        break;
    
    case 1:
        printf("enable core timer\n");
        core_timer_enable();
        break;

    case 2:
        printf("disable core timer\n");
        core_timer_disable();
        break;

    case 3:
        set_timeout();
        break;

    default: 
        break;
    }

    return;
}

void print_invalid_entry_message()
{
    printf("invalid exception!\n");
    return;
}
