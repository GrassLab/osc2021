SECTIONS
{
	. = 0x80000;
	.text :
	{
		KEEP(*(.text.boot))
		*(.text)
	}
	_relocate_start = .;
	.text.relocate :
	{
		KEEP(*(.text.bootloader))
		KEEP(*(.text.io))
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
