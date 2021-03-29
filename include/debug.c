#include "debug.h"
#include "data_type.h"
#include "string.h"
#include "uart.h"

void mem_hex_print (unsigned long address, unsigned int num) {
    unsigned long *ptr = 0;
    /* clear least four bits */
    address &= ~(0xF);
    for (int i = 0; i < num; i++) {
        if (!(i % 2)) {
            if (i != 0)
                uart_send("\r\n");
            uart_sendhf(address + 8 * i);
            uart_send(":\t");
        }
        ptr = (unsigned long *) (address + 8 * i);
        uart_sendhf(*ptr);
        uart_send("\t");
    }
    uart_send("\r\n");
}

int mem_print (char *b) {
    unsigned int blen = strlength(b);
    if (blen < 4 || b[0] != 'x' || b[1] != '/')
        return 0;

    int size = 0;
    for (; b[2 + size] >= '0' && b[2 + size] <= '9'; size++);

    if (b[size + 2] == 'l' && b[size + 3] == 'i') {
        int address_start = size + 4;
        for (; b[address_start] == ' '; address_start++);

        char tmp[128];
        size = size + 1 < 128 ? size + 1 : 128;
        strncopy(tmp, &b[2], size);
        u64 c_addr = atoui(&b[address_start]);
        uart_send((char *)c_addr);
        return 1;
    }
    else if (b[size + 2] == 'g' || b[size + 3] == 'x') {
        int address_start = size + 4;
        for (; b[address_start] == ' '; address_start++);

        char tmp[128];
        size = size + 1 < 128 ? size + 1 : 128;
        strncopy(tmp, &b[2], size);

        if (size == 1)
            mem_hex_print(atoui(&b[address_start]), 1);
        else
            mem_hex_print(atoui(&b[address_start]), atoui(tmp));
        return 1;
    }
    return 0;
}
