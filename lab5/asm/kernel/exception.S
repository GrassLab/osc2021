.section ".text"
.macro store_context
  sub  sp, sp, #16 * 17
  stp  x0, x1, [sp, #16 * 0]
  stp  x2, x3, [sp, #16 * 1]
  stp  x4, x5, [sp, #16 * 2]
  stp  x6, x7, [sp, #16 * 3]
  stp  x8, x9, [sp, #16 * 4]
  stp  x10, x11, [sp, #16 * 5]
  stp  x12, x13, [sp, #16 * 6]
  stp  x14, x15, [sp, #16 * 7]
  stp  x16, x17, [sp, #16 * 8]
  stp  x18, x19, [sp, #16 * 9]
  stp  x20, x21, [sp, #16 * 10]
  stp  x22, x23, [sp, #16 * 11]
  stp  x24, x25, [sp, #16 * 12]
  stp  x26, x27, [sp, #16 * 13]
  stp  x28, x29, [sp, #16 * 14]
  mrs  x9, sp_el0
  stp  x30, x9, [sp, 16 * 15]
  mrs  x9, elr_el1
  mrs  x10, spsr_el1
  stp  x9, x10, [sp, 16 * 16]  
.endm
.macro restore_context 
  ldp  x30, x9, [sp, #16 * 15]
  msr  sp_el0, x9
  ldp  x9, x10, [sp, #16 * 16]
  msr  elr_el1, x9
  msr  spsr_el1, x10
  ldp  x0, x1, [sp, #16 * 0]
  ldp  x2, x3, [sp, #16 * 1]
  ldp  x4, x5, [sp, #16 * 2]
  ldp  x6, x7, [sp, #16 * 3]
  ldp  x8, x9, [sp, #16 * 4]
  ldp  x10, x11, [sp, #16 * 5]
  ldp  x12, x13, [sp, #16 * 6]
  ldp  x14, x15, [sp, #16 * 7]
  ldp  x16, x17, [sp, #16 * 8]
  ldp  x18, x19, [sp, #16 * 9]
  ldp  x20, x21, [sp, #16 * 10]
  ldp  x22, x23, [sp, #16 * 11]
  ldp  x24, x25, [sp, #16 * 12]
  ldp  x26, x27, [sp, #16 * 13]
  ldp  x28, x29, [sp, #16 * 14]
  add  sp, sp, #16 * 17
.endm

.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
  b exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b irq_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

.global _set_exception_vector_table
_set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
  ret

.global not_syscall
exception_handler:
  store_context
  #checks the source of exception
  mrs x9, esr_el1
  mov x10, x9
  #EC( 1<< 26) = 010101 = 0x15: SVC instruction execution in AArch64 state
  lsr x9, x9, #26
  cmp x9, #0x15
  bne not_syscall
  #imm16 low 16bit value 0: syscall 
  and x10, x10, 0xffff
  cmp x10, #0
  bne not_syscall
  bl syscall
  //store return value
  str x0, [sp, #16 * 0]
not_syscall:
  restore_context
  eret

irq_handler:
  store_context
  
  bl irq_routing

  #bl schedule
  
  restore_context
  eret

.global enable_interrupt
enable_interrupt:
  msr DAIFClr, 0xf
  ret

.global disable_interrupt
disable_interrupt:
  msr DAIFSet, 0xf
  ret