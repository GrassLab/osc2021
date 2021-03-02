SECTIONS
{
  . = 0x0080000;
  .text : { *(.text.startup) }
  . = ALIGN(0x8);
  bss_begin = .;
  .bss : { *(.bss*) }
  . = ALIGN(0x8);
  bss_end = .;
}
