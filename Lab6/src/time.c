#include "uart.h"
void time(){
	uart_printf("start\n");
	//core timeer
	asm volatile("mov x0, 1             \n");

	//let timer can be used
    asm volatile("msr cntp_ctl_el0, x0  \n");
    //timer frequence
    asm volatile("mrs x0, cntfrq_el0    \n");
   	asm volatile("add x0, x0, x0        \n");
    asm volatile("msr cntp_tval_el0, x0 \n");
    uart_printf("############\n");
    asm volatile("mov x0, 2             \n");
    asm volatile("ldr x1, =0x40000040   \n");
    asm volatile("str w0, [x1]          \n");
    //asm volatile("eret              \n");


    //from_el1_to_el0
    //asm volatile("mov x0, 0             \n");
    //asm volatile("msr spsr_el1, x0      \n");
    //asm volatile("mov x0, sp            \n");
   	//asm volatile("msr sp_el0, x0        \n");
    //asm volatile("msr elr_el1, x30      \n");
    //asm volatile("eret                  \n");
}