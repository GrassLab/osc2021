#include "printf.h"
#include "string.h"
#include "cpio.h"
#include "printf.h"
#include "sys_regs.h"

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
        value = (value << 4) | byte;
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

void cpio_parse_header(char **ramfs, char *file_name, char *file_content)
{
    struct cpio_newc_header header;
    int file_size = 0, name_size = 0;

    read(ramfs, header.c_magic, 6);
    (*ramfs) += 48;
    read(ramfs, header.c_filesize, 8);
    (*ramfs) += 32;
    read(ramfs, header.c_namesize, 8);
    (*ramfs) += 8;

    name_size = round2four(hex2int(header.c_namesize), 1);
    file_size = round2four(hex2int(header.c_filesize), 2);

    read(ramfs, file_name, name_size);
    read(ramfs, file_content, file_size);

    file_name[name_size] = '\0';
    file_content[file_size] = '\0';
}

void cpio_ls()
{
    char file_name[100];
    char file_content[1000];

    char *ramfs = (char *)0x8000000;
    // char *ramfs = (char *)0x20000000;

    while (1)
    {
        strset(file_name, '0', 100);
        strset(file_content, '0', 1000);

        cpio_parse_header(&ramfs, file_name, file_content);

        if ((strcmp(file_name, "TRAILER!!!") == 0))
            break;

        printf(file_name);
        printf("\n");
    }
}

void cpio_find_file(char file_name_to_find[])
{
    char file_name[100];
    char file_content[1000];

    char *ramfs = (char *)0x8000000;
    // char *ramfs = (char *)0x20000000;
    int found = 0;

    while(1)
    {
        strset(file_name, '0', 100);
        strset(file_content, '0', 1000);

        cpio_parse_header(&ramfs, file_name, file_content);

        if ((strcmp(file_name, file_name_to_find) == 0))
        {
            found = 1;
            break;
        }
        else if ((strcmp(file_name, "TRAILER!!!") == 0))
            break;
    }

    if (found)
        printf(file_content);
    else
        printf("FILE NOT FOUND!");

    printf("\n");
}

void cpio_run_executable(char executable_name[])
{
    char *ramfs = (char *)0x8000000;
    char file_name[100];

    int file_size, name_size;

    while (1)
    {
        strset(file_name, '0', 100);

        struct cpio_newc_header header;

        read(&ramfs, header.c_magic, 6);
        ramfs += 48;
        read(&ramfs, header.c_filesize, 8);
        ramfs += 32;
        read(&ramfs, header.c_namesize, 8);
        ramfs += 8;

        name_size = round2four(hex2int(header.c_namesize), 1);
        file_size = round2four(hex2int(header.c_filesize), 2);

        read(&ramfs, file_name, name_size);
        ramfs += file_size;

        file_name[name_size] = '\0';

        if (strcmp(file_name, executable_name) == 0)
            break;
        else if ((strcmp(file_name, "TRAILER!!!") == 0))
            return;
    }

    ramfs -= file_size;

    char *program_position = (char *)0x10A0000;

    while(file_size--)
    {
        *program_position = *ramfs;
        program_position++;
        ramfs++;
    }

    asm volatile(
        "mov x0, 0x340          \n\t"
        "msr spsr_el1, x0       \n\t"
        "mov x0, 0x10A0000      \n\t"
        "add x0, x0, 0x78       \n\t"
        "msr elr_el1, x0        \n\t"
        "mov x0, 0x60000        \n\t"
        "msr sp_el0, x0         \n\t"
        "ldr lr, =return_to_el1 \n\t"
        "eret                   \n\t");
}
