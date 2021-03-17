SECTIONS
{
  . = 0x80000;
  .text : { KEEP(*(.text.boot)) *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  
  . = ALIGN(0x8);
  __bss_start = .;
  .bss : { *(.bss*) }
  __bss_end = .;
  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;
