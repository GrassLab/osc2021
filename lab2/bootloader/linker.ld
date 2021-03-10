ENTRY(_start)

SECTIONS
{
  . = 0x60000;

  _stext = .;
  .text :
  { 
    *(.text)
    *(.rodata)
  }
  . = ALIGN(0x1000);
  _stext = .;

  _sdata = .;
  .data :
  {
    *(.data*)
  }
  . = ALIGN(0x1000);
  _edata = .;

  _sbss = .;
  .bss :
  {
    *(.bss)
  }
  . = ALIGN(0x1000);
  _ebss = .;

  . = 0x70000;
  _estack = .;

  . = 0x80000;
  _kernel = .;
}
