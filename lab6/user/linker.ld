ENTRY(_start)

SECTIONS
{
  . = 0x0;

  _stext = .;
  .text :
  {
    *(.text*)
    *(.rodata*)
  }
  . = ALIGN(0x1000);

  .data :
  {
    *(.data*)
  }
  . = ALIGN(0x1000);

  .bss :
  {
    *(.bss*)
  }
  . = ALIGN(0x1000);
}