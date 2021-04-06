.section ".text.start"
.global _start
.extern boot_info

_start:
/* store flatten device tree */
mov x20, x0
mov x21, x1

/* init bss section */
    ldr x0, =__bss_start
    ldr x1, =bootloader_offset
    add x0, x0, x1
    ldr x1, =__bss_size
    lsr x1, x1, #3
bss_init_loop:
    cbz x1, _boot_info
    str xzr, [x0], #8
    sub x1, x1, #1
    b bss_init_loop

/*
 * 0: flatten device tree address
 * 1 ~ 3: booting flag
 * 4: bootloader_addr
 * 5: bootloader_size
 * 6: kernel_addr
 * 7: kernel_size
 */
_boot_info:
    ldr x0, =boot_info
    ldr x1, =bootloader_offset
    add x0, x0, x1
    stp x20, x21, [x0]
    stp x2, x3, [x0, #16]   /* 2 ~ 3 booting flag */
    ldr x1, =bootloader_start
    ldr x2, =bootloader_size
    stp x1, x2, [x0, #32]   /* 4 ~ 5 bootloader information */
    mov x1, #0
    stp x1, x1, [x0, #48]   /* 6 ~ 7 kernel information */
    ldr x1, =__bss_start
    ldr x2, =__bss_size
    stp x1, x2, [x0, #64]   /* 6 ~ 7 kernel information */

/* bootloader stack */
    ldr x0, = __stack_base
    mov sp, x0

/* relocate boot loader */
    mov x0, #0x80000
    bl relocate_process
    ldr x0, =_main
    br x0

_main:
    bl main
    nop
    nop
