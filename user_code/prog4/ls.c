#include "lib/system_call.h"

int main(int argc, char **argv){
    int fd = open(argv[1], 0);
    char name[100];
    int size, id = 0;
    for(int id = 0;; ++id){
        size = list_fs(fd, name, id);
        if(size < 0) break;
        if(size > 0) uart_printf("Name: %s, size: %d\n", name, size);
    }
    exit();
}