.global time_freq
time_freq:
    mrs x0, CNTFRQ_EL0
    ret

.global time_counter
time_counter:
    mrs x0, CNTPCT_EL0
    ret
