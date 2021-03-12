.global time_freq
time_freq:
    mrs x0, CNTFRQ_EL0
    ret

.global time_counter
time_counter:
    mrs x0, CNTPCT_EL0
    ret

.global get_bootloader_start
get_bootloader_start:
    ldr x0, =bootloader_start
    ret

.global get_bootloader_end
get_bootloader_end:
    ldr x0, =bootloader_end
    ret

.global get__start
get__start:
    ldr x0, =_start
    ret

.global jump_address
jump_address:
    br x0
