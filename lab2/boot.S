
.section ".text.boot"

.globl _start
_start:
    mrs    x1, mpidr_el1        
    and    x1, x1,#0xFF        // Check processor id
    cbz    x1, master          // Hang for all non-primary CPU
    b      proc_hang

proc_hang: 
    b      proc_hang

master:
    ldr    x1, =0x30000000
    mov    sp, x1
    bl     set_dt_base         // After this, dt_base_g will become address of device tree

    // Go to EL1h
    bl     el1_table_init      // Set EL1_table
    ldr    x0, =0x80000000     // Set HCR_EL2.RW
    msr    hcr_el2, x0
    ldr    x0, =0x3C5          // Set SPSR_EL2.{AIF} & EL1h
    msr    spsr_el2, x0
    adr    x0, el1_entry       // Return to el1 with dedicated sp
    msr    elr_el2, x0
    ldr    x0, =0x30000000     // Set EL1 sp in advance
    msr    SP_EL1, x0
    //ldr    x0, =0xACA0           // 0x389ACA0
    //msr    cntfrq_el0, x0
    eret

el1_entry:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

    bl     kernel_main
