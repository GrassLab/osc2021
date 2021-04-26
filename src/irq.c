#include "irq.h"


void set_x0(unsigned long val){
	unsigned long* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
	task[13] = val; //user reg[0]
}

void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1){
    //printf("????\n");
    return;
}
void sync_el0_handler(){
    asm volatile("\
		str x0,[sp,-8]\n\
		str x1,[sp,-16]\n\
	"::);
	unsigned long x0,x1;
	asm volatile("\
		ldr %0,[sp,-8]\n\
		ldr %1,[sp,-16]\n\
	":"=r"(x0),"=r"(x1):);

    unsigned long esr,svc;
	asm volatile("mrs %0, esr_el1\n":"=r"(esr):);
	if(((esr>>26)&0x3f)==0x15){
        svc = esr & 0x0ffff;
        if(svc == 0){
            printf("svc 0\n");
            return;
        }
        else if(svc == 1){
            unsigned long pid = get_pid();
            //uart_printint(pid);
            set_x0(pid);
            return;
        }
        else if(svc == 2){
            unsigned long ret=uart_gets((char*)x0,(int)x1);
			set_x0(ret);
			return;
        }
        else if(svc == 3){
            uart_puts((char*)x0);
            set_x0(x1);
            return;
        }
        else if(svc == 4){
            exec((char*)x0, (char**) x1);
            set_x0(0);
            return;
        }
        else if(svc == 5){
            exit();
            return;
        }
        else{
            uart_puts("hh\r\n");
        }
	}else{
		printf("???? esr_el1...\n");
		while(1){}
	}
}
void handle_el1_irq(){
    unsigned int irq_sig2 = get32(IRQ_PENDING_1);
    unsigned int irq_sig1 = get32(CORE0_INTERRUPT_SOURCE);
    if(irq_sig2 & AUX_IRQ){
        // uart_puts("uart el1\r\n");
        uart_irq();
        enable_irq();
    }
    if(irq_sig1 == 2){ // CNTPNIRQ
        // uart_puts("core timer el1\r\n");
        disable_irq();
        el1_timer_irq();
        enable_irq();
    }
    return;
}
void handle_el0_irq(){
    unsigned int irq_sig2 = get32(IRQ_PENDING_1);
    unsigned int irq_sig1 = get32(CORE0_INTERRUPT_SOURCE);
    if(irq_sig2 & AUX_IRQ){
        // uart_puts("uart\r\n");
        uart_irq();
        enable_irq();
    }
    if(irq_sig1 == 2){
        // uart_puts("core time\r\n");
        el0_timer_irq();
        enable_irq();
    }
    // else disable_irq();
    return;
}

