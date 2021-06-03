#include "fork.h"
#include "utils.h"
#include "sched.h"
#include "printf.h"
#include "mm.h"
#include "peripherals/mini_uart.h"
#include "vfs.h"

void sys_write(char * buf)
{
	printf(buf);
}

int sys_fork()
{
	return copy_process(0, 0, 0);
}

void sys_exit() 
{
	exit_process();
}

int sys_remain_page_num() 
{
	return 	get_remain_num();
}

int sys_task_id() 
{
	return current->task_id;
}

int sys_read(unsigned long buff_addr, int size) {
	int i = 0;
	char tmp;
	while(1) {
		if((get32(AUX_MU_LSR_REG)&0x01) == 0)
			continue;
		tmp = get32(AUX_MU_IO_REG)&0xFF;
		*((char *)buff_addr + i) = tmp;
		i++;
		if (i == size) break;
	}
	return i;
}

void sys_print_buddy() {
	print_all_buddy();
}

int sys_file_open(const char* pathname, int flags) {
	char *buff = 0xffff000030000000;
	memzero(buff, 100);
	strcpy(buff, pathname);
	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	int ret = user_open(buff, flags);
	set_pgd(current->mm.pgd);
	return ret;
}

int sys_file_read(int file_index,void* buf, int len) {
	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	int ret;
	char *buff = 0xffff000030000000;
	memzero(buff, 100);
	ret = user_read(file_index, buff, len);
	set_pgd(current->mm.pgd);
	strcpy(buf, buff);
	return ret;
}


void sys_file_write(int file_index, const void* buf, int len) {
	// store buf
	char *buff = 0xffff000030000000;
	memzero(buff, 100);
	strcpy(buff, buf);
	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	user_write(file_index, buff, len);
	set_pgd(current->mm.pgd);
	return;
}

void sys_file_close(int file_index) {
	asm volatile ("adrp x0, idmap_dir");
	asm volatile ("msr ttbr0_el1, x0");
	user_close(file_index);
	set_pgd(current->mm.pgd);
	return;
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit, sys_remain_page_num, \ 
								sys_task_id, sys_read, sys_print_buddy, sys_file_open, sys_file_read, sys_file_write, sys_file_close};
