_heap_stack_size = 4K;
_save_dts = 0x880000;
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
    _bss_end = .;
  }
  bss_size = (_bss_end - _bss_top) >> 3;
  .heap :
  {
    . = ALIGN(16);
    _heap_top = .;
  }
  .stack :
  {
    . = ALIGN(16);
    . = . + _heap_stack_size;
    _stack_top = .;
  }
  _end = .;
}