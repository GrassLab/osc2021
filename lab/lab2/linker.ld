_heap_stack_size = 8M;

SECTIONS
{
  . = 0x80000;
  _start = .;
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
  .bss :
  {
    _bss_top = .;
    *(.bss)
  }
  bss_size =  SIZEOF(.bss);
  /* .heap :
  {
    . = ALIGN(8);
    _heap_top = .;
  }
  . = . + _heap_stack_size;
  .stack :
  {
    . = ALIGN(8);
    _stack_top = .;
  } */
  _end = .;
}