.section ".text.sched"

.global get_pid
.global set_pid
.global get_sp
.global switch_exec
.global thread_start
.global switch_exec_clone

.macro save_pres
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x27, x28, [sp, #-16]!
    stp x29, x30, [sp, #-16]!
.endm

.macro load_pres
    ldp x29, x30, [sp], #16
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
.endm

get_pid:
    mrs x0, tpidr_el1
    ret

set_pid:
    msr tpidr_el1, x0
    ret

get_sp:
    mov x0, sp
    ret

switch_exec:
    save_pres
    mov x3, sp
    str x3, [x2]
    msr tpidr_el1, x1
    mov sp, x0
    load_pres
    ret

thread_start:
    mov x0, #1
    bl set_int_stat
    bl enable_interrupt
    bl clear_task_timer
    mov x30, x20
    ret

switch_exec_clone:
    save_pres
    mov x5, sp
    str x5, [x2]
    
    mov sp, x3
    save_pres
    mov x5, sp
    str x5, [x4]

    msr tpidr_el1, x1
    mov sp, x0
    load_pres
    ret