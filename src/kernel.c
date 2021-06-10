#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "mm.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"
#include "slab.h"
#include "vfs.h"
#include "tmpfs.h"
#include "sdhost.h"
#include "fat32.h"

extern unsigned long user_begin;
extern unsigned long user_end;
extern void user_process();

void kernel_process(){
	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	unsigned long begin = (unsigned long)&user_begin;
	unsigned long end = (unsigned long)&user_end;
	unsigned long process = (unsigned long)&user_process;
	int err = do_exec(begin, end - begin, process - begin); // virtual address of begin & size
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	} 
}

void kernel_main(){
	uart_init();
	init_printf(NULL, putc);
	printf("init irq & vector\r\n");
	irq_vector_init();
	printf("init buddy system\r\n");
	init_buddy_system();
	printf("init object allocator\r\n");
	init_obj_allocator();
	printf("init sdcard\r\n");
	sd_init();
	//init_fs();
	printf(" _   _  ______   ___   _    ___  ____   ____ \r\n");
	printf("| \\ | |/ ___\\ \\ / / | | |  / _ \\/ ___| / ___|\r\n");
	printf("|  \\| | |    \\ V /| | | | | | | \\___ \\| |   \r\n");    
	printf("| |\\  | |___  | | | |_| | | |_| |___) | |___\r\n");
	printf("|_| \\_|\\____| |_|  \\___/   \\___/|____/ \\____|\r\n\r\n");
	printf("\r\n-------------------------\r\n");
    printf("=     Kernel  Space     =\r\n");
    printf("-------------------------\r\n");	
	enable_core_timer();
	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);//PF_KTHREAD : process is created in kernel space
	enable_irq();
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}
	while (1){
		free_zombie_task();
		schedule();
	}	
}
