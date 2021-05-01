#include "uart.h"
#include "shell.h"
#include "mm.h"
#include "irq.h"
#include "sched.h"

void kernel_main(){
	timeout_event_init();
    // set up uart
    uart_init();
	// init memory system
    init_memory_system();
	
	enable_interrupt();
	
    // start shell
    shell();
    //while (1) {
        // Once we call schedule for the first time, since current points to the
        // init task, we become the init task. So, everytime init runs, it's
        // actually running this while loop. Here, we're voluntarily giving up
        // the cpu.
        //schedule();
    //}
}
