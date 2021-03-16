SECTIONS
{
	. = 0x80000;
	.relocate : {
		KEEP ( *(.text.relocate) )
	}
	. = ALIGN(8);
	_kernel_begin = .;
	.text : { 
		KEEP( *(.text.boot) )  /* KEEP: make sure datum in this section will not be affected by linker's garbage collection mechanism*/
		*(.text)
	}
	.data : {
		*(.data)
	}
	/* ALIGN: map address to multle of 16,  NOLOAD: mark a section to not be loaded at run time */
	.bss ALIGN(16) (NOLOAD) : {
		__bss_start = .;

		*(.bss)

		__bss_end = .;
	}
	_kernel_end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;  /* translate the unit from byte to word */
__boot_loader = 0x30000000;
