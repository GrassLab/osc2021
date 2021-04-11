.section ".text"
.global xcpt_test
xcpt_test:
    mov x0, 0
1:
    add x0, x0, 1
    svc 0
    ret
    cmp x0, 5
    blt 1b
1:
    b 1b