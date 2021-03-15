__process_size = 0x8000;
__dtb_addr = 0x120000;
SECTIONS
{
 . = 0x80000;
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
	. = . + __process_size;
	__stack_top = .;
 }
 
 __end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;
