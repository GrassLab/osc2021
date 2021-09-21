
SECTIONS
{
    . = 0xFFFF000000000000; /* Upper Virtaul Address (kernel space) */
    . += 0x80000;           /* kernel load address */
    __kernel_start = .;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    . = ALIGN(0x00001000);  /* Page table should be align */ 
    pg_dir = .;             /* Page table starting address */
    .data.pgd : { . += (3 * (1 << 12)); }   /* Add Size of 3 Page table (PGD, PUD, PMD)  */ 
    . = ALIGN(0x00001000);
    __kernel_end = .;
}