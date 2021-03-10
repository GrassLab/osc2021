_stack_top = 0x61000;
_kernel_begin = 0x81000;

SECTIONS
{
    . = 0x60000;
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