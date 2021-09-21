.section ".text.boot"

.global _start
_start:
    bl      main                      // jump to main functoin in C code, should not return
    b       proc_hang                 // for failsafe, halt this core too (Should never do this instruction)

proc_hang:  
    wfe                             // hang for all noo-primary CPU and enter low power mode
    b       proc_hang    

