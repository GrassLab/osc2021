ENTRY(_start)

SECTIONS
{
  . = 0xffff000000080000;

  _stext = .;
  .text :
  {
    *(.text.startup)
    *(.text*)
    *(.vector)
    *(.rodata*)
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
    *(.bss*)
  }
  . = ALIGN(0x1000);
  _ebss = .;

  . = 0xffff000000200000;
  _el2_stack = .;

  _el2_stack_size = 0x10000;
  _el2_stack_end = _el2_stack + _el2_stack_size;
}