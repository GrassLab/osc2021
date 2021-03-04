SECTIONS
{
	. = 0x80000;
	.text :
	{
		KEEP(*(.text.boot))
		*(.text)
	}
	.rodata :
	{
		*(.rodata)
	}
	.data :
	{
		*(.data)
	}
	.bss ALIGN(16) (NOLOAD):
	{
		_bss_start = .;
		*(.bss)
		_bss_end = .;
	}
}

_bss_size = (_bss_end - _bss_start) >> 3;
