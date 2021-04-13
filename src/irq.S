.globl enable_irq
enable_irq:
    msr DAIFClr, 0xf
    ret

.globl disable_irq
disable_irq:
    msr DAIFSet, 0xf
    ret
