#ifndef _IRQ_H
#define _IRQ_H
#include "uart.h"
#include "utils.h"
#include "timer.h"
#include "vt.h"


void enable_interrupt();
void disable_interrupt();
void default_exception_handler();
void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1);
void handle_sync_el0(unsigned long esr_el1, unsigned long elr_el1);

#endif
