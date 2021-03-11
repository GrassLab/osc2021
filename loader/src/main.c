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
    printf("You can start to send the kernel image now!\n");

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

    char *kernel = (char *)0x80000;
    char temp;
    // int sum = 0;

    for (int i = 0; i < size; i++)
    {
        temp = uart_getc();
        *kernel = temp;
        printf("%c", temp);
        kernel++;
    }

    printf("Jump to the kernel...\n");

    asm volatile(
        "mov x0, x10\n"
        "mov x1, x11\n"
        "mov x2, x12\n"
        "mov x3, x13\n");

    printf("Registers saved...\n");

    asm volatile(
        "mov x30, 0x80000\n"
        "ret\n");

    printf("After kernel jump...\n");

    // void *kernel_ptr = (void *)0x80000;
    // goto *kernel_ptr;

    // asm volatile(
    //     "mov x0, x10\n"
    //     "mov x1, x11\n"
    //     "mov x2, x12\n"
    //     "mov x3, x13\n"
    //     "mov x30, 0x80000\n"
    //     "ret\n");

    // printf("After kernel jump...\n");

    return 0;
}