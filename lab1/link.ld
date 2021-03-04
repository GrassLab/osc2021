ENTRY(_start)
SECTIONS
{
    . = 0x80000;
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
    . = ALIGN(32);
    __stack_bottom = .;
    . = . + 0x80000;
    __stack_top = .;
    _end = .;
}

__bss_size = (__bss_end - __bss_start)>>3;
