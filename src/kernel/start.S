.section ".text.start"

_start:
    ldr     x0, = bss_begin
    ldr     x1, = bss_end
    sub     x1, x1, x0

clear:
    // clear bss memory
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear

    ldr     x0, = _stack_top
    mov     sp, x0

    bl      kernel