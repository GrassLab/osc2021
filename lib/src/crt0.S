.section ".text.start"
.globl _pstart
_pstart:
    ldp x0, x1, [sp, 0x0]

.global main
    bl main
    mov x0, #0
    bl exit
