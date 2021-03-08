SECTIONS {
    . = 0x80000;
    .text.boot : { *(.text.boot) }
    .bss  : { 
        . = ALIGN(8); /* Make sure that we can initialize it by using 64-bit register. */
        __bss_start__ = .;
        *(.bss)
        . = ALIGN(8);
        __bss_end__ = .;
    }
    .text : { *(.text) }
    .data : { *(.data) }
}
