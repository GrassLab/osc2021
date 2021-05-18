#include "shell.h"
#include "uart.h"
#include "my_math.h"
#define CMD_LEN 128
int atoi(char* s, int base) {
    int i = 0;
    int num = 0;
    int negative = 0;

    while(s[i] != '\0') {
        if(i == 0 && s[i] == '-') {
            negative = 1;
        }
        if(base == 10) {
            int digit = s[i] - '0';
            num = num*10 + digit;
        }
        if(base == 16) {
            int digit = s[i] >= 'A' ? s[i] - 'A'+ 10 : s[i] - '0';
            num = num*16 + digit;
        }
        i++;
    }
    
    if(negative) return num *= -1;
    return num;
}

int main() {
    uart_init();
    for (int i=0;i<10000;i++)
    {
        asm volatile("nop");
    }
    uart_printf("lab2\n");
    uart_printf("start to load image\n");
    unsigned char input[20];
    uart_read_line_real(input, 0);
    uart_printf("%s", input);
    uart_write('\r');
    int size = atoi(input, 10);

    //size = 6328;
    uart_printf("kernel size = %d\n", size);
    uart_printf("%s", input);
    unsigned char *kernel = (unsigned char *)0x80000;
    for(int i = 0; i < size; i++){
    	unsigned char c = uart_read_real();
        kernel[i] = c;
    }

    uart_printf("end to load\n");
    
    for (int i=0;i<10000;i++)
    {
        asm volatile("nop");
    }
    kernel = (void *)(long)0x00080000;
    asm volatile("br %0" : "=r"((unsigned long int*)kernel));
    
  
}



