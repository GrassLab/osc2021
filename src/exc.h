#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "data_type.h"
void exc_error (int error);
void sp_elx_irq_handler ();
int sys_call_handler ();
void print_el1_exc ();
#endif
