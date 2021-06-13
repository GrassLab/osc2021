__heap_size     = 0x00002000;
SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    __prog_start = .;
    .text : { KEEP(boot.o(.text)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    . = ALIGN(16);
    __heap_start = .;
    __stk_start = . + (1<<16);
    __prog_end = __stk_start;
}
__bss_size = (__bss_end - __bss_start)>>3;