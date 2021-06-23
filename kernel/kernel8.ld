SECTIONS {
    . = 0xffff000000000000;
    . += 0x80000;
    .text.boot : { *(.text.boot) }
    .bss  : { 
        __bss_start__ = .;
        *(.bss)
        __bss_end__ = .;
    }
    .text : { *(.text) }
    .data : { *(.data) }
    _end = .;
}
