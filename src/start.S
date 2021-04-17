.section ".text.boot"

.global _start

_start:
    ldr     x4, =0x20000
    str     x0, [x4], #8
    str     x1, [x4], #8
    str     x2, [x4], #8
    str     x3, [x4], #8

    ldr     x4, =0x80000
    mov     sp, x4
    mov     x1, x4
    ldr     x2, =_start
    ldr     w3, =_bl_size

_reloc_loop:
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, _reloc_loop

    ldr     x1, =_bss_start
    ldr     w2, =_bss_size
    cbz     w2, _bootloader

_bss_init_loop:  
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, _bss_init_loop

_bootloader:
    bl      bootloader-0x20000

.section ".text.halt"

.global _halt

_halt:
    wfe
    b _halt