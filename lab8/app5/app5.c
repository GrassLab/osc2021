#include "inc/syscall.h"

int global=100;

int main(void){
	int local=0;
	if(sys_fork()==0){
		sys_fork();
		sys_fork();
		while(local<10){
			uart_printf("pid: %d, local: 0x%x %d, global: 0x%x %d\n",getpid(),&local,local,&global,global);
			local++;
			global--;

            for(int i =0;i < (1000000); ++i){}
		}
	}else{
		int* a=0;
		uart_printf("%d\n",*a);
		uart_printf("Should not be printed\n");
	}
	cur_exit();
}
