#include <peripheral.h>

#define DATA_READY_BIT (1 << 0)
#define TRANSMITTER_EMPTY_BIT (1 << 5)

void read_uart(char *buffer, int len)
{
    for (int i = 0; i < len; i++)
    {
        while (!(*AUX_MU_LSR_REG & DATA_READY_BIT))
            ;
        buffer[i] = *AUX_MU_IO_REG & 0xff;
    }
}

void print_uart(const char *buffer)
{
    int i = 0;
    while (buffer[i] != '\0')
    {
        while (!(*AUX_MU_LSR_REG & TRANSMITTER_EMPTY_BIT))
            ;
        *(char *)AUX_MU_IO_REG = buffer[i];
        i++;
    }
}

void write_hex_uart(unsigned long num)
{
    int n = 2;
    char buf[20] = "0x";
    char *ptr = (char *)&num;
    for (int i = sizeof(unsigned long) - 1; i >= 0; i--)
    {
        int c = ptr[i] % 16;
        buf[n + 1] = c > 9 ? c - 10 + 'A' : c + '0';
        c = ptr[i] >> 4;
        buf[n] = c > 9 ? c - 10 + 'A' : c + '0';

        n += 2;
    }
    print_uart(buf);
}

void mini_uart_init() {
    int r;
    *GPFSEL1 &= 0xfffc0fff;
    *GPFSEL1 |= 0x00012000;

    *GPPUD = 0;
    r = 150; while(r--);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while(r--);
    *GPPUDCLK0 = 0;

    *AUXENB = 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR_REG = 6;
    *AUX_MU_CNTL_REG = 3;
}