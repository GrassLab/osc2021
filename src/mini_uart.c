#include "mini_uart.h"
#include "ops.h"

void init_uart()
{

}

void send_char(char c)
{
    while(1) {
        if (get32(AUX_MU_LSR_REG) & 0x20) {
            break;
        }
    }

    put32(AUX_MU_IO_REG, c);
}

void send_string(char *str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        send_char(str[i]);
        i++;
    }
}

char receive_char()
{
    while(1) {
        if (get32(AUX_MU_LSR_REG) & 0x01) {
            break;
        }
    }
    return (get32(AUX_MU_IO_REG) & 0xFF);
}