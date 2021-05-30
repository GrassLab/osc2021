
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
    bl     set_dt_base       // After this, dt_base_g will become address of device tree

    // Go to EL1h
    bl     el1_table_init      // Set EL1_table
    ldr    x0, =0x80000000     // Set HCR_EL2.RW
    //ldr    x0, =0xffff000080000000     // Set HCR_EL2.RW
    msr    hcr_el2, x0
    ldr    x0, =0x3C5          // Set SPSR_EL2.{AIF} & EL1h
    msr    spsr_el2, x0
    adr    x0, el1_entry       // Return to el1 with dedicated sp
    msr    elr_el2, x0

    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

    // mov    x0, 0
    // mov    x1, 0x40000000
    // bl     create_kernel_pgd
    // msr    ttbr0_el1, x0 // load PGD to the bottom translation based register.
    // msr    ttbr1_el1, x0 // also load PGD to the upper translation based register.


    ldr    x0, =0xffff000030000000     // Set EL1 sp in advance
    msr    SP_EL1, x0
    eret

el1_entry:
    #define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
    #define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
    #define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)
    ldr    x0, = TCR_CONFIG_DEFAULT
    msr    tcr_el1, x0

    #define MAIR_DEVICE_nGnRnE 0b00000000
    #define MAIR_NORMAL_NOCACHE 0b01000100
    #define MAIR_IDX_DEVICE_nGnRnE 0
    #define MAIR_IDX_NORMAL_NOCACHE 1
    ldr x0, =( \
      (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
      (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr    mair_el1, x0

    #define PD_TABLE 0b11
    #define PD_BLOCK 0b01
    #define PD_ACCESS (1 << 10)
    #define BOOT_PGD_ATTR PD_TABLE
    #define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
    
    mov    x0, 0         // PGD's page frame at 0x0
    mov    x1, 0x1000    // PUD's page frame at 0x1000
    ldr    x2, = BOOT_PGD_ATTR
    orr    x2, x1, x2    // combine the physical address of next level page with attribute.
    str    x2, [x0]
    ldr    x2, = BOOT_PUD_ATTR
    mov    x3, 0x00000000
    orr    x3, x2, x3
    str    x3, [x1]      // 1st 1GB mapped by the 1st entry of PUD
    mov    x3, 0x40000000
    orr    x3, x2, x3
    str    x3, [x1, 8]   // 2nd 1GB mapped by the 2nd entry of PUD

    msr    ttbr0_el1, x0 // load PGD to the bottom translation based register.
    msr    ttbr1_el1, x0 // also load PGD to the upper translation based register.
    ldr    x2, =kernel_main // indirect branch to the virtual address
    mrs    x3, sctlr_el1
    orr    x3, x3, 1
    msr    sctlr_el1, x3 // enable MMU, cache remains disabled


    br     x2
