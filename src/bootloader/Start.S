#include "mm.h"

.section ".text.boot"

_start:
    adr     x1, _start
    ldr     x2, =0x70000
    mov     x3, x2
    adr     x5, _end
    cmp     x1, x2
    beq     after_moved
relocation:
    cmp     x5, x1
    bls     to_target
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    b       relocation

to_target:
    br x3

after_moved:
    ldr     x1, =0x100000
    str     x0, [x1]            // store dt loading address to 0x100000
=======
    adr     x0, _start
    ldr     x1, =0x70000
    mov     x2, x1
    adr     x4, _end
    cmp     x0, x1
    beq     after_moved
relocation:
    cmp     x4, x0
    bls     to_target
    ldr     x3, [x0], #8
    str     x3, [x1], #8
    b       relocation

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