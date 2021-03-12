ENTRY(_start)
SECTIONS
{
    . = 0x70000;
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
        *(.bss  .bss.* .gnu.linkonce.b.*)
        __bss_end = .;
    }

    _end = .;
}
__bss_size = (__bss_start - __bss_end) >> 3;
__stack_base = __text_start << 1;
bootloader_start = __text_start;
bootloader_end = _end;
