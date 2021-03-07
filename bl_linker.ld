SECTIONS
{
    . = 0x60000;
    _sec_st = .;
    
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }

    .rodata : { *(.rodata .rodata.*) }

    .data : { *(.data .data.*) }

    .bss : {
        . = ALIGN(16);
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        _bss_end = .;
    }
    . = ALIGN(8);
    _sec_ed = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

_bss_size = (_bss_end - _bss_start) >> 3;
_bl_size = (_sec_ed - _sec_st) >> 3;