.section ".text.start"

_start: 
    ldr     x0, = bss_begin
    ldr     x1, = bss_end
    sub     x1, x1, x0
    bl      clear

    // sp_el0 is set before start
    bl       app

clear:
    // clear bss memory
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear
    ret     lr