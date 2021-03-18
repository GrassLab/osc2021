#include "uart.h"
#include "shell.h"
#include "cpio.h"

int main() {
		//uart_init();
		read_cpio_archive();
		run_shell();
		return 0;
}
