_stack_top = 0x48000;

SECTIONS
{
    . = 0x80000;
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