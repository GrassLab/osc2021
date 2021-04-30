SECTIONS
{
	. = 0x80000;
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
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
}
__bss_size = (__bss_end - __bss_start) >> 3;
