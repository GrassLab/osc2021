ENTRY(_start)

MEMORY
{
    ROM (rx)  : ORIGIN = 0x80000, LENGTH = 0x100
    RAM (rwx) : ORIGIN = 0x81000, LENGTH = 0x100
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
    } >RAM
    __bss_end = .;
}
