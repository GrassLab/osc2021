#include "inc/uart.h"

void dumpState(){
	unsigned long esr,elr,spsr;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\n":"=r"(spsr):);

	uart_printf("--------------------\n");
	uart_printf("SPSR: 0x%x\n",spsr);
	uart_printf("ELR: 0x%x\n",elr);
	uart_printf("ESR: 0x%x\n",esr);
	uart_printf("--------------------\n");
	//uart_printf("Exception Return Address: 0x%x\n",elr);
	//uart_printf("Exception Class: 0x%x\n",(esr>>26)&0x3f);//0x15 for svc inst
	//uart_printf("Instruction Specific Syndrome: 0x%x\n",esr&0x1ffffff);//issued imm value for svc inst
}

void exception_handler(){
	dumpState();
}

void interrupt_handler(){
	asm volatile("mrs x0, cntfrq_el0	\n");
	asm volatile("add x0, x0, x0		\n");
	asm volatile("msr cntp_tval_el0, x0	\n");
	unsigned long cntpct,cntfrq,tmp;
	asm volatile("mrs %0, cntpct_el0	\n":"=r"(cntpct):);
	asm volatile("mrs %0, cntfrq_el0	\n":"=r"(cntfrq):);

	tmp=cntpct*10/cntfrq;
	uart_printf("--------------------\n");
	uart_printf("Time Elapsed: %d.%ds\n",tmp/10,tmp%10);
	uart_printf("--------------------\n");
}

void error_handler(){
	dumpState();
	uart_printf("unknown exception...\n");
	while(1){}
}