.section ".text"
.global _start

_start:
	mrs    x1, mpidr_el1 // read cpu id
	and    x1, x1, #0xff
	cbz    x1, 2f

1:
	wfe
	b 1b
2:
	bl from_el2_to_el1
	bl set_exception_vector_table
	ldr    x1, =__dtb_addr
	str    x0, [x1]
	ldr    x1, =__stack_top
	mov    sp, x1

	ldr    x1, =__bss_start
	ldr    w2, =__bss_size
3:	
	cbz    w2, 4f
	str    xzr, [x1], #8
	sub    w2, w2, #1
	cbnz   w2, 3b
4:  
	bl main
	b 1b

from_el2_to_el1:
	mov x0, (1 << 31)
	msr hcr_el2, x0
	mov x0, 0x3c5
	msr spsr_el2, x0
	msr elr_el2, lr 
	eret

.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
  b exception_handler        // branch to a handler function.
  .align 7                   // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler1
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
 

set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
  ret

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

.macro print_el registers
	sub sp, sp, 32
	str lr, [sp]
	mov x0, \registers
    bl uart_printhex
	ldr lr, [sp]
	add sp, sp, 32
.endm

exception_handler:
	eret

exception_handler1:
    save_all
    bl exception_entry
    load_all
    eret	

exception_entry:
	sub sp, sp, 32
	str lr, [sp]
	mrs x0, spsr_el1
	print_el x0
	mrs x0, elr_el1
	print_el x0
	mrs x0, esr_el1
	print_el x0
	
	ldr lr, [sp]
	add sp, sp, 32
	ret
	