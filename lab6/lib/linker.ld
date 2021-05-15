__heap_size     = 0x00002000;
__stack_size    = 0x00002000;
MEMORY
{
    code_ram   (rwx) : ORIGIN = 0x00080000, LENGTH = 0x10000
    data_ram   (rwx) : ORIGIN = 0x00090000, LENGTH = __stack_top - 0x00090000
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
        . = ALIGN(0x10);
        __heap_start = .;
        . = __heap_start + __stack_size + __heap_size;
        . = ALIGN(0x10);
        __stack_top = .;
    } > data_ram
}

