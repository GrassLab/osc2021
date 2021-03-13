#include "uart.h"
#include "string.h"
#include "cpio.h"

int hex2int(char *hex)
{
    int value = 0;
    for (int i = 0; i < 8; i++)
    {
        // get current character then increment
        char byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        value = (value << 4) | (byte & 0xF);
    }
    return value;
}

int round2four(int origin, int option)
{
    int answer = 0;

    switch (option)
    {
    case 1:
        if ((origin + 6) % 4 > 0)
            answer = ((origin + 6) / 4 + 1) * 4 - 6;
        else
            answer = origin;
        break;

    case 2:
        if (origin % 4 > 0)
            answer = (origin / 4 + 1) * 4;
        else
            answer = origin;
        break;

    default:
        break;
    }

    return answer;
}

void read(char **address, char *target, int count)
{
    while (count--)
    {
        *target = **address;
        (*address)++;
        target++;
    }
}

void parse_cpio()
{

    char *ramfs = (char *)0x20000100;

    struct cpio_newc_header temp;

    char file_name[100];
    char file_content[100];

    int file_size = 0, name_size = 0;

    uart_puts("Start parsing...\n");
    uart_send('\n');

    while(1)
    {
        temp = (const struct cpio_newc_header){0};

        read(&ramfs, temp.c_magic, 6);
        ramfs += 48;
        read(&ramfs, temp.c_filesize, 8);
        ramfs += 32;
        read(&ramfs, temp.c_namesize, 8);
        ramfs += 8;

        name_size = round2four(hex2int(temp.c_namesize), 1);
        file_size = round2four(hex2int(temp.c_filesize), 2);

        read(&ramfs, file_name, name_size);
        read(&ramfs, file_content, file_size);

        file_name[name_size] = '\0';
        file_content[file_size] = '\0';

        if ((strcmp(file_name, "TRAILER!!!") == 0))
            break;

        uart_puts(file_name);
        uart_send('\n');
        uart_puts(file_content);
        uart_send('\n');
    }
}