_stack_top = 0x480000;

SECTIONS
{
    . = 0x81000;
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