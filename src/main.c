#include "uart.h"
#include "shell.h"

int main() {
		uart_init();
		run_shell();
		return 0;
}
