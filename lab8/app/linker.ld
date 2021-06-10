ENTRY(main)
SECTIONS
{
    . += 0x80000;
    __kernel_start = .;
    .text : { 
      *(.text.boot) 
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
    __kernel_end = .;
}

