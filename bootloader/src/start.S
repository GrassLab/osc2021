.section ".text.boot"

_prepare:
    // if relocated
    subs    x0, x0, #555
    beq     start

    // else do initialization
    ldr     x0, = bss_begin
    ldr     x1, = bss_end
    sub     x1, x1, x0

clear:
    // clear bss memory
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear

    // set stack position, defined in linker.ld
    ldr     x0, = _stack_top
    mov     sp, x0

    // relocate
    ldr     x0, = _relocate_addr
    bl      do_relocate

    adr     x0, _begin
    adr     x1, _relocate_addr

    sub     x2, sp, x0  // get offset of stack point
    add     sp, x1, x2  // set stack point according to the offset

    //  set relocated flag
    mov     x0, xzr
    add     x0, x0, #555

    br      x1  // jump to relocated address
start:
    // start bootloader
    ldr     x0, = _begin
    bl      main

    // jump to kernel
    br      x0
