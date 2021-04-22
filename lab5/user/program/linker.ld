SECTIONS
{
  . = 0x0;
  .text : 
  {
	 KEEP(*(.text.boot))
  	 *(.text) 
  }
  .data :
  {
  	*(.data)
  }
  .rodata :  
  {
    *(.rodata)
  }
  .bss ALIGN(0x10): 
  {
    __bss_begin = .; 
  	 *(.bss);
     *(COMMON);
    __bss_end = ALIGN(0x10);
  }
}
