
.section ".text.boot"


_start:
    // load bss position
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    
    // clear bss memory
    clear:
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear

    // set stack position, defined in linker.ld
    ldr     x0, = _stack_top
    mov     sp, x0

    // start kernel
    bl      kernel
