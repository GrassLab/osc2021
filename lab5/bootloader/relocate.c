extern unsigned char _begin, _end, __relocate_point, main;
# include "uart.h"
# include "my_math.h"
# include "my_string.h"
# include "relocate.h"

void relocate() {
    //uart_init();
    //uart_puts("relocate\n");
    
    unsigned long kernel_size = (&_end - &_begin);
    unsigned char *new_bl = (unsigned char *)&__relocate_point;
    unsigned char *bl = (unsigned char *)&_begin;

    while (kernel_size--) {
            *new_bl++ = *bl;
            bl++;
            //*bl++ = 0;
        }
        
    //char hex_c[20];
    void (*func_p)(void) = &main;
    //int_to_hex(func_p, hex_c);
    //uart_puts(hex_c);
    //uart_puts("\n");
    func_p -= 0x10000;
    //int_to_hex(func_p, hex_c);
    //uart_puts(hex_c);
    //uart_puts("\n");
    func_p();
    /*
    void (*start)(void) = (void *)&__relocate_point;
    start();
    */
}

void loadimg() {
    long long address = KERNAL_START;

    uart_puts("Send image via UART now!\n");
    uart_flush();
    char p[20];

    // big endian
    int img_size = 0, i;
    for (i = 0; i < 4; i++) {
        img_size <<= 8;
        img_size |= (int)uart_read_raw();
    }
    uart_puts("Image size : ");
    int_to_str(img_size, p);
    uart_puts(p);
    uart_puts("\n");

    // big endian
    int img_checksum = 0;
    for (i = 0; i < 4; i++) {
        img_checksum <<= 8;
        img_checksum |= (int)uart_read_raw();
    }
    uart_puts("Check sum : ");
    int_to_str(img_checksum, p);
    uart_puts(p);
    uart_puts("\n");

    char *kernel = (char *)address;

    for (i = 0; i < img_size; i++) {
        char b = uart_read_raw();
        *(kernel + i) = b;
        img_checksum -= (int)b;
    }


    if (img_checksum != 0) {
        uart_puts("Failed!");
        uart_puts("\n");
        int_to_str(img_checksum, p);
        uart_puts(p);
        uart_puts("\n");
    }
    else {
        uart_puts("Done");
        uart_puts("\n");
        register unsigned int r;
        r = 1000;
        while(r--) { asm volatile("nop"); }
        void (*start_os)(void) = (void *)KERNAL_START;
        start_os();
    }
}
