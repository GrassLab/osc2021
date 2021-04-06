__relocate_address = 0x60000;


SECTIONS {
    . = 0x80000;
    __begin = .;
    .text : {
        KEEP(*(.text.boot));
        *(.text)
    }
    .loader : {
        __begin_loader = .;
        KEEP(*(.text.loader));
        KEEP(*(.text.loader.util));
        __end_loader = .;
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
    __end = .;
}
