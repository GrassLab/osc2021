_stack_size = 0xA00;

SECTIONS
{
 . = 0x60000;
 __start_addr = .;
 .text : { 
	*( .text)  
 }
 .rodata : {
	*(.rodata)
 }
 .data : {
 	*(.data)
 }
 .bss (NOLOAD) : {
	. = ALIGN(16);
	__bss_start = .;
 	*(.bss .bss.*)
	__bss_end = .;
 }
 
 .heap : {
	 . = ALIGN(16);
	 __heap_top = .;
	 *(.heap)
 }
 .stack : {
	 . = ALIGN(16);
	. = . + _stack_size;
	. = ALIGN(16);
	__stack_top = .;
 }
 
 __end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;
