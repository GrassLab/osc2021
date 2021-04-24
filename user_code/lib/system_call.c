#include "system_call.h"
int getpid(){
    return 0;
}
size_t uart_read(char buf[], size_t size){
    return 0;
}
size_t uart_write(const char buf[], size_t size){
    return 0;
}
int exec(const char *name, char *const argv[]){
    return 0;
}
void exit(){
    return;
}
int fork(){
    int res;
    asm volatile("svc 6\n\
                  mov %0, x0\n\
                ":"=r"(res):);
    return res;
}