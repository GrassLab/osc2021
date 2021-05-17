#include <peripheral.h>
#include <stddef.h>

extern char _kernel[];
extern char *_dtb;

void mini_uart_init();
void read_uart(char *buffer, int len);
void print_uart(const char *buffer);
void write_hex_uart(unsigned long num);

void load_kernel()
{
    unsigned int size;
    print_uart("Trying to load kernel.img from UART...\r\n");

    read_uart((char *)&size, 4);
    print_uart("Kernel base: ");
    write_hex_uart((size_t)_kernel);
    print_uart("\r\n");
    print_uart("Kernel size: ");
    write_hex_uart(size);
    print_uart("\r\n");

    read_uart(_kernel, size);
    print_uart("Start kernel\r\n");

    ((void (*)(char *))_kernel)(_dtb);
}

int main()
{
    mini_uart_init();
    load_kernel();
    return 0;
}
