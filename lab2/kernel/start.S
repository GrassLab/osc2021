.section ".text.boot"

.global _start

_start:
    adr x5, DTB_HEADER
    str x0, [x5]
    b 3f
4:  wfe
    b 4b

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

.global DTB_HEADER

.data
    DTB_HEADER: .dword  0