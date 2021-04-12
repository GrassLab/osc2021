.macro save_all
    sub sp, sp, #272
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]

    mrs x22, elr_el1
    mrs x23, spsr_el1
    stp x22, x23, [sp, #16 * 16]
.endm

.macro load_all
    ldp x22, x23, [sp, #16 * 16]
    msr elr_el1, x22
    msr spsr_el1, x23

    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]
    add sp, sp, #272
    eret
.endm

.macro  set_args_and_bl type_num bl_func
     save_all
     mov x1, \type_num
     mrs x2, esr_el1
     mrs x3, elr_el1
     bl  \bl_func
     load_all
     eret
.endm

.section ".text"

.align 11 // vector table should be aligned to 0x800
.global vector_table
vector_table:
    b Sync_ELnt_handler 
    .align 7
    b IRQ_ELnt_handler 
    .align 7
    b FIQ_ELnt_handler 
    .align 7
    b Error_ELnt_handler 
    .align 7

    b Sync_ELnh_handler 
    .align 7
    b IRQ_ELnh_handler 
    .align 7
    b FIQ_ELnh_handler 
    .align 7
    b Error_ELnh_handler 
    .align 7

    b Sync_ar64_handler 
    .align 7
    b IRQ_ar64_handler 
    .align 7
    b FIQ_ar64_handler 
    .align 7
    b Error_ar64_handler 
    .align 7

    b Sync_ar32_handler 
    .align 7
    b IRQ_ar32_handler 
    .align 7
    b FIQ_ar32_handler 
    .align 7
    b Error_ar32_handler 
    .align 7

Sync_ELnt_handler:
     set_args_and_bl #0 general_exception_handler

IRQ_ELnt_handler:
     set_args_and_bl #1 general_exception_handler

FIQ_ELnt_handler:
     set_args_and_bl #2 general_exception_handler

Error_ELnt_handler:
     set_args_and_bl #3  general_exception_handler

Sync_ELnh_handler:
     set_args_and_bl #4  general_exception_handler

IRQ_ELnh_handler:
     set_args_and_bl #5  general_exception_handler

FIQ_ELnh_handler:
     set_args_and_bl #6  general_exception_handler

Error_ELnh_handler:
     set_args_and_bl #7  general_exception_handler

Sync_ar64_handler:
     set_args_and_bl #8  general_exception_handler

IRQ_ar64_handler:
     set_args_and_bl #9  general_exception_handler

FIQ_ar64_handler:
     set_args_and_bl #10 general_exception_handler

Error_ar64_handler:
     set_args_and_bl #11 general_exception_handler

Sync_ar32_handler:
     set_args_and_bl #12 general_exception_handler

IRQ_ar32_handler:
     set_args_and_bl #13 general_exception_handler

FIQ_ar32_handler:
     set_args_and_bl #14 general_exception_handler

Error_ar32_handler:
     set_args_and_bl #15 general_exception_handler


