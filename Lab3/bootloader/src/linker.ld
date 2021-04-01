
ENTRY(_start)
SECTIONS
{
  . = 0x80000;
  _begin = .;
  
  
  .text :
  {
 
    *(.text)
  }

  .rodata : 
  { 
    *(.rodata) 
  }
  
  .data :
  {
    *(.data)
  }
	
  . = ALIGN(0x8);
  .bss (NOLOAD) :
  {
  
    __bss_start = .;

    *(.bss*)
  
    __bss_end = .;
  }
  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
__load_size = (_end - _begin) >> 3;
