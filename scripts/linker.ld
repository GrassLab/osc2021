
SECTIONS
{
    . = 0xffff000000000000;
    . += 0x80000;
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x1000);
    bss_begin = .;
    .bss : { *(.bss*) }
    . = ALIGN(0x1000); 
    bss_end = .;
    . = ALIGN(0x1000); 
    __heap_start = . + (1<<12);
    __stack_top = __heap_start + 0x1000;
}
