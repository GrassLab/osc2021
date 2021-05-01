#include "context.h"

.global from_el2_to_el1
from_el2_to_el1:
    mov    x0, (1 << 31)
    msr    hcr_el2, x0
    mov    x0, 0x5
    msr    spsr_el2, x0
    msr    elr_el2, lr
    eret

.global from_el1_to_el0
from_el1_to_el0:
    msr    elr_el1, x0
    msr    sp_el0, x0
    mov    x0, 0x0
    msr    spsr_el1, x0
    eret

.global set_exception_vector_table
set_exception_vector_table:
    adr    x0, exception_vector_table
    msr    vbar_el1, x0
    ret

.align 11
.global exception_vector_table
exception_vector_table:
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7

    b    exception_handler
    .align 7
    b    interrupt_handler
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7

    b    exception_handler
    .align 7
    b    interrupt_handler
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7

    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7

.global leave_exception
leave_exception:
    load_all
    eret

exception_handler:
    save_all
    b    print_content

print_content:
    mrs   x0, spsr_el1
    mrs   x1, elr_el1
    mrs   x2, esr_el1
    bl    print_sysreg
    b     leave_exception

interrupt_handler:
    save_all
    bl    irq_route
    b     leave_exception

invalid_exception:
    eret
