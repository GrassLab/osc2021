_begin = 0x10000000;

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
    .bss : { *(.bss) }
    .= ALIGN(8);
}