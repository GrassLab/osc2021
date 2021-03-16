.globl memzero

memzero:
    str     xzr, [x0], #8   //store register to mem
    subs    x1, x1, #8
    b.gt    memzero
    ret