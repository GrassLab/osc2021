.section ".text.boot"
.global _start
_start:
    mov x0, (1 << 31)
    msr hcr_el2, x0
    mov x0, 0x3c5
    msr spsr_el2, x0
    adr x0, .Lel1
    msr elr_el2, x0
    eret
.Lel1:
    adr x4, _start
    adr x5, exception_vector_table
    adr x0, __bss_start__
    adr x3, __bss_end__
    mov x1, xzr
    mov sp, x4
    msr vbar_el1, x5
    sub x2, x3, x0
    bl memset
    msr tpidr_el1, xzr
    bl kmain
.Lloop:
    wfe
    b .Lloop
