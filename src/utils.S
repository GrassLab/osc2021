// get current exception level
.globl get_el 
get_el:
    mrs     x0, CurrentEL 
    lsr     x0, x0, #2
    ret

/* 
.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret
*/

.globl set_pgd
set_pgd:
    DSB     ISH             // ensure write has completed (Memory barrier)
    msr     ttbr0_el1, x0   // set pgd at user space
    tlbi    vmalle1is       // invalidate all TLB entries
    DSB     ISH             // ensure completion of TLB invalidation (Memory barrier)
    isb                     // clear pipeline
    ret