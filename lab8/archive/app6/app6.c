#include "inc/syscall.h"

int global=100;

int main(void){
	int local=0;
	if(fork()==0){
		fork();
		fork();
		while(local<10){
			uart_printf("pid: %d, local: 0x%x %d, global: 0x%x %d\n",getpid(),&local,local,&global,global);
			local++;
			global--;
			delay(1000000);
		}
	}else{
		int* a=0;
		uart_printf("%d\n",*a);
		uart_printf("Should not be printed\n");
	}
	exit();
}