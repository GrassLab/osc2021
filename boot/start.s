.section ".text.boot"

.global _start
_start:
    ldr x0, =_bss_size
    ldr x1, =_bss_start
bss_init:
    cbz x0, stack_init
    str xzr, [x1], #8
    sub x0, x0, #8
    b bss_init
stack_init:
    ldr x0, =_stack_start
    mov sp, x0
    bl main
suspend:
    wfe
    b suspend
