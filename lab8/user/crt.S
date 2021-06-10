.global _start
.section ".text"
_start:
    ldr x0, [sp]
    add x1, sp, #0x8
    bl main
    b exit
