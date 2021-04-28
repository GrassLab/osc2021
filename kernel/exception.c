#include "uart.h"
#include "scheduler.h"

void dumpState() {
    unsigned long spsr_el1, elr_el1, esr_el1;
    asm volatile("mrs %0, spsr_el1 \n":"=r"(spsr_el1):);
    asm volatile("mrs %0, elr_el1 \n":"=r"(elr_el1):);
    asm volatile("mrs %0, esr_el1 \n":"=r"(esr_el1):);

    uart_puts("-----dump state-----\n");
    uart_puts("SPSR_EL1: 0x");
    uart_puts_hex(spsr_el1);
    uart_puts("\n");
    uart_puts("ELR_EL1: 0x");
    uart_puts_hex(elr_el1);
    uart_puts("\n");
    uart_puts("ESR_EL1: 0x");
    uart_puts_hex(esr_el1);
    uart_puts("\n");
    uart_puts("---------------------\n");
    while(1);
}

void dumpTimer() {
    asm volatile("mrs x0, cntfrq_el0 \n");
    asm volatile("add x0, x0, x0 \n"); // 2 second
    asm volatile("msr cntp_tval_el0, x0 \n");
    timer_tick();
    
    unsigned long cntpct, cntfrq, tmp;
    asm volatile("mrs %0, cntpct_el0 \n":"=r"(cntpct):);
    asm volatile("mrs %0, cntfrq_el0 \n":"=r"(cntfrq):);
    tmp = cntpct/cntfrq;
    uart_puts("-----Timer-----\n");
    uart_puts("Time Elapsed: ");
    uart_puts_int(tmp);
    uart_puts("\n");
    uart_puts("---------------\n");
    
}

/*
void el0_svc() {
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

    unsigned long esr, svc; //  store the value in $ESR_EL1, svc number
    asm volatile("mrs %0, esr_el1 \n":"=r"(esr):);

    // check the exception class (esr[31:26])
    if(((esr>>26) & 0x3f) == 0x15) {
        // check the svc number (esr[24:0])
        svc = esr & 0x1ffffff;
        switch(svc) {
            case 0: {
                dumpState();
                return;
            }
            // get pid
            case 1: {
                uart_puts_int(current->id);
                uart_puts("\n");
                return current->id;
            }
            
            // uart_read
            case 2: {
                unsigned long ret = uart_read_buff((char*)x0,(int)x1,1);
            }
            // uart_write
            case 3: {
                uart_puts((char*)x0);
                return;
            }
            // exec
            // exit
            // fork
        }
    }
}
*/