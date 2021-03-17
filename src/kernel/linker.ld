_begin = 0x80000;
_stack_top = 0x480000;

SECTIONS
{
    . = _begin;
    .text : {
        *(.text.start) 
        *(.text)
    }
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