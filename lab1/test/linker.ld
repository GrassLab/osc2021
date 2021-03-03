MEMORY
{
    code_ram   (rwx!rw) : ORIGIN = 0x80000, LENGTH = 0x2000
    data_ram   (rw!x)  : ORIGIN = 0x82000, LENGTH = 0xA000
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
