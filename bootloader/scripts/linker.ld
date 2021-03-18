ENTRY(_start)
__heap_start = 0x90000;
__stack_top  = 0xA0000;

SECTIONS
{
    . = 0x80000;
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) } 
    bss_end = .;
}
