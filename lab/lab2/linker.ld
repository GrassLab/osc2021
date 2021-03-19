_heap_stack_size = 8M;
_save_dts = 0x9000000;
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
  bss_size =  SIZEOF(.bss) >> 3;
  .heap :
  {
    . = ALIGN(8);
    _heap_top = .;
  }
  . = . + _heap_stack_size;
  .stack :
  {
    . = ALIGN(8);
    _stack_top = .;
  }
  _end = .;
}