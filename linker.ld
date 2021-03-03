SECTIONS
{
	. = 0x80000;
	.text : { 
		KEEP( *(.text) )  /* KEEP: make sure datum in this section will not be affected by linker's garbage collection mechanism*/
		*(*text)
	}
	.data : {
		*(.data)
	}
	.bss ALIGN(16) (NOLOAD) : { /* ALIGN: map to multle of 16,  NOLOAD: mark a section to not be loaded at run time */
		__bss_start = .;

		*(.bss)

		__bss_end = .;
	}

}

__bss_size = (__bss_end - __bss_start) >> 3;
