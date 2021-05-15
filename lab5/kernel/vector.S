#include <asm/constant.h>

.macro kernel_entry
    sub sp, sp, #PT_REGS_SIZE
    stp x0, x1, [sp, #0x0]
    stp x2, x3, [sp, #0x10]
    stp x4, x5, [sp, #0x20]
    stp x6, x7, [sp, #0x30]
    stp x8, x9, [sp, #0x40]
    stp x10, x11, [sp, #0x50]
    stp x12, x13, [sp, #0x60]
    stp x14, x15, [sp, #0x70]
    stp x16, x17, [sp, #0x80]
    stp x18, x19, [sp, #0x90]
    stp x20, x21, [sp, #0xa0]
    stp x22, x23, [sp, #0xb0]
    stp x24, x25, [sp, #0xc0]
    stp x26, x27, [sp, #0xd0]
    stp x28, x29, [sp, #0xe0]

    /* save lr, sp */
    mrs	x21, sp_el0
    stp lr, x21, [sp, #0xf0]

    /* save pc (return address), flags */
    /* spsr will deside which EL to return to */
    mrs x22, elr_el1
	mrs x23, spsr_el1
    stp x22, x23, [sp, #0x100]

    /* pass pt_regs* argument */
    mov x0, sp

.endm

.macro kernel_exit
    ldp lr, x21, [sp, #0xf0]
    msr sp_el0, x21

    ldp x22, x23, [sp, #0x100]
    msr elr_el1, x22
	msr	spsr_el1, x23

    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #0x10]
    ldp x4, x5, [sp, #0x20]
    ldp x6, x7, [sp, #0x30]
    ldp x8, x9, [sp, #0x40]
    ldp x10, x11, [sp, #0x50]
    ldp x12, x13, [sp, #0x60]
    ldp x14, x15, [sp, #0x70]
    ldp x16, x17, [sp, #0x80]
    ldp x18, x19, [sp, #0x90]
    ldp x20, x21, [sp, #0xa0]
    ldp x22, x23, [sp, #0xb0]
    ldp x24, x25, [sp, #0xc0]
    ldp x26, x27, [sp, #0xd0]
    ldp x28, x29, [sp, #0xe0]

    add sp, sp, #PT_REGS_SIZE
.endm

_el1_lower_el_aarch64_sync:
    kernel_entry
    bl sync_handler
    kernel_exit
    eret

_el1_lower_el_aarch64_irq:
    kernel_entry
    bl core_timer_handler
    kernel_exit
    eret

_el1_curr_el_spx_irq:
    /* TODO: implement top-half & bottom-half */
    kernel_entry
    bl irq_handler
    kernel_exit
    eret

.global el1_vector_base
.section ".vector"
.balign 0x800
el1_vector_base:
el1_curr_el_sp0_sync:
    b el1_curr_el_sp0_sync

.balign 0x80
el1_curr_el_sp0_irq:
    b el1_curr_el_sp0_irq

.balign 0x80
el1_curr_el_sp0_fiq:
    b el1_curr_el_sp0_fiq

.balign 0x80
el1_curr_el_sp0_serror:
    b el1_curr_el_sp0_serror

.balign 0x80
el1_curr_el_spx_sync:
    b el1_sync_handler

.balign 0x80
el1_curr_el_spx_irq:
    b _el1_curr_el_spx_irq

.balign 0x80
el1_curr_el_spx_fiq:
    b el1_curr_el_spx_fiq

.balign 0x80
el1_curr_el_spx_serror:
    b el1_curr_el_spx_serror

.balign 0x80
el1_lower_el_aarch64_sync:
    b _el1_lower_el_aarch64_sync

.balign 0x80
el1_lower_el_aarch64_irq:
    b _el1_lower_el_aarch64_irq

.balign 0x80
el1_lower_el_aarch64_fiq:
    b el1_lower_el_aarch64_fiq

.balign 0x80
el1_lower_el_aarch64_serror:
    b el1_lower_el_aarch64_serror

.balign 0x80
el1_lower_el_aarch32_sync:
    b el1_lower_el_aarch32_sync

.balign 0x80
el1_lower_el_aarch32_irq:
    b el1_lower_el_aarch32_irq

.balign 0x80
el1_lower_el_aarch32_fiq:
    b el1_lower_el_aarch32_fiq

.balign 0x80
el1_lower_el_aarch32_serror:
    b el1_lower_el_aarch32_serror