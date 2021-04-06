#include "inc/uart.h"

void dumpState(){
	unsigned long esr,elr;
	asm volatile("mrs %0, esr_el2	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el2	\n":"=r"(elr):);
	uart_printf("Exception Return Address: 0x%x\n",elr);
	uart_printf("Exception Class: 0x%x\n",(esr>>26)&0x3f);//0x15 for svc inst
	uart_printf("Instruction Specific Syndrome: 0x%x\n",esr&0x1ffffff);//issued imm value for svc inst
}

void exception_handler(){
	dumpState();
}

void error_handler(){
	uart_printf("unknown exception...\n");
	while(1){}
}