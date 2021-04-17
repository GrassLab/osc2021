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

// use with interrupt disabled
void add_task(void *task, void *data, unsigned long priority);

#define ENABLE_IRQ_1 ((volatile unsigned int *)0x3f00b210)
#define ENABLE_IRQ_2 ((volatile unsigned int *)0x3f00b214)
#define ENABLE_BASIC_IRQ ((volatile unsigned int *)0x3f00b218)
#define DISABLE_IRQ_1 ((volatile unsigned int *)0x3f00b21c)
#define DISABLE_IRQ_2 ((volatile unsigned int *)0x3f00b220)
#define DISABLE_BASIC_IRQ ((volatile unsigned int *)0x3f00b224)
#define IRQ_PENDING_1 ((volatile unsigned int *)0x3f00b204)
#define IRQ_PENDING_2 ((volatile unsigned int *)0x3f00b208)
#define IRQ_BASIC_PENDING ((volatile unsigned int *)0x3f00b200)

#endif