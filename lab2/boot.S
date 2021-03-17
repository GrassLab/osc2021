
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
    bl     set_dt_base         // dt_base_g is address of device tree now

    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

    ldr   x0, =0x3E000000
    mov   sp, x0
    mov   x1, x2
    bl    kernel_main