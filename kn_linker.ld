SECTIONS
{
    . = 0x80000;
    kn_start = .;

    .text : { 
        *(.text .text.*) 
        . = ALIGN(0x800);
        *(.vec_tbl) 
    }

    . = ALIGN(0x1000);
    .rodata : { *(.rodata .rodata.*) }

    . = ALIGN(0x10);
    PROVIDE(_data = .);
    .data : { *(.data .data.*) }

    . = ALIGN(0x1000);
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }

    . = ALIGN(0x1000);
    kn_end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
