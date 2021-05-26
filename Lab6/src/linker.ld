SECTIONS
{
  . = 0x80000;
  
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  .data :
  {
    *(.data)
  }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;

    *(.bss)

    __bss_end = .;
  }
  _end = .;
  
  . = 0x8000000;
  cpio_buf = .;
}
__stack_top = 0xA0000;
__bss_size = (__bss_end - __bss_start) >> 3;
