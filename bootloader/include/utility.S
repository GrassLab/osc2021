.extern boot_info
.global time_freq
time_freq:
    mrs x0, CNTFRQ_EL0
    ret

.global time_counter
time_counter:
    mrs x0, CNTPCT_EL0
    ret

.global get__start
get__start:
    ldr x0, =_start
    ret

.global jump_address
jump_address:
    mov x20, x0
    ldr x0, =boot_info
    mov x1, #0xcafe
    br x20
