SECTIONS
{
	. = 0x80000;
	_relocate_start = .;
	.text :
	{
		KEEP(*(.text.boot))
		*(.text)
	  _bootloader_addr = .;
		KEEP(*(.text.bootloader))
	}
	.rodata :
	{
		*(.rodata)
	}
	.data :
	{
		*(.data)
	}
	.bss ALIGN(8) (NOLOAD):
	{
		_bss_start = .;
		*(.bss)
		_bss_end = .;
	}
	_relocate_end = .;
}

_bss_size = (_bss_end - _bss_start) >> 3;
_bootloader_offset = (_bootloader_addr - _relocate_start);
