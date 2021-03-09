SECTIONS
{
  . = 0x80000;
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
  .bss ALIGN(0x10) : 
  {
    _bss_begin = .; 
  	*(.bss)
    _bss_end = .;
  }

  
  

}
