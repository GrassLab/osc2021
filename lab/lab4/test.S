.section ".text"
.global _start
_start:
    mov x0, 0
1:
    add x0, x0, 1
    cmp x0, 5
    blt 1b
    svc 0
1:
    b 1b