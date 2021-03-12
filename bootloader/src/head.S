.section ".text.start"
.global _start
_start:
    mov x1, #8
    tst x1, #8
    b.eq bss_init_loop
    ldr x0, = __stack_base
    mov sp, x0

/* relocate boot loader */
    bl relocate_process
    ldr x0, =new_loader_address
    br x0

new_loader_address:
bl clear_memory

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
