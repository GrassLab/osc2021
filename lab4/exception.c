//#include "uart.h"
#include "allocater.h"
static int time = 0;
void timer_init()
{
	unsigned long int count,freq;
	asm volatile ("mrs %0, cntpct_el0" :"=r" (count));
	asm volatile ("mrs %0, cntfrq_el0" :"=r" (freq));
	//cntpct_el0,cntfrq_el0
	time = count / freq;
}
void irq_router(void)
{
	//spsr_el1 : 0x0000000060000000
    //elr_el1 : 0x0000000000081640
    //esr_el1 : 0x0000000000000000

	uart_puts("core time : ");
	print_int(time);
	uart_puts("\n");
	time = time + 2;
	core_timer_handler();
}
void exception_handler (void)
{
    //spsr_el1 : 0x00000000200003C0
    // elr_el1 : 0x0000000000081480
    // esr_el1 : 0x0000000056000000



    uart_puts("exception handler!!!\n");

    //spsr_el1, elr_el1, and esr_el1 
    unsigned long int spsr,elr,esr;
    asm volatile ("mrs %0, spsr_el1\n" :"=r" (spsr));//200003C0  --> 2 is C bit 
    asm volatile ("mrs %0, elr_el1\n" :"=r" (elr));
    asm volatile ("mrs %0, esr_el1\n" :"=r" (esr));
	    
	//int iss = esr & 0x01ffffff;

	//print_int(iss);
	//uart_puts("\n");

	//if(iss==0x1)
	//{
	uart_puts("     spsr_el1 : ");
	print_mem(spsr);
	uart_puts("\n");
		
	uart_puts("      elr_el1 : ");
	print_mem(elr);
	uart_puts("\n");

	uart_puts("      esr_el1 : ");
	print_mem(esr);
	uart_puts("\n");  
	//}
}
void not_implemented ()
{
  uart_puts ("function not implemented!\n");
  while (1);
}