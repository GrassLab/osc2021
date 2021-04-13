.globl put32
put32:
    str     w1, [x0]
    ret

.globl get32
get32:
    ldr     w0, [x0]
    ret

.globl delay
delay:
    subs    x0, x0, #1
    bne delay
    ret

.globl run_program
run_program:
    mov     x0, 0x3c0
    msr     spsr_el1, x0
    ldr     x0, =0x70000     // the target address of program
    msr     elr_el1, x0
    msr     sp_el0, x0
    eret

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret