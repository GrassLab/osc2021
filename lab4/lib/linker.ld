__heap_size     = 0x00012000;
__stack_size    = 0x00012000;
MEMORY
{
    code_ram   (rwx) : ORIGIN = 0x00080000, LENGTH = 0x6000
    data_ram   (rwx) : ORIGIN = 0x00086000, LENGTH = __stack_top - 0x00086000
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
        *(.rodata*)
        *(.data)
        . = ALIGN(0x8);
        bss_begin = .;
        *(.bss)
        bss_end = .;
        __heap_start = .;
        __stack_top = . + __stack_size + __heap_size;
    } > data_ram
}

