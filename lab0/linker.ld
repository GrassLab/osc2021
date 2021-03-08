SECTIONS
{
	. = 0x80000;
	.text : { *(.text) }
	/DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
