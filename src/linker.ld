_stack_top = 0x8100;

SECTIONS
{
    . = 0x80000;
    .text : 
    { 
        *(.boot) 
        *(.text) 
    }
    .data : 
    {
        *(.rodata)
        *(.data)
    }
    bss_begin = .;
    .bss : { *(.bss) }
    bss_end = .;
}