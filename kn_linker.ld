SECTIONS
{
    . = 0x80000;
    .text : { *(.text .text.*) }

    . = ALIGN(4096);
    .rodata : { *(.rodata .rodata.*) }

    . = ALIGN(4096);
    PROVIDE(_data = .);
    .data : { *(.data .data.*) }

    . = ALIGN(4096);
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}