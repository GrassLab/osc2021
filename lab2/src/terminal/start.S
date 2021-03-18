.section ".text.boot"
# Do not use x0 register, that is for the address for dtd
_start:
    adr x3, _start
    adr x1, __bss_start__
    adr x2, __bss_end__
    mov sp, x3
set_bss_zero_loop:
    cmp x2, x1
    bls entry
    ldr xzr, [x1], #8
    b set_bss_zero_loop
entry:
    bl main
loop:
    wfe
    b loop
