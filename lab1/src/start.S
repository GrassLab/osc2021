.section ".text.boot"
.global __my_start__
__my_start__:
    adr x1, __bss_start__
    adr x2, __bss_end__
    adr x0, __my_start__
    mov sp, x0
set_bss_zero_loop:
    tst x2, x1
    bls entry
    ldr xzr, [x1], #8
    b set_bss_zero_loop
entry:
    bl __my_main__
loop:
    wfe
    b loop
