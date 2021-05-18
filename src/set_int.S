#include "context.h"

.global from_el2_to_el1
from_el2_to_el1:
    mov    x0, (1 << 31)
    msr    hcr_el2, x0
    mov    x0, 0x3c5
    msr    spsr_el2, x0
    msr    elr_el2, lr
    eret

.global from_el1_to_el0
from_el1_to_el0:
    msr    elr_el1, x3
    msr    sp_el0, x2
    mov    x4, 0x0
    msr    spsr_el1, x4
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

    b    cur_el_svc_handler
    .align 7
    b    interrupt_handler
    .align 7
    b    invalid_exception
    .align 7
    b    invalid_exception
    .align 7

    b    low_el_svc_handler
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
    bl  times_up
    kernel_exit
    eret

// =====Exception=====

low_el_svc_handler:
    kernel_entry
    sub  x8, x8, 2
    cbz  x8, check_file
    mov  x0, sp
    bl   el0_svc_handler
    b    leave_exception

check_file:
    ldr  x0, [sp, 16 * 0]
    bl   search_file
    mov  w1, 0xffffffff
    sub  w1, w1, w0
    cbnz w1, sys_exec
    bl   times_up
    kernel_exit
    mov  w0, 0xffffffff
    mov  x8, 0
    eret

sys_exec:
    str  w0, [sp, 16 * 1]
    kernel_exit
    mov  x8, 0
    bl   do_exec

// =====Interrupt=====

interrupt_handler:
    kernel_entry
    bl    irq_route
    b     leave_exception

// ===================

cur_el_svc_handler:
    kernel_entry
    b   leave_exception

invalid_exception:
    eret
