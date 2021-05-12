.section ".text.boot"
.global _start
.global loop
_start:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    mov x1, #1
    cmp x0, x1
    b.eq .Lel1
    mov x0, (1 << 31)
    msr hcr_el2, x0
    mov x0, 0x3c5
    msr spsr_el2, x0
    adr x0, .Lel1
    msr elr_el2, x0
    mov x0, #1
    msr cntp_ctl_el0, x0
    mrs x1, cpacr_el1
    mov x0, #(3 << 20)
    orr x0, x1, x0
    msr cpacr_el1, x0
// mrs x0, sctlr_el1
// mov x1, #(~0b11010)
// and x0, x0, x1
// msr sctlr_el1, x0
    adr x5, exception_vector_table
    mov x6, #0x3c0
    msr vbar_el1, x5
    msr spsr_el1, x6
    msr tpidr_el1, xzr
    eret
.Lel1:
    adr x4, _start
    adr x0, __bss_start__
    adr x3, __bss_end__
    mov x1, xzr
    mov sp, x4
    sub x2, x3, x0
    bl memset
    bl kmain
loop:
    wfe
    b loop
