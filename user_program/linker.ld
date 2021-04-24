SECTIONS
{
  . = 0x80000;
  __kernel_start = .;
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
  .bss ALIGN(16) (NOLOAD) :
  {
    *(.bss .bss.*)
  }
  . = ALIGN(4096);
  _end = .;
}