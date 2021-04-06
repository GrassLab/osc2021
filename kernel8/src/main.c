#include "uart.h"
#include "shell.h"

#include "mm.h"

void kernel_main(){
    // set up uart
    uart_init();
	// init memory system
    init_memory_system();

    // start shell
    shell();
}
