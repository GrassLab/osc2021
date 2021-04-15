#include "uart.h"
#include "allocater.h"
#include <stdio.h>
#include <stdlib.h>

#define MMIO_BASE       0x3F000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020
#define TMP_KERNEL_ADDR  0x100000
#define TMP_USER_ADDR   0x200000
extern char bss_end[];
extern char start_begin[];

struct filenode 
{ 
    char* file_addr; 
    char* data_addr;
    int data_size;
};
static struct filenode f[100];
static int NUMBER_OF_CPIOFILE;

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
        kernel[i] = c;
        index = index - 1;
        i = i + 1 ;
    }       
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
    //load_img();
    void (*func_ptr)() = load_img;
    unsigned long int original_function_address = (unsigned long int)func_ptr;
    void (*call_function)() = (void (*)(char *))(original_function_address - (unsigned long int)start_begin + TMP_KERNEL_ADDR);
    call_function();
}

void init_cpio()
{
    int flag = 0;
    int first = 1;
    volatile unsigned char *kernel ;
    int file_size = 0;
    int name_size = 0;
    NUMBER_OF_CPIOFILE = 0;
    while(1)
    {   
        if (first)
        {
            kernel = (void *)(long)(0x08000000);  
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
        f[NUMBER_OF_CPIOFILE].data_size = file_size;
        char *filename = kernel + 0x6E;
        char *data = kernel + name_size;
        f[NUMBER_OF_CPIOFILE].file_addr = filename;
        f[NUMBER_OF_CPIOFILE].data_addr = data;
        NUMBER_OF_CPIOFILE = NUMBER_OF_CPIOFILE + 1;
    }
}
int path(char* input)
{
    if(strcmp(input,"ls"))
    {
        for(int i=0; i<NUMBER_OF_CPIOFILE;i++)
        {
            print_mem(f[i].data_addr);
            uart_puts("  ");
            uart_puts(f[i].file_addr);
            uart_puts("\n");
        }
        return 1;
    }

    for(int i=0; i<NUMBER_OF_CPIOFILE;i++)
    {
        if(strcmp(input,f[i].file_addr))
        {
            for(int j=0;j<f[i].data_size;j++)
            {
                if(f[i].data_addr[j]=='\n')
                    uart_send('\r');
                uart_send(f[i].data_addr[j]);
            }
            uart_puts("\n");
            return 1;
        }
    }
    return 0;
    
}

unsigned long my_atoi(char* c,int t)
{
    unsigned long tmp=0;
    char *kernel = c;
    for(int i=0; i < t; i++)
    {
        int a=0;
        a = (kernel[0] & 0x000000ff);
        tmp = tmp*256 + a;
        // i = i - 1;
    }

    unsigned long tt = tmp;
    if(tt == 0)
        uart_puts("ZERO");
    while(tt>0)
    {
        //char c= ((tt%10) + '0') ;
        tt = tt/10;
        uart_send('a');
    }
    uart_puts("\n");



    return tmp;
}

void dtb()
{
    struct fdt_header 
    {
        unsigned long magic;
        unsigned long totalsize;
        unsigned long off_dt_struct;
        unsigned long off_dt_strings;
        unsigned long off_mem_rsvmap;
        unsigned long version;
        unsigned long last_comp_version;
        unsigned long boot_cpuid_phys;
        unsigned long size_dt_strings;
        unsigned long size_dt_struct;
    }header;
    unsigned long  a ;

    asm volatile ("mov %0, x0" : "=r"(a));

    char *dtb_kernel=a;
    for(int i=0;i<50;i++)
        uart_send(dtb_kernel[i]);

    dtb_kernel = a;

    // header.magic = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.totalsize = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.off_dt_struct = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.off_dt_strings = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.off_mem_rsvmap = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.version = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.last_comp_version = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.boot_cpuid_phys = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.size_dt_strings = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;
    // header.size_dt_struct = my_atoi(dtb_kernel,4);
    // dtb_kernel = dtb_kernel + 4;

    // dtb_kernel = a + header.off_dt_struct;
    // if(header.magic == 0xd00dfeed)
    //     uart_puts("test dtb\n");

    // unsigned long tt = header.magic;

    // while(tt>0)
    // {
    //     char c=(tt%10) + '0' ;
    //     tt = tt/10;
    //     uart_send(c);
    //     uart_puts("\n");
    // }
    
    //uart_puts(dtb_kernel);
    // if((int)kernel[] == 0)
    // {
    //     uart_send('2'); 
    // }
}
void mem()
{
    unsigned long int mem_size=0,page_size,final_size=-1;
    char size[100];
    uart_puts("How many size (4KB): ");
    uart_read_line(size);
    uart_send('\r');
    int index = 0;
    while(size[index]!='\0')
    {
        mem_size = mem_size * 10 + (size[index] - '0');
        index = index + 1;
    }
    final_size = my_quar(2,mem_size);
    unsigned int addr = require_buddy_mem(final_size);

    char c[20];
    int t = 0;

    if(addr == -1)
    {
        return ;
    }

    uart_puts("memory allocate address : (");
    print_mem(addr);
    uart_puts(" )   ");
    while(addr>0)
    {
        c[t] = '0' + addr %10;
        addr = addr / 10;
        t = t + 1;
    }
    while(t--)
    {
        uart_send(c[t]);
    }
    uart_puts("\n");

    
    uart_puts("memory allocate size (4KB): ");
    int allocate = my_pow(2,final_size);
    char c1[20];
    int t1 = 0;
    while(allocate > 0)
    {
        c1[t1] = '0' + allocate % 10;
        allocate = allocate / 10;
        t1 = t1 + 1;
    }
    while(t1--)
    {
        uart_send(c1[t1]);
    }
    uart_puts("\n");
    lookuppage();

}
void release()
{
    char c1[20],c2[20];
    unsigned int addr=0,size=0;

    uart_puts("Release memory address : ");
    uart_read_line(c1);
    uart_send('\r');
    int index = 0;
    while(c1[index]!='\0')
    {
        addr = addr * 10 + (c1[index] - '0');
        index = index + 1;
    }

    uart_puts("Release memory size (4KB): ");
    uart_read_line(c2);
    uart_send('\r');
    index = 0;
    while(c2[index]!='\0')
    {
        size = size * 10 + (c2[index] - '0');
        index = index + 1;
    }
    size = my_quar(2,size);
    free_buddy_mem(addr,size);
    lookuppage();
}

void mymalloc()
{
    int size=0,number=0;
    char c[10],c1[10];
    uart_puts("How many size (Bytes): ");
    uart_read_line(c);
    uart_send('\r');
    int index = 0;
    while(c[index]!='\0')
    {
        size = size * 10 + (c[index] - '0');
        index = index + 1;
    }
    uart_puts("How many number of mem_pool : ");
    uart_read_line(c1);
    uart_send('\r');
    index = 0;
    while(c1[index]!='\0')
    {
        number = number * 10 + (c1[index] - '0');
        index = index + 1;
    }

    for (int i=0;i<number;i++)
    {
        my_malloc(size);
    }
    lookuppool();
}
void myfree()
{
    unsigned long int addr=0; 
    char c[30];
    uart_puts("free address (decimal): ");
    uart_read_line(c);
    uart_send('\r');
    int index = 0;
    while(c[index]!='\0')
    {
        addr = addr * 10 + (c[index] - '0');
        index = index + 1;
    } 
    my_free(addr);
    lookuppool();
}

void copy_cpio(char *input)
{
    int tmp = -1;//file number in cpio
    for(int i=0; i<NUMBER_OF_CPIOFILE;i++)
    {
        if(strcmp(input,f[i].file_addr))
        {
            tmp = i;
        }
    }
    //print_int(tmp);
    //TMP_USER_ADDR
    int i = 0;
    volatile unsigned char *kernel = (char*)TMP_USER_ADDR;
    int index = f[tmp].data_size;
    while(index)
    {
        kernel[i] = f[tmp].data_addr[i];
        index = index - 1;
        i = i + 1 ;
    } 
}

void main()
{
    uart_init();
    char *welcome = "--------------------lab4--------------------\n";
    uart_puts(welcome);
    //init_header();
    //init_pool();
    //init_header();
    init_cpio();
    unsigned long int pc = 0;
    asm volatile("adr %0, ." : "=r"(pc)); // get cpu count
    //print_mem(pc);
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
            print_int(NUMBER_OF_CPIOFILE);
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
        else if(strcmp(input,"dtb"))
        {
            dtb();
        }
        else if(strcmp(input,"t"))
        {
            init_header();
        }
        else if(strcmp(input,"mem"))
        {
            mem();
        }        
        else if(strcmp(input,"rel"))
        {
            release();
        }        
        else if(strcmp(input,"lookpage"))
        {
            lookuppage();
        }   
        else if(strcmp(input,"malloc"))
        {
            mymalloc();
        }        
        else if(strcmp(input,"free"))
        {
            myfree();
        }        
        else if(strcmp(input,"lookpool"))
        {
            lookuppool();
        }       
        else if(strcmp(input,"try"))
        {
            init_header();
        }
        else if(strcmp(input,"cpio"))//load user program
        {
            copy_cpio("kernel8.img");
            // = TMP_USER_ADDR;
            //volatile unsigned char *kernel = (char*)TMP_USER_ADDR;
            //asm volatile("br %0" : "=r"((unsigned long int*)kernel));
            from_el1_to_el0(0x3c0,TMP_USER_ADDR);
        }
        else if(strcmp(input,"timer"))//timer
        {
            timer_init();
            core_timer_enable();
            from_el1_to_el0(0x0,pc);
        }       
        else if(strcmp(input,"demo1"))//el1->el0
        {
            from_el1_to_el0(0x3c0,pc);
        }
        else if(strcmp(input,"svc"))//svc 
        {
            asm volatile("svc #0");
        }
        // else if(strcmp(input,"demo3"))
        // {
        //     uart_interupt_init();
        // }
        else
        {
            if(! path(input))
            {
                uart_puts("Error: ");
                uart_puts(input);
                uart_puts(" command not found! Try <help> to check all available commands\n");
            }
        }
    }
        

}

/*
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
       
