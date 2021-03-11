ENTRY(__relocate)

SECTIONS
{
  /*
    0x80000 is the default address where the kernel image would be loaded into memory
    (for 64bit machine), this address could be modified by varaible `kernel_address` in config.txt
    see: https://www.raspberrypi.org/documentation/configuration/config-txt/boot.md
    for more information
  */
  . = 0x80000;

  /* relocate
  Code that handle code relocation, would move the actual loader program into a higher memory space
  */
  __relocate = .;
  .relocate : { KEEP(*(.text.relocate)) }

  . = ALIGN(4096);
  _begin = .;
  .text : { KEEP(*(.text.boot)) *(.text*) }
  .rodata : { *(.rodata*)}
  .data : { *(.data*) }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;
    *(.bss*)
    __bss_end = .;
  }
  . = ALIGN(4096);
  _end = .;
}

__bss_size = (__bss_end - __bss_start) >> 3;

/* Point to a higher address to
*/
__relocate_target_addr = 0x30000000;