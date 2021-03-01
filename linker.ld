SECTIONS
{
 . = 0x80000;
 .text : { 
	KEEP(*(.text.boot))
	 *( .text .text.* .gun.linkonce.t*)  
 }
 .rodata : {
	*(.rodata .rodata.* .gnulinonce.r*)
 }
 .data : {
 	*(.data .data.* .gnu.linkonce.d*)
 }
 .bss (NOLOAD) : {
	. = ALIGN(16);
	_bss_start = .;
 	*(.bss .bss.*)
	_bss_end = .;
 }
 _end = .;
}

