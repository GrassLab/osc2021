SECTIONS
{
    . = 0x80000;
    .reloc : { KEEP(*(.text.reloc)) }

    . = ALIGN(4096);
    _start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.*) }

    .rodata : { *(.rodata .rodata.*) }

    .data : { *(.data .data.*) }

    . = ALIGN(16);
    .bss : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    }

    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}

_bss_size = (_bss_end - _bss_start) >> 3;
_reloc_addr = 0x60000;