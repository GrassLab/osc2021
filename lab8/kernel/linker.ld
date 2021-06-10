SECTIONS
{
  . = 0xffff000000000000; /* kernel space */
  . += 0x80000; /* kernel load address */
  //. = 0x80000;
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
