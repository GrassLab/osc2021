#include "inc/uart.h"
#include "inc/reboot.h"
#include "inc/mailbox.h"
#include "inc/cpio.h"
#include "inc/allocator.h"
#include "inc/thread.h"
#include "inc/system.h"
#include "inc/shell.h"

void main(){
	uart_init();
	allocator_init();
	shell();
}