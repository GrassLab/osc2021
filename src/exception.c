#include "printf.h"
#include "entry.h"
#include "timer.h"
#include "exception.h"
#include "uart.h"

extern int isTimerMultiplexingEventIRQ;

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void sync_exc_router(unsigned long spsr, unsigned long elr, unsigned long esr)
{
    printf("In sync_exc_router, System register info:\n");
    printf("spsr_el1:  0x%x\n", spsr);
    printf("elr:  0x%x\n", elr);
    printf("esr:  0x%x\n\n", esr);

	int EC = esr >> 26; // Read exception class in bits[31:26] (Excpetion source)
	switch (EC) {
    case 0b010101: // SVC instruction execution in AArch64 state
        sync_svc_handler(spsr, elr, esr);
        break;
    default:
        printf("Unknow synchronous exception source!\n");
        break;
	};
}

void sync_svc_handler(unsigned long spsr, unsigned long elr, unsigned long esr)
{
    int ISS = esr & 0xFFFFFF;
    switch (ISS) {
    case 1:
        printf("[cpio svc]\n");
        break;
    case 2:
        core_timer_enable();
        printf("[Core timer] interrupt enabled\n");
        break;
    case 3:
        core_timer_disable();
        printf("[Core timer] interrupt disabled\n");
    break;
    };
}

void irq_exc_router()
{
	// Identify IRQ source (check p16 in QA7_rev3.4)
    unsigned int src = *CORE0_IRQ_SRC;
    unsigned int irq_basic1_pending  = *IRQ_PENDING_1;
    // printf("In IRQ exception router\n");
    // printf("Core IRQ source: 0x%x\n", src);
    // printf("irq_basic1_pending: 0x%x\n", irq_basic1_pending);

    // irq_basic1_pending
    if (irq_basic1_pending & AUX_IRQ) {
        //printf("irq_basic1_pending\n");
        uart_irq_handler();
    }
    // ARM Core Timer Interrupt
    else if (src & (1<<1)) {
        if (isTimerMultiplexingEventIRQ) {
            // core_timer_handler();
            timerEvents_irq_handler();
        }
        else {
            printf("Core timer interrupt!\n");
            core_timer_handler();
            print_timestamp();
            
        }
    } else {
        printf("Unknown IRQ source\n");
    }
}

void print_timestamp()
{
	unsigned long int cnt_freq, cnt_tpct;
    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r" (cnt_freq),  "=r" (cnt_tpct)
        :
    );
    unsigned long int timestamp = cnt_tpct / cnt_freq;
    printf("timestamp: %u\n", timestamp);
    return;
}

void print_pstate_interrupt_mask_bits()
{
    unsigned int DAIF_bits = 0;
    asm volatile(
        "mrs %0, DAIF  \n\t"
        : "=r" (DAIF_bits)
    );

    printf("DAIF Bits: 0x{%x}\n", DAIF_bits);
}