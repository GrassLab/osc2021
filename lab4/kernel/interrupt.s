.section ".text"

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

.global from_el2_to_el1
.global from_el1_to_el0
.global set_exception_vector_table
.global get_currentEL

from_el2_to_el1:
  mov x0, (1 << 31) // EL1 uses aarch64
  msr hcr_el2, x0
  mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr spsr_el2, x0
  msr elr_el2, lr
  eret // return to EL1

//CPACR_EL1

from_el1_to_el0:
  msr elr_el1, x0
  //mov x0, 0x3c0
  mov x0, #0
  msr spsr_el1, x0
  msr sp_el0, x1
  eret

set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
  ret

get_currentEL:
  mrs x0, CurrentEL
  ret

exception_handler_loop:
  b exception_handler_loop

exception_handler_current_el_sync:
  save_all
  mov x0, #100
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_current_el_irq:
  save_all
  mov x0, #101
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_current_el_fiq:
  save_all
  mov x0, #102
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_current_el_serror:
  save_all
  mov x0, #103
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_lower_el_sync:
  save_all
  mrs x0, spsr_el1
  mrs x1, elr_el1
  mrs x2, esr_el1
  bl sys_call
  load_all
  eret
exception_handler_lower_el_irq:
  save_all
  mrs x0, cntp_ctl_el0
  ands x0, x0, 0b0100
  beq 1f
  bl core_timer_handler
  load_all
  eret
1:
  mov x0, #202
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_lower_el_fiq:
  save_all
  mov x0, #202
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret
exception_handler_lower_el_serror:
  save_all
  mov x0, #203
  mrs x1, esr_el1
  mrs x2, elr_el1
  bl dump_interrupt_reg
  load_all
  b exception_handler_loop
  //eret

.global exception_vector_table
.align 11 // vector table should be aligned to 0x800

exception_vector_table:
  b exception_handler_loop // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b exception_handler_loop
  .align 7
  b exception_handler_loop
  .align 7
  b exception_handler_loop
  .align 7

  b exception_handler_current_el_sync
  .align 7
  b exception_handler_current_el_irq
  .align 7
  b exception_handler_current_el_fiq
  .align 7
  b exception_handler_current_el_serror
  .align 7

  b exception_handler_lower_el_sync
  .align 7
  b exception_handler_lower_el_irq
  .align 7
  b exception_handler_lower_el_fiq
  .align 7
  b exception_handler_lower_el_serror
  .align 7

  b exception_handler_loop
  .align 7
  b exception_handler_loop
  .align 7
  b exception_handler_loop
  .align 7
  b exception_handler_loop
  .align 7
