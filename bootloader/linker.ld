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
        . = ALIGN(16);
        *(.bss  .bss.* .gnu.linkonce.b.*)
        . = ALIGN(16);
    }
    __bss_end = .;

    _end = .;
}
__bss_size = (__bss_end - __bss_start);
__stack_base = __text_start << 1;
bootloader_start = __text_start;
bootloader_end = _end;
bootloader_size = _end - __text_start;
bootloader_offset = 0x80000 - __text_start;
