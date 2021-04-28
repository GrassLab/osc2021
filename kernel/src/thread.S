.global get_current_thread
get_current_thread:
    mrs     x0, TPIDR_EL0
    ret

.global set_current_thread
set_current_thread:
    msr     TPIDR_EL0, x0
    ret
