SECTIONS
{
  . = 0x00080000;
  .text : { *(.text.startup) }
  . = ALIGN(8);
  .rodata : { *(.rodata) }
}
