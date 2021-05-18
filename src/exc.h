#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "data_type.h"

struct trap_frame {
    u64 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
        x30, null1, sp, lr;
};
void exc_error (int error, struct trap_frame *tf);

/* handler */
void sp_elx_irq_handler ();
int sys_call_handler (struct trap_frame *tf);
void aarch64_irq_handler ();

void print_el1_exc ();

void enable_core_timer ();
#endif
