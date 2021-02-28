.section ".text.boot"
.global __my_start__
__my_start__:
    ldr x0, =__my_start__
    mov sp, x0
    bl __my_main__
loop:
    wfe
    b loop
