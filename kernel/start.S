/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

.section ".text.start"

.global _start

_start:
1:  // set el2 to el1
    bl      from_el2_to_el1

    // set exception vector table
    adr x0, exception_vector_table
    msr vbar_el1, x0

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

2:  cbz     w2, 3f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 2b

    // jump to C code, should not return
3:  bl      main
    // for failsafe, halt this core too
    b       1b


from_el2_to_el1:
    mov     x0,       (1 << 31) // EL1 uses aarch64
    msr     hcr_el2,  x0
    mov     x0,       0x3c5     // EL1h (SPSel = 1) with interrupt disabled
    msr     spsr_el2, x0
    msr     elr_el2,  lr
    eret                        // return to EL1

// save general registers to stack
.macro save_all
    sub sp, sp, 36 * 8
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

    mrs x19, sp_el0
    mrs x20, elr_el1
    mrs x21, spsr_el1

    stp x19, x20, [sp, 16 * 16]
    str x21, [sp, 16 * 17]
    mov x0, sp
    ands x1, x0, 15
    sub x0, x0, x1
    mov sp, x0
.endm

// load general registers from stack
.macro load_all
    ldp x21, x20, [sp, 16 * 17]
    ldp x19, x20, [sp, 16 * 16]

    msr spsr_el1, x21
    msr elr_el1, x20
    msr sp_el0, x19

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
    add sp, sp, 36 * 8
.endm

// Exception Table
.align 11               // vector table should be aligned to 0x800
.global exception_vector_table

exception_vector_table:
  b unknown_handler   // branch to a handler function.
  .align 7              // entry size is 0x80, .align will pad 0
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7

  b unknown_handler
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7

  b el0_exception
  .align 7
  b el0_IRQ
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7

  b unknown_handler
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7
  b unknown_handler
  .align 7

unknown_handler:
  save_all
  bl handle_unknown_exception
  load_all
  eret

el0_exception:
  save_all
  mov x0, sp
  bl handle_exception
  load_all
  eret

el0_IRQ:
  save_all
  bl handle_irq
  load_all
  eret

.global child_return_from_fork
child_return_from_fork:
  load_all
  eret