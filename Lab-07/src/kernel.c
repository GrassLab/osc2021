#include <stddef.h>
#include <stdint.h>

#include "shell.h"
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

	printf("Init irq\n\r");
	irq_vector_init();
	
	printf("Init memory\n\r");
	init_buddy_system();
	init_obj_allocator();
	
	printf("Init sdcard\n\r");	
	sd_init();
	
	printf("Init file system\n\r");		
	init_fs();


	shell();

/*
	enable_core_timer();
	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	enable_irq();
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}
	while (1){
		free_zombie_task();
		schedule();
	}	
*/

}
