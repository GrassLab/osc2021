#include "exception.h"

// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf p.19 Appendix A: Defines
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define CORE1_TIMER_IRQ_CTRL 0x40000044
#define CORE2_TIMER_IRQ_CTRL 0x40000048
#define CORE3_TIMER_IRQ_CTRL 0x4000004c


.section ".text.boot"

_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f

1:  
    wfe
    b 1b

2:  
    bl from_el2_to_el1

    ldr x0, =exception_vector_table
    msr VBAR_EL1, x0     // set vector table address

    ldr x1, =_start
    mov sp, x1
    ldr x1, =__bss_start
    ldr x2, =__bss_size

3:  
    cbz w2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b

4:  
    // Core Timer Interrupt
    mov x0, 1
    msr cntp_ctl_el0, x0     // enable timer’s interrupt
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0    // set expired timer’s to counter's frequency
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]             // unmask timer interrupt

    bl main
    b 1b

from_el2_to_el1:
    mov x0, (1 << 31)    // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5        // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr      // restore program counter to link register
    eret                 // return to EL1

// vector table
.align 11                // vector table should be aligned to 0x800
exception_vector_table:
    b exception_handler  // 0x000, Exception fron the current EL while using SP_EL0
    .align 7             
    b exception_handler  // 0x080
    .align 7 
    b exception_handler  // 0x100
    .align 7 
    b exception_handler  // 0x180
    .align 7 // 0x200

    b exception_handler  // 0x200, Exception from the current EL while using SP_ELx
    .align 7 
    b exception_handler  // 0x280
    .align 7 
    b exception_handler  // 0x300
    .align 7 
    b exception_handler  // 0x380
    .align 7

    b exception_handler  // 0x400, Exception fron a lower EL and at least one lower EL is AArch64
    .align 7 
    b interrupt_handler  // 0x480
    .align 7 
    b exception_handler  // 0x500
    .align 7 
    b exception_handler  // 0x580
    .align 7 

    b exception_handler  // 0x600, Exception from a lower EL and all lower ELs are AArch32
    .align 7 
    b exception_handler  // 0x680
    .align 7 
    b exception_handler  // 0x700
    .align 7
    b exception_handler  // 0x780
    .align 7 

// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

exception_handler:
    save_all
    bl exception_entry
    load_all
    eret

interrupt_handler:
    save_all
    bl timer_interrupt_entry
    load_all
    eret
