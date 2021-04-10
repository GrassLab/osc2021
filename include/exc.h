#ifndef EXC_H
#define EXC_H

void el2_to_el1_preserve_sp();
void set_el1_evt();

void exec_usr(void *addr, void *sp, unsigned long pstate);
void _exec_usr(void *addr, void *sp, unsigned long pstate);
void ret_kern(void *el1_sp);
// no return
void switch_exec(void *el1_elr, void *el0_sp, unsigned long el1_spsr,
                void *el1_sp);

void enable_interrupt();
void disable_interrupt();
// set int stat to 1
void get_interrupt();
// disable int and set int stat to 0
void ret_interrupt();

// use at the end of interrupt handler
void add_task(void *task, void *data, unsigned long priority);

#endif