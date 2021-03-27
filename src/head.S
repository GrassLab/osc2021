.section ".text.start"
.global _start
.global boot_info
_start:
/* store flatten device tree */
mov x20, x0

/* init bss section */
    ldr x0, =__bss_start
    ldr x1, =__bss_size
    lsr x1, x1, #3
bss_init_loop:
    cbz x1, _boot_info
    str xzr, [x0], #8
    sub x1, x1, #1
    b bss_init_loop

_boot_info:
    ldr x0, =boot_info
    mov x1, #3
init_info_loop:
    cbz x1, init_kernel_info
    ldp x2, x3, [x20]
    stp x2, x3, [x0]
    add x0, x0, #16
    add x20, x20, #16
    sub x1, x1, #1
    b init_info_loop
init_kernel_info:
    ldr x1, =kernel_start
    ldr x2, =kernel_size
    stp x1, x2, [x0]
    ldr x1, =__bss_start
    ldr x2, =__bss_size
    stp x1, x2, [x0, #16]
    ldr x1, =__stack_base
    ldr x2, =__stack_size
    sub x1, x1, #0x10
    stp x1, x2, [x0, #32]

/* bootloader stack */
init_stack:
    ldr x0, = __stack_base
    sub x0, x0, #0x10
    mov sp, x0

/* append bootloader behind the tail of kernel */
    mov x0, 11
    ldr x1, =boot_info
    ldp x2, x3, [x1, #48]
    add x2, x2, x3    // end of kernel
    and x2, x2, #0xFFFFFFFFFFFFF000
    add x2, x2, #0x1000
    str x2, [x1, #32]
    mov x0, #0x70000
    bl relocate_process

_main:
    bl main
    nop
    nop
