SECTIONS												/* Use the SECTIONS keyword to declare a section, the following is its content */
{
    . = 0x80000;										/* move the 'location counter' to 0x30000 */
    .text : 											/* assign .text section */
	{ 
		KEEP(*(.text.boot)) 							/* make sure .text.boot will not be remove by linker-time garbage collection. */
		*(.text .text.*) 								/* *(.text .text.*) is all .text and .text. */
	}	
    .rodata : 											/* assign .rodata section */
	{ 
		*(.rodata .rodata.*) 							/* *(.rodata .rodata.*) is all .rodata and .rodata. */
	}					
    .data : 											/* assign .data section */
	{ 
		*(.data .data.*) 								/* *(.data .data.*) is all .data and .data. */
	}						
    .bss (NOLOAD) : 									/* assign .bss section, NOLOAD means not load at runtime */
	{									
        . = ALIGN(16);									/* aligned to 16-byte boundaries. ex.0x0000000000080000 */
        __bss_start = .;								/* assign __bss_start to the current memory position */
        *(.bss .bss.*)									/* *(.bss .bss.*) is all .bss and .bss. */
        *(COMMON)										/* assign uninitialized data section */
        __bss_end = .;									/* assign __bss_end to the current memory position */
    }
    _end = .;											/* assign _end to the current memory position */
}
__bss_size = (__bss_end - __bss_start) >> 3;			/* bss_size =  (bss_end - bss_start) / 8 */