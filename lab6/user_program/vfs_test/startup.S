.section ".text.boot"

.global _start
_start:
    bl      main                      
    b       proc_hang               

proc_hang:  
    wfe                           
    b       proc_hang    

