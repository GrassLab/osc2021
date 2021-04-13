#include "exception.h"
#include "str_tool.h"
void dumpstate(){
    unsigned long esr,elr,spsr,level;
	asm volatile("mrs %0, esr_el1	\r\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\r\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\r\n":"=r"(spsr):);
	asm volatile("mrs %0, currentEL	\r\n":"=r"(level):);	//lsr x0, x0, #2
	asm volatile("lsr %1, %0, #2	\r\n":"=r"(level),"=r"(level):);
	//asm volatile("lsr %[res], %[_in], #2	\r\n":[res]"=r"(level),[_in]"=r"(level):);

	printf("--------------------\r\n");
	printf("SPSR: 0x%x\r\n",spsr);
	printf("ELR: 0x%x\r\n",elr);
	printf("ESR: 0x%x\r\n",esr);
	printf("Current Exception level is: %d\r\n",level);
	printf("--------------------\r\n");
}
void exception_handler(){
    printf("entering exception handler\r\n");
    dumpstate();
}