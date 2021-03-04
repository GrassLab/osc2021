SECTIONS {
    . = 0x80000;
    .text : {
        KEEP(*(.text.boot));
        *(.text)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_begin = .;
        *(.bss*)
        __bss_end = .;
    }
}
