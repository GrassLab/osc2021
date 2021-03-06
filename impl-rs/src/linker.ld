SECTIONS
{
    . = 0x80000;
    .text :
    {
        *(.text._start) *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss ALIGN(8):
    {
        __bss_start = .;
        *(.bss*);
        . = ALIGN(8);
        /* Fill for the bss == 0 case, so that __bss_start <= __bss_end_inclusive holds */
        . += 8;
        __bss_end_inclusive = . - 8;
    }

    /DISCARD/ : { *(.comment*) }
}
