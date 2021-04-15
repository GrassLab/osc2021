// enable interrupt
.globl irq_enable
irq_enable:
    msr DAIFClr, 0xf
    ret
    
// disable interrupt
.globl irq_disable
irq_disable:
    msr DAIFSet, 0xf
    ret

