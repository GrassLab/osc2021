ENTRY(_start)
SECTIONS
{
    . = 0x80000;
    _begin = .;
    .text : { 
      KEEP(*(.text.boot)) 
      *(.text)
    }
    .rodata : { 
      *(.rodata)
    }

    .data : {
      *(.data .data.* .gnu.linkonce.d*)
    }
    .bss ALIGN(16) (NOLOAD) : {
        __bss_start = .;
        *(.bss .bss.*)
        __bss_end = .;
    }
    _end = .;
}

__stack_top = 0x70000;
__relocate_point = 0x70000;
__bss_size = (__bss_end - __bss_start)>>3;
