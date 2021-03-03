SECTIONS
{
    . = 0x80000;
    .text : { *(.text) }
    .rodata: { *(.rodata) }
    .data : { *(.data) }
    bss_begin = .;
    .bss : { *(.bss) }
    bss_end = .;
    . = 0x081000;
    _stack_top = .;
}