.global _start
.equ EL1_STACK_SIZE, 0x10000

.section ".text.startup"
_start:
  /* initialize EL2 stack */
  ldr x9, =_el2_stack_end
  mov sp, x9
  sub sp, sp, #0x10
  str x0, [sp]

  /* initialize EL2/EL1 bss */
  ldr x20, =_sbss
  ldr x21, =_ebss
L1:
  cmp x20, x21
  b.eq L1_E
  str xzr, [x20]
  add x20, x20, #8
  b L1
L1_E:

  /* setup interrupt vector base */
  ldr x0, =el1_vector_base
  msr vbar_el1, x0

  /* initialize buddy/cache system */
  bl init_buddy
  bl init_cache

  /* allocate EL1 stack */
  mov x0, EL1_STACK_SIZE
  bl kmalloc
  add x9, x0, EL1_STACK_SIZE

  /* eret_to_el1 */
  mov x0, (1 << 31) // EL1 uses aarch64
  msr hcr_el2, x0
  mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr spsr_el2, x0
  ldr x0, =el1_start
  msr elr_el2, x0

  /* pass dtb pointer */
  ldr x0, [sp]
  eret

/* in EL1 */
el1_start:
  mov sp, x9
  bl main
  wfe
