#include "uart.h"
#include "math.h"
#include "printf.h"

int main()
{
    // set up serial console
    uart_init();
    init_printf(0, putc);

    // say hello
    printf("Hello World!\n");
    printf("Send the size of the kernel image first.\n");

    char size_char = '0';
    int size = 0, exponent = 0;

    while (1)
    {
        size_char = uart_getc();

        // stop sending
        if (size_char == '\n')
            break;
        // throw away junk bits in the buffer
        if (size_char < 48 || size_char > 57)
            continue;

        size += (size_char - '0') * pow(10, exponent);
        exponent++;

        printf("%c %d\n", size_char, size);
    }

    printf("You can start to send the kernel image now!\n");
    char *kernel = (char *)0x80000;

    for (int i = 0; i < size; i++)
    {
        printf("%d\n", i);
        *kernel++ = uart_getc();
    }

    printf("Jump to the kernel...\n");

    asm volatile(
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        "mov x30, 0x80000; ret");

    return 0;
}