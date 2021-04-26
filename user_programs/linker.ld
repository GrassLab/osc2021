SECTIONS
{
  . = 0x80000;
  .text :
  {
    /* always put main function at the start */
    *(.text._runtime)
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
}