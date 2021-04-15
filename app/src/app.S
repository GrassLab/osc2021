.section ".text"
.global _start
_start:
    mov x0, 0
1:
    add x0, x0, 1
    mov x1, x0
    mov x0, #400
    svc 0x80
    mov x0, x1
    cmp x0, 0x5
    blt 1b
1:
    mov x0, #60
    svc 0x80
    b 1b
