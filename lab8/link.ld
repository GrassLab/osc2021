ENTRY(_start)
SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    __kernel_start = .;
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
    .text : {
      *(.exec.app)
    }
    _exec_app_img = .;
    __kernel_end = .;
}

__bss_size = (__bss_end - __bss_start)>>3;
