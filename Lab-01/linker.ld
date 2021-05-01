ENTRY(_start)
SECTIONS
{
    . = 0x80000;
    .text : { 
    	KEEP(*(.text.boot)) *(.text) 
    }
    . = ALIGN(4);
    .rodata : { 
    	*(.rodata) 
    }
    . = ALIGN(4);
    .data : { 
    	*(.data) 
    }
    . = ALIGN(4);
    .bss (NOLOAD) : {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss)
        . = ALIGN(4);
        __bss_end = .;
    }
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;