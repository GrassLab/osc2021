SECTIONS
{
    . = 0x80000;
    .text : { *(.text) }
    .data : { *(.data) }
    bss_begin = .;
    .bss : { *(.bss) }
    bss_end = .;
    . = 0x180000;
    _stack_base = .;
}