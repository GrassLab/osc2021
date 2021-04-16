.section ".text"
.global _start
_start:
    mov x0, 0
L1:
    add x0, x0, 1
    svc 0
    cmp x0, 5
    blt L1

L2:
    b L2

    hlt 0 /* force trap to EL1 */
