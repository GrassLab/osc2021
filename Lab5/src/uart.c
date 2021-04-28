#include "aux.h"
#include "gpio.h"
#include "my_string.h"
void uart_init(){
	*AUX_ENABLES |= 1;
	*AUX_MU_CNTL= 0;
	*AUX_MU_IER = 0;
	*AUX_MU_LCR = 3;
	*AUX_MU_MCR = 0;
	*AUX_MU_BAUD = 270;
	*AUX_MU_IIR = 6;
	

	register unsigned int r = *GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));
    r |= (2 << 12) | (2 << 15);
    *GPFSEL1 = r;

 

    //close pull up & pull down
    *GPPUD = 0;
	
    for(int i = 0; i < 150; i++){
    	//asm assembly do nothing 
    	asm volatile("nop");
    }
	 *GPPUDCLK0 = (1 << 14) | (1 << 15);
    for(int i = 0; i < 150; i++){
    	//asm assembly do nothing 
    	asm volatile("nop");
    }
    *GPPUDCLK0 = 0;

    *AUX_MU_CNTL = 3;

}

char uart_read(){
	do {
        asm volatile("nop");
    	} while (!(*AUX_MU_LSR & 0x01));
	char r = (char)(*AUX_MU_IO);
	if(r == '\r'){
		return '\n';
	}
	else{
		return r;
	}
}

void uart_write(unsigned int c){
	do {
        asm volatile("nop");
    	} while (!(*AUX_MU_LSR & 0x20));
	if(c=='\n'){//convert "\n" to "\r\n"
		uart_write('\r');//recursive
	}
	*AUX_MU_IO = c;
}
int uart_gets(char* s,int size,int display){
	for(int i=0;;++i){
		if(i==size){
			uart_printf("buffer overflow!\n");
			return i;
		}

		s[i]=uart_read();
		if(display)uart_write(s[i]);

		if(s[i]=='\n'){
			s[i]=0;
			return i;
		}
	}
}
unsigned long uart_getX(int display){
	unsigned long ret=0;
	char c;
	while(1){
		c=uart_read();
		if(display)uart_write(c);
		if(c=='\n')break;
		if(c>='0'&&c<='9'){
			ret=ret*16+c-'0';
		}else if(c>='a'&&c<='f'){
			ret=ret*16+c-'a'+10;
		}else if(c>='A'&&c<='F'){
			ret=ret*16+c-'A'+10;
		}
	}
	return ret;
}
void uart_puts(char *s){
	while(*s){
		uart_write(*s);
		s++;
	}
}
void uart_printf(char *fmt, ...){
	__builtin_va_list args;
    __builtin_va_start(args, fmt);

    extern volatile unsigned char _end;

    char* s = (char*)&_end;
    vsprintf(s, fmt, args);

    while (*s) {
        if (*s == '\n') uart_write('\r');
        uart_write(*s++);
    }
}

void uart_flush(){
	while(*AUX_MU_LSR & 0x01){
		*AUX_MU_IO;
	}
}
