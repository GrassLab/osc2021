.section ".text.reloc"

.global _reloc

_reloc:
    ldr     x1, =_reloc
    mov     sp, x1
    ldr     x1, =_bss_start
    ldr     w2, =_bss_size

_bss_init_loop:  
    cbz     w2, _bss_init_fini
    str     xzr, [x1], #8
    sub     w2, w2, #1
    b       _bss_init_loop

_bss_init_fini:
    bl      relocate
    bl      _reloc_addr

.section ".text.boot"
    
_bootloader:
    bl      bootloader

.global _halt 
   
_halt:
    wfe
    b       _halt