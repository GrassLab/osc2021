.section = ".text"

.global _start

_start:
    mrs x1, mpidr_el1 //get CPU id
    and x1, x1, #3 
    cbz x1, 2f //if CPU id == 0, goto 2

//infinite loop for CPU id != 0
1:
    wfe
    b 1b

2:
    // setup exception vector for EL1
    ldr x0, =exception_table
    msr VBAR_EL1, x0
    
    // set stack pointer to __bss_end__ + 0x4000
    ldr x0, =_start
    add x0, x0, #0x4000
    mov sp, x0

    // setup sp_el for el0 and el1, el2 need to be set in el3
    add x0, x0, #0x2000
    msr sp_el0, x0
    add x0, x0, #0x2000
    msr sp_el1, x0

    ldr x1, = bss_begin
    ldr x2, = bss_end
    sub x2, x2, x1
    cbz x2, 4f
    bl memzero

memzero: //clear the bss
	str xzr, [x1], #8
	subs x2, x2, #8
	b.gt memzero

4:
    bl from_el2_to_el1 //jump to main.c

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    adr x0, main
    msr elr_el2, x0
    eret // return to EL1

.global from_el1_to_el0
from_el1_to_el0:
    msr spsr_el1, x0
    //adr x0, main
    msr elr_el1, x1
    eret 







