ENTRY(_boot)

__stack_top  = 0x90000;

SECTIONS
{
    . = 0x80000;
    __bootlaoder_start = .;
    .text : { KEEP(*(.text.boot)) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;
}
__bss_size = (__bss_end - __bss_start) >> 3;
__bootloader_size = (__bss_end - __bootlaoder_start) >> 3;