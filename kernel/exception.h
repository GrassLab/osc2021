#ifndef EXCEPTION_H
#define EXCEPTION_H

void log_state();

void handle_synchronous();
void handle_irq();
void handle_unknown_exception();

#endif