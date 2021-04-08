#include "printf.h"

void svc_entry(unsigned long spsr, unsigned long elr, unsigned long esr)
{
    printf("\n");
    printf("spsr_el1\t%x\n", spsr);
    printf("elr_el1\t\t%x\n", elr);
    printf("esr_el1\t\t%x\n", esr);
    printf("\n");
    
    return;
}