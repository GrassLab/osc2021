ENTRY(_start)
__heap_start = 0x90000;
__stack_top  = 0xA0000;

MEMORY
{
    code_ram   (rx!rw) : ORIGIN = 0x80000, LENGTH = 0x5000
    data_ram   (rw!x)  : ORIGIN = 0x86000, LENGTH = 0x1A000
}
SECTIONS
{       
    .text :
    {
        boot.o(.text)
        *(.text)

    } > code_ram

    .data :
    {
        *(.data)
        bss_begin = .;
        *(.bss)
        bss_end = .;
        *(.rodata*)
    } > data_ram
}
