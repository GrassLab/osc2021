.section ".data"

_exception_ret_addr: .asciz "Exception return address: "
_exception_class: .asciz "Exception class (EC): "
_exception_iss: .asciz "Instruction specific syndrom (ISS): "
_exception_spsr: .asciz "SPSR: "

.section ".text"

.align 11 // aligned to 0x800 (2^11)
.global _exception_vector_table
_exception_vector_table:
    b _exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80 (2^7), .align will pad 0
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _irq_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

hang:
    wfe
    b hang

.macro _kernel_entry
sub sp, sp, #256 // size of all registers x0-x30 (31 * 16)
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
.endm

.macro _kernel_exit
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
add sp, sp, #256
.endm

.global _exception_handler
_exception_handler:
    _kernel_entry

    bl _except_handler
    _kernel_exit

    eret

.global _irq_handler
_irq_handler:
    _kernel_entry

    bl _timer_handler

    _kernel_exit

    eret
