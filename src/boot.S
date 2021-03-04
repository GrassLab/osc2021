#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF       // Check processor id
    cbz     x0, master          // Hang for all non-primary CPU
    b       proc_hang

proc_hang:
    wfe
    b       proc_hang

master:
    // Clear bss
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero

    // Jump to C code
    mov     sp, #LOW_MEMORY
    bl      kernel_start