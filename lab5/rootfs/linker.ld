SECTIONS
{
    .text :
    { 
        KEEP(*(.text.boot))
        *(.text) 
    }
    .data :
    {
        . = ALIGN(8);
        *(.data) 
    }
    .bss(NOLOAD) :
    {
        . = ALIGN(16);
        *(.bss)
    }
}