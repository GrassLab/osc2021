//extern uart_printhex
.globl _load_user_program

_load_user_program:
    // bl uart_printhex
    msr    elr_el1, x0
    msr    sp_el0, x1
    mov    x0, 0x00
    msr    spsr_el1, x0
    
    eret

.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret