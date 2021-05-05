SECTIONS {
    . = 0x10000000;
    .text.start : {*(.text.start)}
    .bss  : { 
        __bss_start__ = .;
        *(.bss)
        __bss_end__ = .;
    }
    .text : { *(.text) }
    .data : { *(.data) }
    _end = .;
}
