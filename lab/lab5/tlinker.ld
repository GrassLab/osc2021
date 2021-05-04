SECTIONS
{
  . = 0x20000000;
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
  _end = .;
}