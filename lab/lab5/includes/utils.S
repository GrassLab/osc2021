.extern print_h

.global _moveTo
_moveTo:
  add sp, sp, x1
  br x0

.global _branch
_branch:
  br x0


.global _run_el0
_run_el0:
    mov x2, 0x0
    msr spsr_el1, x2 // interupt set up
    msr sp_el0, x1
    msr elr_el1, x0
    eret // return to EL0

.global _get_el
_get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

.global _get_freq
_get_freq:
    mrs x0, cntfrq_el0
    ret


