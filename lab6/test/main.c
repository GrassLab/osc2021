#include "uart.h"
#include "utils.h"
#include "reboot.h"
#include "page_malloc.h"
#include "string.h"
#include "thread.h"
#include "tmpfs.h"

void terminal(){
    char input[20];
    char buffer[1000];

    while(1) {
        uart_puts("#");
        
        shell(input);

        if(!strcmp(input, "hello")) {
            uart_puts("Hello World!\n");
        }
        else if(!strcmp(input, "help")) {
            uart_puts("hello: print Hello World!\n");
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot system\n");
        }
        else if(!strcmp(input, "reboot")) {
            reset(20000);
            uart_puts("input c to cancel reset\n");
            shell(input);
            if(!strcmp(input, "c")) {
                cancel_reset();
            }
        }
        else if(!strcmp(input, "xcpt")) {            
            from_el1_to_el0(0x3c0);
            xcpt_test();
        }
        else if(!strcmp(input, "time")) {                        
            core_timer_enable();
            from_el1_to_el0(0x0);            
        }
        else if(!strcmp(input, "loadimg")) {
            loadimg();
        }
        else if(!strcmp(input, "ls")) {
            ls(input,0);
        }
        else if(!strcmp(input, "alloc_init")) {
            uart_puts("input low  address(hex): ");
            shell(input);
            unsigned int addr_low = atoi(input, 16);
            uart_puts("input high address(hex): ");
            shell(input);
            unsigned int addr_high = atoi(input, 16);
            alloc_page_init(addr_low, addr_high);
        }
        else if(!strcmp(input, "alloc_page")) {
            uart_puts("input page size: ");
            shell(input);
            unsigned int page_size = atoi(input, 10);
            alloc_page(page_size);
        }
        else if(!strcmp(input, "free_page")) {
            uart_puts("input page address(hex): ");
            shell(input);
            unsigned int page_address = atoi(input, 16);
            uart_puts("input page size: ");
            shell(input);
            unsigned int free_size = atoi(input, 10);
            free_page(page_address, free_size);
        }
        else if(!strcmp(input,"lab5-1")){
			threadTest1();
		}else if(!strcmp(input,"lab5-2")){
			threadTest2();
		}
        else if(!strcmp(input,"lab6-1")){
			char buf[100];
			//RW test
			file* f=vfs_open("dir/dirdir/f3",0);
			for(int i=0;i<10;++i)vfs_write(f,"12345",5);
			vfs_close(f);
			f=vfs_open("dir/dirdir/f3",0);
			int n=vfs_read(f,buf,100);
			buf[n]=0;
			uart_printf("%d: %s\n",n,buf);
			vfs_close(f);

			//create test
			file* a=vfs_open("dir/hello",O_CREAT);
			file* b=vfs_open("dir/world",O_CREAT);
			vfs_write(a,"Hello ",6);
			vfs_write(b,"World!",6);
			vfs_close(a);
			vfs_close(b);
			b=vfs_open("dir/hello",0);
			a=vfs_open("dir/world",0);
			int sz;
			sz=vfs_read(b,buf,100);
			sz+=vfs_read(a,buf+sz,100);
			buf[sz]=0;
			uart_printf("%s\n",buf);//should be Hello World!
			vfs_close(a);
			vfs_close(b);

			//ls test
			f=vfs_open("dir",0);
			while(1){
				n=vfs_read(f,buf,100);
				if(n==0)break;
				buf[n]=0;
				uart_printf("...%s\n",buf);
			}
			vfs_close(f);
		}
        else if(strcmp(input,"lab6-2")==0){
			threadTest();
		}
        else if(strcmp(input,"archive")==0){
			dumpArchive();
		}
        else {
            //if(!ls(input,1))
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");           
        }
    }
}

void main() {
    print_welcome(1);
    alloc_page_init(0x10000000, 0x20000000);
    //allocator_init();
    file_operations fops;
	tmpfs_fopsGet(&fops);
	vfs_init(tmpfs_Setup,fops.write,fops.read);
    terminal();
}
