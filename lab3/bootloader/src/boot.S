.section ".text.boot"

.global _boot

_boot:
    // cpu register size is 64bit
    // read cpu id, stop slave cores
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
    // cpu id > 0, stop
1:  
    wfe
    b 1b

2:
    // set stack top
    ldr x1, =__stack_top
    mov sp, x1

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    ldr x7, =__bootloader_size

    // compute offset, offset value is stored at x3
    ldr x3, =0x80000
    sub x3, x3, #0x60000

    // store main function address before move
    ldr x4, =main

    ldr x5, =0x80000
    ldr x6, =0x60000

clear_bss:
    cbz x2, move_bootloader

    // post-indexed store
    // first store 0 at address in register x1
    // then the address in x1 is added 8bytes
    str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, clear_bss

move_bootloader:
    cbz x7, jump_to_main

    ldr x8, [x5], #8 
    str x8, [x6], #8
    sub x7, x7, #1
    cbnz x7, move_bootloader

jump_to_main:
    sub x4, x4, x3
    br x4
    b 1b



