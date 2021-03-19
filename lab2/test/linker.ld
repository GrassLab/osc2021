MEMORY
{
    code_ram   (rwx) : ORIGIN = 0x00080000, LENGTH = 0x2000
    data_ram   (rwx) : ORIGIN = 0x00082000, LENGTH = 0xA000
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
        __stack_top = . + 0x1000;
    } > data_ram
}

