.section ".text"

// Rpi 3 has 4 CPUs and load this file to GPU to run.
// let only one  processor handle this by let the other 3 processors enter low power mode by instruction "wfi" 

_start:
    mrs   x1, MPIDR_EL1                     // mrs => move from system register.  mpidr_el1 => system register for core identification
    and   x1, x1, #3                        // select the first processor by logical and
    cbz   x1, _init                         // cbz => compare and branch on zero
    b     _low_power_mode                   // b => unconditional jump

_init:
    // set stack pointer to here
    ldr   x1, =_start                       // ldr => load register, ",=" involves ARM pseudo instruction 
    mov   sp, x1                            // copy the address of "_start" to stack pointer


    ldr   x1, =__bss_start                  // __bss_start defined in linker.ld
    ldr   x2, =__bss_size                   // __bss_size in word(8byte in 64bit), also defined in linker.ld
_reset_loop:
    cbz   x2, _entry                        // if the size of not cleared .bss is zero, jump to entry
    str   xzr, [x1], #8                     // str => store register, xzr => const register representing 0
                                            // str value, [base address], offset(in byte).  note that the base register will also be added with offset
    sub   x2, x2, #1                        // size of not cleared bss - 1
    cbnz  x2, _reset_loop                   // cbnz => compare and branch on non-zero

_low_power_mode:
    wfe                                     // wfe => wait for event
    b     _low_power_mode                   // should not be executed

_entry:
    bl    main                              // bl => branch with link, unconditional jump to where link is
    b     _low_power_mode