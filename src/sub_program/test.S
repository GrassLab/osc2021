.section ".text"
.global _start
_start:
    mov x3, 0
1:
    add x3, x3, 1
    svc 0
    cmp x3, 5
    blt 1b
2:
    b 2b