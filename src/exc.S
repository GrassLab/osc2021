#define ASM
#include "syscall.h"
#define CPACR_EL1_VAL ((0b11 << 20) | (0b11 << 16))

.section ".text.exc"

.global el2_to_el1_preserve_sp
.global set_el1_evt
.global _exec_usr
.global ret_exc

el2_to_el1_preserve_sp:
    mov x0, #(1 << 31)
    msr hcr_el2, x0
    mov x0, #0x3c5
    msr spsr_el2, x0
    ldr x0, =CPACR_EL1_VAL
    msr cpacr_el1, x0
    adr x0, el1_set_sp
    msr elr_el2, x0
    mov x0, sp
    eret

el1_set_sp:
    mov sp, x0
    ret

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

.macro save_all
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    str x30, [sp, #-16]!
.endm

.macro load_all
    ldr x30, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
.endm

.macro clear_reg
    mov x0, 0
    mov x1, 0
    mov x2, 0
    mov x3, 0
    mov x4, 0
    mov x5, 0
    mov x6, 0
    mov x7, 0
    mov x8, 0
    mov x9, 0
    mov x10, 0
    mov x11, 0
    mov x12, 0
    mov x13, 0
    mov x14, 0
    mov x15, 0
    mov x16, 0
    mov x17, 0
    mov x18, 0
    mov x19, 0
    mov x20, 0
    mov x21, 0
    mov x22, 0
    mov x23, 0
    mov x24, 0
    mov x25, 0
    mov x26, 0
    mov x27, 0
    mov x28, 0
    mov x29, 0
.endm

set_el1_evt:
    adr x0, el1_evt
    msr vbar_el1, x0
    ret
  
_exc_not_imp:
    save_all
    bl exc_not_imp
    load_all
    eret

_syn_handler:
    save_all
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, sp_el0
    mrs x3, spsr_el1
    mov x4, sp
    bl syn_handler
    load_all
    eret

ret_exc:
    msr elr_el1, x0
    msr sp_el0, x1
    msr spsr_el1, x2
    mov sp, x3
    load_all
    eret

_irq_handler:
    save_all
    mrs x0, elr_el1
    mrs x1, sp_el0
    mrs x2, spsr_el1
    mov x3, sp
    bl irq_handler

_exec_usr:
    msr elr_el1, x0
    msr sp_el0, x1
    msr spsr_el1, x2
    clear_reg
    eret

.section ".vec_tbl"

.align 11
el1_evt:
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp

    .align 7
    b _syn_handler
    .align 7
    b _irq_handler
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp

    .align 7
    b _syn_handler
    .align 7
    b _irq_handler
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp

    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp
    .align 7
    b _exc_not_imp