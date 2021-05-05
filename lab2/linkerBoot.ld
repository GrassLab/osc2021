ENTRY(_boot)

MEMORY
{
    code_ram   (rwx) : ORIGIN = 0x00060000, LENGTH = 0x2000
    data_ram   (rwx) : ORIGIN = 0x00062000, LENGTH = 0xA000
}

SECTIONS
{
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)

    } > code_ram

    .data :
    {
        *(.rodata*)
        *(.data)
        . = ALIGN(0x8);
        __bss_start = .;
        *(.bss)
        __bss_end = .;
        __stack_top = . + 0x1000;
    } > data_ram
}
__bss_size = (__bss_end - __bss_start) >> 3;
