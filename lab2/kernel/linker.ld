SECTIONS
{
  . = 0x00080000;
  .text : { *(.text.startup) }
  . = ALIGN(8);
  .rodata : { *(.rodata) }
  . = ALIGN(8);
  .data : { *(.data) }
  . = ALIGN(8);
  bss_begin = .;
  .bss : { *(.bss*) }
  . = ALIGN(8);
  bss_end = .;
}
