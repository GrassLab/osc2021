ENTRY(_start)

MEMORY
{
    ROM (rx)  : ORIGIN = 0x80000, LENGTH = 0x2000
    RAM (rwx) : ORIGIN = 0x82000, LENGTH = 0x2000
}

SECTIONS
{
    . = 0x80000;
    .text :
    {
        *(.text)
    } >ROM
    .data :
    {
        *(.data)
    } >RAM
    __bss_begin = .;
    .bss :
    {
        *(.bss)
        __bss_end = .;
        __stack_top = . + 0xffff;
    } >RAM
}
