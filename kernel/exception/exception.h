#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../process/thread.h"

void log_state();

void handle_exception(struct Trap_Frame * tf);
void handle_irq();
void handle_unknown_exception();

#endif