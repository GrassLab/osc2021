# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`309551092`| `samuelyen36` | `顏劭庭` | samuelyen.cs05@g2.nctu.edu.tw |

## How to build
Makefile is ready to be used.
> make


## How to run
I add another command to run the code in qemu.
> make run

## How to burn it into pi3
I follow the instruction in lab0, loading bootcode.bin, fixup.dat and start.elf into SD card, and also put our kernel8.img into Rpi3. Then, we can start the board to check the result.

## Architecture
* linker.ld: linker script of this homework.
* boot.S: bootloader, clear bss and setup stack pointer and jump to kernel function.
* mm.S: defines memzero function.
* kernel.c: our kernel function, I call simple shell here.
* shell.c: Where I implement the simple shell.
* utils.S: defines put32, get32 function and delay function that is used in UART initialization.
* mini_uart.c: initialization function, uart_getc, uart_putc and uart_puts are defined here.
