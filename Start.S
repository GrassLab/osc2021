#include "mm.h"

.section ".text.boot"

_start:
    ldr     x0, _start
    ldr     x1, =0x80000
    mov     x2, x1
    ldr     x4, _end
    cmp     x0, x1
    beq     after_moved
moving:
    cmp     x4, x0
    bls     to_target
    ldr     x3, [x0], #8
    str     x3, [x1], #8
    b       moving

to_target:
    br x2

after_moved:
    // Clear bss
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero

    // Jump to C code
    mov     sp, #LOW_MEMORY
    bl      boot_main

end:
    wfe
    b end