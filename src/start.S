#include "sysregs.h"
#include "mm.h"
#include "base.h"
#include "mmu.h"

.section ".text.boot"

.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3              // get CPU ID and mask reserved bits
    cbz     x1, clear_bss
    // cpu id > 0, stop
proc_hang:  
    wfe                             // hang for all noo-primary CPU and enter low power mode
    b       proc_hang    

clear_bss:
    // clear bss
    adr     x0, __bss_start
    adr     x1, __bss_end
    sub     x1, x1, x0
    bl      memzero

    // jump into master label. cpu id == 0, is primary CPU 
master:  
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED   // disable MMU 
    msr     sctlr_el1, x0

    ldr     x0, =HCR_EL2_VALUE       // set EL1 aarch as aarch 64
    msr     hcr_el2, x0

    ldr	    x0, =SPSR_EL2_VALUE      // set processor restore state after calling eret
    msr	    spsr_el2, x0

    adr	    x0, el1_entry		     // eret return address
    msr	    elr_el2, x0

    eret		                     // from el2 to el1

el1_entry:
    ldr     x1, =CPACR_EL1_VALUE     // let el0, el1 can use Floating point and Advanced SIMD
    msr     CPACR_EL1, x1

    //ldr     x1, =_start               // set EL1 stack before our code
    //mov     sp, x1

    ldr     x0, =exception_vector_table      // set up el1 exception table
    msr     vbar_el1, x0

    //ldr     x0, =SPSR_EL1_VALUE
    //msr     spsr_el1, x0

    ldr	    x0, =0x3c5                // set processor restore state after calling eret
    msr	    spsr_el1, x0              // kernel should be running in EL1

    //ldr     x1, =0x40000              // set EL0 stack pointer
    //msr     sp_el0, x1

    /* Prepare MMU config */
    bl      __create_page_tables      // Create Kernel's Page table
    
    adrp	x0, pg_dir				  // starting address of page table(PUD) defined in ld
    msr	    ttbr0_el1, x0             // load PGD to the bottom translation based register.         
    msr     ttbr1_el1, x0             // load PGD to the upper translation based register.

    ldr	    x0, =(TCR_VALUE)
    msr	    tcr_el1, x0

    ldr 	x0, =(MAIR_VALUE)
    msr	    mair_el1, x0

    ldr     x0, =_kernel_start        // set EL1 stack pointer (virtual address) before our code
    mov     sp, x0
    
    ldr     x2, =main                 // save absolute address for jumping to main functoin in C code after enable MMU

    ldr     x1, =SCTLR_MMU_ENABLED    // Enable MMU
    mrs     x0, sctlr_el1
    orr     x0 , x0, x1
    msr     sctlr_el1, x0             

    // Jump to main functoin in C code, should not return
    br      x2                        

    .macro  create_pgd_entry, tbl, virt, tmp1, tmp2
    create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
    create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
    .endm

    .macro  create_table_entry, tbl, virt, shift, tmp1, tmp2
    lsr	\tmp1, \virt, #\shift
    and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1           // table index
    add	\tmp2, \tbl, #PAGE_SIZE
    orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE	
    str	\tmp2, [\tbl, \tmp1, lsl #3]
    add	\tbl, \tbl, #PAGE_SIZE                      // next level table page
    .endm

    .macro  create_block_map, tbl, phys, start, end, flags, tmp1
    lsr	\start, \start, #SECTION_SHIFT
    and	\start, \start, #PTRS_PER_TABLE - 1         // table index
    lsr	\end, \end, #SECTION_SHIFT
    and	\end, \end, #PTRS_PER_TABLE - 1             // table end index
    lsr	\phys, \phys, #SECTION_SHIFT
    mov	\tmp1, #\flags
    orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT     // table entry
9999:	
    str	\phys, [\tbl, \start, lsl #3]               // store the entry
    add	\start, \start, #1                          // next entry
    add	\phys, \phys, #SECTION_SIZE                 // next block
    cmp	\start, \end
    b.ls	9999b
    .endm

    /* Ready jump to c code */
    /* 
    adr     x0, pseudo_main
    msr     elr_el1, x0

    eret        
    */                      

/* Now kernel should be running in EL1 and jump to main function*/
/* 
pseudo_main:  
    bl      main                      // jump to main functoin in C code, should not return
    b       proc_hang                 // for failsafe, halt this core too (Should never do this instruction)
*/


__create_page_tables:
    mov     x29, x30                                            // save return address

    /* clear page tables */
    adrp    x0, pg_dir                                          // starting address of page table(PUD) defined in ld
    mov     x1, #PG_DIR_SIZE
    bl      memzero

    adrp    x0, pg_dir
    mov     x1, #VA_START
    create_pgd_entry x0, x1, x2, x3

    /* Mapping kernel and init stack*/
    mov 	x1, xzr                                             // start mapping from physical offset 0
    mov 	x2, #VA_START                                       // first virtual address
    ldr     x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)        // last virtual address
    create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

    /* Mapping device memory*/
    mov     x1, #DEVICE_BASE                                    // start mapping from device base address 
    ldr     x2, =(VA_START + DEVICE_BASE)                       // first virtual address
    ldr     x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)   // last virtual address
    create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

    mov     x30, x29
    ret