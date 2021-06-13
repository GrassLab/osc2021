.global _start
.equ EL1_STACK_SIZE, 0x10000

#define PA(p) (p - 0xffff000000000000)

.section ".text.startup"
_start:
  /* initialize EL2 stack */
  ldr x9, =PA(_el2_stack_end)
  mov sp, x9
  sub sp, sp, #0x10

  /* store dtb pointer */
  str x0, [sp]

  /* since there's no EL2, use the same stack */
  mov x9, sp
  msr sp_el1, x9

  /* initialize EL2/EL1 bss */
  ldr x20, =PA(_sbss)
  ldr x21, =PA(_ebss)
L1:
  cmp x20, x21
  b.eq L1_E
  str xzr, [x20]
  add x20, x20, #8
  b L1
L1_E:

  /* setup interrupt vector base (in VA) */
  ldr x0, =el1_vector_base
  msr vbar_el1, x0

  /* eret_to_el1 */
  mov x0, (1 << 31) // EL1 uses aarch64
  msr hcr_el2, x0
  mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr spsr_el2, x0
  ldr x0, =PA(el1_start)
  msr elr_el2, x0
  eret

/* in EL1 */
el1_start:
  bl setup_identity_mapping
  /* fix fp, sp */
  mov x1, #0
  movk x1, #0xffff, lsl 48
  mov x0, sp
  orr x0, x0, x1
  mov sp, x0
  mov x0, fp
  orr fp, x0, x1

  ldr x0, [sp]
  orr x0, x0, x1
  ldr x9, =main
  br x9

