SECTIONS {
  . = 0x80000;
  .text : { *(.text.boot) *(.text) }
  .data : { *(.data) }
  . = ALIGN(8);
  _bss_start = .;
  .bss : { *(.bss) }
  . = ALIGN(8);
  _bss_end = .;
}

_bss_size = _bss_end - _bss_start;
_stack_start = 0x10000;
