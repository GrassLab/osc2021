#include "printf.h"
#include "sys.h"

int main(int argc, char **argv) {
    printf("[mmap - illegal_wrtie]\n");
    char *ptr2 = call_sys_mmap(NULL, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("mmap allocated address: 0x%x\n", ptr2);
    printf("%d\n", ptr2[1000]);  // should be 0
    ptr2[0] = 1;                 // illegal_write. (Should be seg fault, if no writable flag in mmap)
    printf("%d\n", ptr2[0]);     // not reached
    return 0;
}