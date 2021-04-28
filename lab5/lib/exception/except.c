#include "../../include/except.h"
#include "../../include/uart.h"
#include "../../include/task.h"

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
void x0Set(unsigned long v){
	unsigned long* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
	task[16]=v;
}

void _except_handler(){
    uart_printf("Sync Exception\n");
    unsigned long esr, svc;
    asm volatile("mrs %0, esr_el1  \n":"=r"(esr):);

    unsigned long x0,x1,x2;
   // asm volatile("str x0,[sp, 0]  \n"::);
   // asm volatile("str x0,[sp, 8]  \n"::);
   // asm volatile("str x0,[sp, 16]  \n"::);

   // asm volatile("ldr %0,[sp, 16]  \n":"=r"(x0):);
   // asm volatile("ldr %0,[sp, 16]  \n":"=r"(x1):);
   // asm volatile("ldr %0,[sp, 16]  \n":"=r"(x2):);

    if(((esr>>26)&0x3f) == 0x15){
        svc = esr & 0x1ffffff;
        switch(svc){
            case 0:
                dumpState();
                break;
            case 1:
                cur_exit();
                break;
            case 2:
                exec((char*)x0,(char**)x1);
                break;
        }


    }
}


void _TODO_(){
    dumpState();
    while(1){}
}
