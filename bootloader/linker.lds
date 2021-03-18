ENTRY(_start)

MEMORY
{
    ROM (rx)  : ORIGIN = 0x60000, LENGTH = 0x2000
    RAM (rwx) : ORIGIN = 0x62000, LENGTH = 0x2000
}

SECTIONS
{
    . = 0x80000;
    .text :
    {
        KEEP(*(.text.boot)) *(.text)
    } >ROM
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) } >ROM
    .data :
    {
        *(.data)
    } >RAM
    . = ALIGN(0x8);
    __bss_begin = .;
    .bss :
    {
        *(.bss)
        __bss_end = .;
        __stack_top = . + 0xffff;
    } >RAM

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
