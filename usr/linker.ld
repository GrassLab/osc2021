SECTIONS
{
    . = 0x80000;

    .text : { KEEP(*(.text.boot)) *(.text .text.*) }

    .rodata : { *(.rodata .rodata.*) }

    .data : { *(.data .data.*) }

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}