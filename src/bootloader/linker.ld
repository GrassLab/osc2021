_begin = 0x80000;
_stack_top = 0xA0000;
_relocate_addr = 0x60000;

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