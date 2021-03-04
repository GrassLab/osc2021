.section ".text.start"
.global _start
_start:
    ldr x0, = __stack_base
    mov sp, x0

/* init bss section */
    ldr x0, =__bss_start
    ldr x1, =__bss_size
bss_init_loop:
    cbz x1, _main
    str xzr, [x0], #8
    sub x1, x1, #1
    b bss_init_loop

_main:
    bl main
    nop
    nop
