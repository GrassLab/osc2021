#include "uart.h"
#include "stddef.h"
#include "stdint.h"
void load(){
	uart_printf("load user prgram\n");

	unsigned long size;

	unsigned char *load_addr = (unsigned char*)0x30000000;

	const char *content = cpio_load_program("app.img",load_addr, cpio_buf);


	uart_printf("start user app\n");
	// change exception level
	// asm volatile("mov x0, 0x3c0  \n"); // disable timer interrupt, enable svn
  	asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
	asm volatile("msr spsr_el1, x0  \n");
	asm volatile("msr elr_el1, %0   \n" ::"r"(load_addr));
	asm volatile("msr sp_el0, %0    \n" ::"r"(load_addr));

	// enable the core timer’s interrupt
	asm volatile("mov x0, 1             \n");
	asm volatile("msr cntp_ctl_el0, x0  \n");
	asm volatile("mrs x0, cntfrq_el0    \n");
	asm volatile("add x0, x0, x0        \n");
	asm volatile("msr cntp_tval_el0, x0 \n");
	asm volatile("mov x0, 2             \n");
	asm volatile("ldr x1, =0x40000040   \n");
	asm volatile("str w0, [x1]          \n");

	asm volatile("eret              \n");
}