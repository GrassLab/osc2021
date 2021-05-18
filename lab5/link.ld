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
    . = . + 0x800000;
    __stack_top = .;
    .text : {
      *(.exec.app)
    }
    _exec_app_img = .;
    _end = .;
}

__bss_size = (__bss_end - __bss_start)>>3;
