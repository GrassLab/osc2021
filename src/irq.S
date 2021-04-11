.globl enable_irq
enable_irq:
    msr DAIFClr, 0xf
    ret

.globl disable_irq
disable_irq:
    msr DAIFSet, 0xf
    ret

.globl sync_call_uart
sync_call_uart:
    svc #3
    eret