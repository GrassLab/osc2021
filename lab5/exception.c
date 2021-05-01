#include "inc/uart.h"
#include "inc/thread.h"

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

void exception_handler(){
	asm volatile("\
		str x0,[sp,-8]\n\
		str x1,[sp,-16]\n\
		str x2,[sp,-24]\n\
	"::);
	unsigned long x0,x1,x2;
	asm volatile("\
		ldr %0,[sp,-8]\n\
		ldr %1,[sp,-16]\n\
		ldr %2,[sp,-24]\n\
	":"=r"(x0),"=r"(x1),"=r"(x2):);

	unsigned long esr,svc;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	if(((esr>>26)&0x3f)==0x15){
		svc=esr&0x1ffffff;
		if(svc==0){
			dumpState();
			return;
		}else if(svc==1){//getpid
			unsigned long ret=tidGet();

			x0Set(ret);
			return;
		}else if(svc==2){//uart_read
			unsigned long ret=uart_gets((char*)x0,(int)x1,1);

			x0Set(ret);
			return;
		}else if(svc==3){//uart_write
			uart_puts((char*)x0);

			x0Set(x1);
			return;
		}else if(svc==4){//exec
			exec((char*)x0,(char**)x1);

			x0Set(0);
			return;
		}else if(svc==5){//exit
			exit();

			while(1){}
			return;
		}else if(svc==6){//fork
			unsigned long ret=fork();

			x0Set(ret);
			return;
		}else{
			uart_printf("TODO\n");
			return;
		}
	}else{
		uart_printf("unknown esr_el1...\n");
		while(1){}
	}
}

void interrupt_handler(){
	/*
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
	*/
	asm volatile("mrs x0, cntfrq_el0	\n");
	asm volatile("asr x0, x0, 7			\n");// 1/128 second
	asm volatile("msr cntp_tval_el0, x0	\n");
	threadSchedule();
}

void error_handler(){
	dumpState();
	uart_printf("unknown exception...\n");
	while(1){}
}