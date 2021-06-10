.global switch_to
switch_to:
  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp fp, lr, [x0, 16 * 5]
  mov x9, sp
  str x9, [x0, 16 * 6]

  ldp x19, x20, [x1, 16 * 0]
  ldp x21, x22, [x1, 16 * 1]
  ldp x23, x24, [x1, 16 * 2]
  ldp x25, x26, [x1, 16 * 3]
  ldp x27, x28, [x1, 16 * 4]
  ldp fp, lr, [x1, 16 * 5]
  ldr x9, [x1, 16 * 6]
  mov sp,  x9
  msr tpidr_el1, x1
  cmp x2, #0
  beq 1f
  br x2
1:
  ret

.global get_current
get_current:
  mrs x0, tpidr_el1
  ret

.global fork_stack
fork_stack:
  ldr x3, [x1], #8
  str x3, [x0], #8
  subs x2, x2, #8
  bne fork_stack
  ret

.global thread_copy_reg
thread_copy_reg:
  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp x1, x2, [x0, 16 * 5]
  str x3, [x0, 16 * 6]
  cmp x2, #0
  bne 1f
  stp x1, lr, [x0, 16 * 5]
1:
  str x3, [x0, 16 * 6]
  ret

.global load_template
load_template:
  mov x3, x0
  ldp x0, x1, [x1]
  blr x3
  mov x22, #5
  svc #0
