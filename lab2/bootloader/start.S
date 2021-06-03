.section ".text.boot"

.global _start

_start:
    mov    x22, x0
    /* let cores enter busy loop except core0 */
    mrs    x0, mpidr_el1
    and    x0, x0, 3
    cbz    x0, 3f
4:  wfe
    b      4b

3:
    /* set stack pointer */
    ldr x0, = _start
    mov sp, x0

    /* initialize bss to 0 */
    ldr x0, = __bss_start
    ldr w1, = __bss_size
    cbz w1, 1f
2:  str xzr, [x0], #8
    sub w1, w1, #1
    cbnz w1, 2b

1:
    bl main
    b 4b