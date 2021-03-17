.section ".text.start"
.global _start
.extern bootloader_info

_start:
    /* store flatten device tree */
    mov x20, x0
    mov x21, x1
    ldr x0, =bootloader_info
    ldr x1, =bootloader_offset
    add x0, x0, x1
    stp x20, x21, [x0]
    stp x2, x3, [x0, #16]

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
