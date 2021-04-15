SECTIONS
{
    . = 0x70000;
    __prog_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_begin = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    __prog_end = .;
}