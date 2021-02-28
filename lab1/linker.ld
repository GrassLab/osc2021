ENTRY(__my_start__)

SECTIONS {
    . = 0x80000;
    .text : { *(.text.boot) *(.text) }
    .data : { *(.data) }
    .bss  : { 
        __bss_start__ = .;
        *(.bss)
        __bss_end__ = .;
    }
}
