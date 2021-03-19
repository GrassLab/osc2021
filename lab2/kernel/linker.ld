ENTRY(_start)

SECTIONS
{
  . = 0x80000;

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

  _svector = .;
  .vector :
  {
    *(.vector)
  }
  . = ALIGN(0x1000);
  _evector = .;

  . = 0x200000;
  _estack = .;

  . = 0x20000000;
  _cpio_buf = .;
}
