#include "exception.h"
#include "uart.h"
#include "util.h"
#include "timer.h"

// http://www.zencoder.info/2020/01/15/get-current-el-in-arm64/ 
/*
*	show current exception level
*   
*   use "mrs x CurrentEL" get current exception level and put on x
*   
*	CurrentEL: register for get current exception level
*   %0       : output operand
*	"=r" (current_el): output to current_el
*	": : :"memory" : gcc compiler fence
*
*	要右移2，請參見http://www.lujun.org.cn/?p=1676
*
*   4    3    2    1    0
*	---------------------
*	|    | EL |    | 	|
*	---------------------
*
*	因為current_el在第3第2位，所以要右移2才是我們要的值
*/
void show_current_el()
{
	unsigned long long current_el;
	asm volatile("mrs %0, CurrentEL\n\t" : "=r" (current_el) : : "memory");
	
	char buf[16] = {0};
	uart_putstr("currentEL is ");
	unsignedlonglongToStr(current_el >> 2, buf);
	uart_putstr(buf);
}

/*
*	exception_entry
*   
*   The method use for synchronous excpetion handle
*   
*	In required 1-3
*   Only needs to print the content of [spsr_el1], [elr_el1], and [esr_el1] in the exception handler
*
*   [spsr_el1]:  current processor’s state
*	[elr_el1]:  the exception return address
*	[esr_ell]:  if exception is synchronous exception or an an SError interrupt, save cause of that exception
*
*   note: if core timer is enable in requirement 2, we don't show exception info message
*/
void exception_entry() 
{
	// if core timer enable, not show message
	unsigned long long cntp_ctl_el0;
	asm volatile("mrs %0, cntp_ctl_el0" : "=r"(cntp_ctl_el0));	
	if(cntp_ctl_el0 != 0)
		return;
	
	unsigned long long spsr_el1, elr_el1, esr_el1;

	asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
	asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
	asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));

	char buf[16] = {0};
	uart_putstr("print exception info: \n");
	uart_putstr("SPSR_EL1: ");
	unsignedlonglongToStrHex(spsr_el1, buf);
	uart_putstr(buf);
	uart_putstr("\n");
	uart_putstr("ELR_EL1: ");
	unsignedlonglongToStrHex(elr_el1, buf);
	uart_putstr(buf);
	uart_putstr("\n");
	uart_putstr("ESR_EL1: ");
	unsignedlonglongToStrHex(esr_el1, buf);
	uart_putstr(buf);
	uart_putstr("\n\n");
}

/*
*	no_exception_handle
*   
*   The method use for not implement excpetion handle
*   
*	From TA,
*   In this lab, we only focus on Synchronous and IRQ exceptions.
*
*   and only handle exception vector table's
*	1. Exception from the currentEL while using SP_ELx
*	2. Exception from a lower EL and at least one lower EL is AARCH64.
*
*   so, other exception or other table in exception vector table, we don't implement
*/
void no_exception_handle()
{
	
}

/*
*	lowerEL_irq_interrupt
*   
*   The method use for lowerEL irq interrupt handle
*	1. distinguish interrupt source : core timer or uart
*	2. use different exception_handle handle different interrupt
*  
*   irq : https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
*   p.16
*   Core0 interrupt source & CNTHPIRQ interrupt
*
*	critical section
*   3. before exception_handle, disable interrupt
*   4. after exception_handle, enable interrupt    
*
*/
void lowerEL_irq_interrupt()
{
	disable_interrupt();
	
	unsigned int core_irq = (*CORE0_IRQ_SOURCE & (1 << 1));	
	unsigned int uart_irq = (*IRQ_PENDING_1 & AUX_IRQ);
	
	// ARM core timer irq interrupt
	if(core_irq)
		core_timer_handle();
	// uart irq interrupt
	else if(uart_irq)
		uart_interrupt_handler();
	
	enable_interrupt();
}

void currentEL_irq_interrupt()
{
	disable_interrupt();
	
	unsigned int core_irq = (*CORE0_IRQ_SOURCE & (1 << 1));	
	unsigned int uart_irq = (*IRQ_PENDING_1 & AUX_IRQ);
	
	// ARM core timer irq interrupt
	if(core_irq)
		timout_handle();
	// uart irq interrupt
	else if(uart_irq)
		uart_interrupt_handler();
	
	enable_interrupt();
}

/*
*	enable_interrupt
*   
*   DAIFClr register : enable D,A,I,F
*	0xf = 1111 ,  represent D,A,I,F enable all   
*
*/
void enable_interrupt() 
{ 
	asm volatile("msr DAIFClr, 0xf"); 
}

/*
*	disable_interrupt
*   
*   DAIFSet register : disable D,A,I,F
*	0xf = 1111 ,  represent D,A,I,F disable all   
*
*/
void disable_interrupt() 
{ 
	asm volatile("msr DAIFSet, 0xf");
}