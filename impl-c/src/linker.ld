SECTIONS
{
  /*
    0x80000 is the default address where the kernel image would be loaded into memory
    (for 64bit machine), this address could be modified by varaible `kernel_address` in config.txt
    see: https://www.raspberrypi.org/documentation/configuration/config-txt/boot.md
    for more information
  */
  . = 0x80000;
  .text :
  {
    KEEP(*(.text.boot)) *(.text)
  }
  .rodata :
  {
    *(.rodata)
  }
  .data :
  {
    *(.data)
  }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;
    *(.bss .bss.*)
    __bss_end = .;
  }
  _end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;
__stack_top = _start;