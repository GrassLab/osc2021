.section ".text"
.global main
main:
    mov x0, 0
1:
    add x0, x0, 1
    svc #0
    cmp x0, 5
    blt 1b
    svc #10
    ret
