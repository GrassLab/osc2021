#include "printf.h"
#include "sys.h"

int main(int argc, char **argv) {
    printf("[mmap - illegal_read]\n");
    char* ptr1 = call_sys_mmap((void *)0x1000, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("mmap allocated address: %x\n", ptr1);
    printf("%d\n", ptr1[1000]); // should be 0
    printf("%d\n", ptr1[4097]); // should be segfault

    call_sys_exit();
    return 0;
}