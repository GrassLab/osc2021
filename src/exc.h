#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "data_type.h"
void exc_error (int error);

/* handler */
void sp_elx_irq_handler ();
int sys_call_handler ();
void aarch64_irq_handler ();

void print_el1_exc ();

void enable_core_timer ();
#endif
