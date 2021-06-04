#include "../include/uart.h"
#include "../include/error.h"
void ERROR(char* s){
    uart_printf("%s",s);
    while(1);
}
