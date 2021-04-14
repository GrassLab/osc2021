ENTRY(_start)

SECTIONS
{
  . = 0x80000;

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

  . = 0x200000;
  _el2_stack = .;

  _el2_stack_size = 0x10000;
  _el2_stack_end = _el2_stack + _el2_stack_size;

  /DISCARD/ : {
    *(.comment*)
    *(.rela*)
    *(.note*)
    *(.eh_frame)
    *(.iplt)
    *(.igot*)
  }
}