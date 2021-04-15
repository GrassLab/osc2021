__heap_size  = 0x8000;
__stack_size = 0x8000;
SECTIONS
{
    . = 0x80000;
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
    . = ALIGN(0x1000);
    .heap : {
		
		__heap_top = .;
		. = . + __heap_size;
		*(.heap)
	}
	. = ALIGN(0x1000);
 	.stack : {
		. = . + __stack_size;
		__stack_top = .;
 	}
 	__prog_end = .;
    
}
