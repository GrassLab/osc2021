#include "printf.h"

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

    default:
        break;
    }

    return;
}


void print_core_timer()
{
    printf("core timer interrupt!\n");
    return;
}

void show_invalid_entry_message()
{
    printf("unknown exception!\n");
    return;
}