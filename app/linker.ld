ENTRY(_pstart)
SECTIONS
{
    . = 0x00000;
    __text_start = .;
    .text :
    {
        KEEP(*(.text.start))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    .data :
    {
        *(.data .data.* .gnu.linkonce.d*)
    }
    .bss :
    {
        __bss_start = .;
        . = ALIGN(16);
        *(.bss  .bss.* .gnu.linkonce.b.*)
        . = ALIGN(16);
        __bss_end = .;
    }

    _end = .;
}
