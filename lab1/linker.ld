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

  . = ALIGN(0x8);

  _bss_begin = .;
  .bss : {
  	*(.bss)
  }
  _bss_end = .;

}
