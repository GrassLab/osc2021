#ifndef __INTERRUPT_HANDLER_H
#define __INTERRUPT_HANDLER_H

#include <stdint.h>
#include "miniuart.h"
#include "printf.h"

void sys_call(uint64_t spsr_el1, uint64_t elr_el1, uint64_t esr_el1);
void print_core_timer(uint64_t cntfrq_el0, uint64_t cntpct_el0);
void dump_interrupt_reg(uint64_t type, uint64_t elr_el1, uint64_t esr_el1);

#endif
