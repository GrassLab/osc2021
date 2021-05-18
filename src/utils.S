// get current exception level
.globl get_el 
get_el:
    mrs x0, CurrentEL 
    lsr x0, x0, #2
    ret

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret