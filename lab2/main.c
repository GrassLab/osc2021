#include "uart.h"
#include <stdio.h>

#define MMIO_BASE       0x3F000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020
#define TMP_KERNEL_ADDR  0x100000
extern char bss_end[];
extern char start_begin[];


int strcmp(char *input,char *command)
{
    while(*input != '\0')
    {
        if(*input != *command)
        {
            return 0;
        }
        input = input + 1;
        command = command +1;
    }
    if (*input != *command)
    {
        return 0;
    }
    return 1;
}

void uart_read_line(char *input)
{
    int index = 0;
    char c;
    while(c != '\n')
    {
        c = uart_getc();
        uart_send(c);

        if(c != '\n')
        {
            input[index] = c;
            index =index + 1;
        }
        else
        {
            input[index] = '\0';
        }
    }

}

void hello()
{
    uart_puts("Hello World!\n");
}

void help()
{
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
}

void reboot()
{
    unsigned int r;
    r = *PM_RSTS; 
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;  //debugger, watchdog, software
    *PM_WDOG = PM_WDOG_MAGIC | 10;  //used 20 bit count down
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;//had a watchdog full reset. clear this flag by writing 0 this field
}
void load_img()
{

    int index = 0;
    int tmp;
    char c;
    do
    {
        c=uart_getc();
        tmp = c - '0';
        //uart_send(c);
        if(c != '\n')
            index = index*10 + tmp;
    }while(c != '\n');


    uart_puts("Start of loading img\n");
    int i = 0;
    volatile unsigned char *kernel = (void *)(long)0x00080000;
    int t = index;
    while(index)
    {
        c=uart_getc();
        //uart_send(c);
        kernel[i] = c;
        //uart_send(kernel[i]);
        index = index - 1;
        i = i + 1 ;
        //int tt = i;
        // char *q ;
        // char *num;
        // q = num;
        // while(tt >= 1)
        // {
        //     *num = tt%10 + '0' ;
        //     tt =tt/10;
        //     num = num +1 ;
        // }
        // *num = '\0';
        // uart_puts("     ");
        // num = num - 1;
        // while( q <= num)
        // {
        //     uart_send(*num);
        //     num = num - 1;
        // }
        // uart_puts("\n");

        // if (index == 0)
        // {
        //     uart_puts("zero!!!!\n");
        // }
    }       
    //uart_puts("End loadimg !!!\n");
    // for (int i=0; i<t;i++)
    // {
    //     uart_send(kernel[0]);
    // }
    //uart_puts("\n");
    uart_puts("End of loading img\n");
    for (int i=0;i<10000;i++)
    {
        asm volatile("nop");
    }
    kernel = (void *)(long)0x00080000;
    asm volatile("br %0" : "=r"((unsigned long int*)kernel));

}

void copy_and_load_img()
{
    char *kernel = start_begin;
    char *end = bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
    uart_puts("Start of copy kernel\n");
    while (kernel <= end) 
    {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("End of copy kernel\n");
    load_img();
}


void path()
{
    int flag = 0;
    int first = 1;
    volatile unsigned char *kernel ;
    int file_size = 0;
    int name_size = 0;
    while(1)
    {   
        if (first)
        {
            kernel = (void *)(long)(0x20000000);
        }
        else
        {
            kernel = kernel + file_size + name_size ;
            file_size = 0;
            name_size = 0;
        }

        for(int i=0;i<8;i++)
        {
            if(kernel[i+14] != '0')
            {
                break;
            }

            if((i == 7) && (kernel[i+14] == '0'))
            {
                flag = 1;
            }
        }
        for(int i=0;i<8;i++)
        {
            int file_num=0;
            int name_num=0;
            if('A' <= kernel[54 + i] && kernel[54 + i] <= 'F')
            {
                file_num = ((int)kernel[54 + i]) - 'A' + 10;
            }
            else
            {
                file_num = ((int)kernel[54 + i]) - '0' ;
            }

            if('A' <= kernel[94 + i] && kernel[94 + i] <= 'F')
            {
                name_num = ((int)kernel[94 + i]) - 'A' + 10;
            }
            else
            {
                name_num = ((int)kernel[94 + i]) - '0' ;
            }
            file_size = file_size * 16 + file_num;
            name_size = name_size * 16 + name_num;
        }
        name_size = name_size + 0x6E;

        if((file_size % 4) != 0)
            file_size = file_size + (4 - (file_size % 4));

        if((name_size % 4) != 0)
        {
            name_size = name_size + (4 - (name_size % 4));
        }

        if (flag == 1)
        {
            break;
        }
        first = 0;
        char *filename = kernel + 0x6E;
        uart_puts(filename);
        uart_puts("\n");
    }

}


void main()
{
    uart_init();
    char *welcome = "--------------------lab2--------------------\n";
    uart_puts(welcome);

    while(1)
    {
        uart_puts("#");
        char input[100];
        //uart_puts("before");
        uart_read_line(input);
        //uart_puts("after");
        uart_send('\r');

        if (strcmp(input, "hello"))
        {
            hello();
        }
        else if(strcmp(input,"help"))
        {
            help();
        }
        else if(strcmp(input,"reboot"))
        {
            reboot();
        }
        else if(strcmp(input,"loadimg"))
        {
            load_img();
        }
        else if(strcmp(input,"copy_loadimg"))
        {
            copy_and_load_img();
        }
        else if(strcmp(input,"path"))
        {
            path();
        }
        else
        {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
        

}

/*


070701/0322052D/000041ED/000003E8/000003E8/00000002/6046/01CE/00000000/00000008/00000011/00000000/00000000/00000002/00000000.#jump2
070701/03220530/000081A4/000003E8/000003E8/00000001/6040/8E40/000009B4/00000008/00000011/00000000/00000000/00000007/00000000 gpio.h#
070701/03220538/000081A4/000003E8/000003E8/00000001/6040/8E41/00000858/00000008/00000011/00000000/00000000/00000007/00000000/ main.c
070701/03220535/000081A4/000003E8/000003E8/00000001/6040/8E41/00000201/00000008/00000011/00000000/00000000/00000009/00000000 Makefileall
070701/03220534/000081ED/000003E8/000003E8/00000001/6040/8E40/00000618/00000008/00000011/00000000/00000000/0000000C/00000000 kernel8.img
070701/0322052E/000081A4/000003E8/000003E8/00000001/6040/8E40/000000EF/00000008/00000011/00000000/00000000/0000000A/00000000 linker.ld
070701/03220533/000081A4/000003E8/000003E8/00000001/6040/8E40/0000005C/00000008/00000011/00000000/00000000/00000007/00000000 uart.hvoid 
070701/03220537/000081A4/000003E8/000003E8/00000001/6040/8E41/000001CE/00000008/00000011/00000000/00000000/00000008/00000000 start.S
070701/03220539/000081A4/000003E8/000003E8/00000001/6040/BA55/00000E8E/00000008/00000011/00000000/00000000/00000007/00000000 uart.c
070701/00000000/00000000/00000000/00000000/00000001/0000/0000/00000000/00000000/00000000/00000000/00000000/0000000B/00000000 TRAILER!!!

	   struct cpio_newc_header {
		   char	   c_magic[6];
		   char	   c_ino[8];
		   char	   c_mode[8];
		   char	   c_uid[8];
		   char	   c_gid[8];
		   char	   c_nlink[8];
		   char	   c_mtime[8];
		   char	   c_filesize[8];
		   char	   c_devmajor[8];
		   char	   c_devminor[8];
		   char	   c_rdevmajor[8];
		   char	   c_rdevminor[8];
		   char	   c_namesize[8];
		   char	   c_check[8];
	   };
       */
       
