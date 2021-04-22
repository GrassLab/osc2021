_begin = 0x80000;

SECTIONS
{
    . = _begin;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .data : 
    {
        *(.rodata)
        *(.data)
    }
    . = ALIGN(8);
    bss_begin = .;
    .bss : { *(.bss) }
    .= ALIGN(8);
    bss_end = .;
}