SECTIONS
{
    .text : { *(.text.boot) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss  : {*(.bss*) }
}
