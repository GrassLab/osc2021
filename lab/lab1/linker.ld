_heap_size = 0x200;
_stack_size = 0x800;

SECTIONS
{
  . = 0x80000;
  .text :
  {
    *(.text)
  }
  .rodata :
  {
    *(.rodata)
    rodata_end = .;
  }
  .data : AT (rodata_end)
  {
    data_top = .;
    *(.data)
  }
  data_size = SIZEOF(.data);
  data_load_start = LOADADDR(.data);
  .bss :
  {
    _bss_top = .;
    *(.bss)
  }
  bss_size =  SIZEOF(.bss);
  .heap :
  {
    . = ALIGN(4);
    _heap_top = .;
    . = . + _heap_size;
    . = ALIGN(4);
    _heap_bottom = .;
  }
  .stack :
  {
    . = ALIGN(4);
    _stack_bottom = .;
    . = . + _stack_size;
    . = ALIGN(4);
    _stack_top = .;
  }
}